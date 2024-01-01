@tool

## A 2D sprite that animates and displays a Spine animation.
##
## In the Spine editor, export a Spine project using binary skeleton data (".skel") with a corresponding texture atlas (".atlas").
##
## SpineSprite is auto-configuring. If you create a SpineSprite and name it "XYZ", the sprite will scan the project folder
## for an "XYZ.skel" skeleton and an "XYZ.atlas" atlas, and auto-assign them to the sprite. If "XYZ.atlas" does not exist
## but there is just one other ".atlas" file, that other ".atlas" file will be assigned.
class_name SpineSprite
extends Node2D

## Emitted with a SpineAnimationEvent object that describes an animation state change.
signal animation_event( e:SpineAnimationEvent )

## Emitted when the definition of this SpineSprite has changed
signal changed

## Emitted when the Spine skeleton has been updated in [method _process]. Useful for precisely synchronizing
## other object positions to point attachments.
signal updated

## [INTERNAL USE]
enum BlendMode
{
	NORMAL   = 0,
	ADDITIVE = 1,
	MULTIPLY = 2,
	SCREEN   = 3
}

const SpineSpriteDefinition = preload( "SpineSpriteDefinition.gd" )
const SpineSpriteFragment = preload( "SpineSpriteFragment.gd" )
const SpineAnimationEvent = preload( "SpineAnimationEvent.gd" )
const SpineSkin = preload( "SpineSkin.gd" )
const SpinePointAttachment = preload( "SpinePointAttachment.gd" )
const screen_shader = preload( "SpineScreenBlendShader.gdshader" )

## A resource that contains references to a pair of [SpineSkeletonResource] and [SpineAtlasResource] resources.
@export var definition:SpineSpriteDefinition :
	set(value):
		if definition != value:
			if definition and definition.changed.is_connected(_on_definition_changed):
				definition.changed.disconnect( _on_definition_changed )

			definition = value

			if definition and not definition.changed.is_connected(_on_definition_changed):
				definition.changed.connect( _on_definition_changed )

			_on_definition_changed()

## Enable if the spine atlas resource is imported with premultiplied alpha enabled.
@export var premultiplied_alpha := false :
	set(value):
		if premultiplied_alpha != value:
			premultiplied_alpha = value
		_prepare_materials()

@export_group("Materials")

## Optional material to use for sprite fragments drawn in normal (mix) blending mode. If left empty then SpineSprite will use a default normal blending material.
@export var normal_material:Material

## Optional material to use for sprite fragments drawn in additive blending mode. If left empty then SpineSprite will use a default additive blending material.
@export var additive_material:Material

## Optional material to use for sprite fragments drawn in multiply blending mode. If left empty then SpineSprite will use a default multiply blending material.
@export var multiply_material:Material

## Optional material to use for sprite fragments drawn in screen blending mode. If left empty then SpineSprite will use a default screen blending material.
@export var screen_material:Material

@export_group("Skin")

@export var default_skin:Dictionary :
	set(value):
		default_skin = value
		if not is_ready(): return
		var skin = SpineSkin.new()
		for name in default_skin:
			if default_skin[name]: skin.add( name )
		set_skin( skin )

@export_group("Animation")

## The master animation time scale. Multiplies any 'time_scale' parameters set for individual animations.
@export var time_scale := 1.0 :
	set(value):
		time_scale = value
		if data: data.set_time_scale( value )

## The default animation that will play when the sprite is instantiated.
var default_animation := "" :
	set(value):
		if default_animation != value:
			default_animation = value
			if Engine.is_editor_hint() and value != "":
				set_animation( value, true )

## A string list of all animation names available to this sprite. Will be 'null' if the sprite
## definition is not configured. Note that this Godot Spine extension adds a special animation
## called "(none)" at the beginning of the list.
var animation_names = null

## Native sprite data that interfaces with the 'spine-cpp' library.
var data:Variant

var _mesh_builder:SurfaceTool = SurfaceTool.new()
var _fragments:Array[SpineSpriteFragment] = []
var _used_fragment_count := 0
var _active_blend_mode := BlendMode.NORMAL

var _materials:Array[Material] = []
var _is_ready := false
var _skin_names = null
var _bone_names = null
var _slot_names = null
var _slots      = null

var _active_animations:Dictionary = {}
var _attachments:Array[SpineSpriteAttachment] = []
var _staged_attachments:Array = []
var _attachments_staged := false

func _ready():
	material = ShaderMaterial.new()
	material.shader = screen_shader

	_configure_resources()
	data = ClassDB.instantiate( "SpineSpriteData" )
	data.configure( self )

	_prepare_materials()

	# Remove any old fragments (I don't *think* this could happen but just in case)
	for child in get_children(true):
		if child is SpineSpriteFragment:
			child.queue_free()

	var skin = default_skin
	if not skin: skin = {}

	var skin_names = get_skin_names()

	for cur_name in skin.keys():
		if not cur_name in skin_names:
			skin.erase( cur_name )  # erase any skin names that no longer exist

	for name in skin_names:
		if not name in skin:
			skin[name] = false

	var cur_skin = _get_skin()
	if cur_skin: skin[ data.get_skin_name(cur_skin) ] = true

	default_skin = skin

func _prepare_materials():
	_materials = []

	if normal_material:
		_materials.push_back( normal_material )
	else:
		var material = CanvasItemMaterial.new()
		if premultiplied_alpha:
			material.blend_mode = CanvasItemMaterial.BlendMode.BLEND_MODE_PREMULT_ALPHA
		_materials.push_back( material )

	if additive_material:
		_materials.push_back( additive_material )
	else:
		var material = CanvasItemMaterial.new()
		material.blend_mode = CanvasItemMaterial.BlendMode.BLEND_MODE_ADD
		_materials.push_back( material )

	if multiply_material:
		_materials.push_back( multiply_material )
	else:
		var material = CanvasItemMaterial.new()
		material.blend_mode = CanvasItemMaterial.BlendMode.BLEND_MODE_MUL
		_materials.push_back( material )

	if screen_material:
		_materials.push_back( screen_material )
	else:
		var material = ShaderMaterial.new()
		material.shader = screen_shader
		_materials.push_back( material )

## Returns true if the named animation is currently playing on any track.
## If 'animation_name' is null, returns true if any animation is playing on any track.
func is_playing( animation_name:Variant=null )->bool:
	if _active_animations:
		if animation_name:
			for i in _active_animations:
				var a = _active_animations[i]
				if a.animation_name == animation_name: return true
		else:
			return _active_animations.size() > 0
	return false

## Returns true if this SpineSprite is ready to update or draw.
func is_ready()->bool:
	if _is_ready: return true
	if not definition or not definition.is_ready(): return false
	if not data or not data.is_ready(): return false
	if not animation_names:
		animation_names = definition.get_animation_names()
		animation_names.push_front( "(none)" )
		notify_property_list_changed()

		if default_animation == "" and animation_names.size() > 1:
			default_animation = animation_names[1];
		else:
			set_animation( default_animation, true )

	data.set_time_scale( time_scale )

	if not _attachments_staged: _stage_attachments()

	if not Engine.is_editor_hint():
		# Don't cache the result in the Editor because the linked resources can be altered.
		_is_ready = true

	return true

## Queues the named animation to play.
func add_animation( name:String, delay:float=0.0, looping:bool=false, track_index:int=0, time_scale:float=1 ):
	if is_ready():
		if name == "(none)":           set_empty_animations()
		elif name in animation_names: data.add_animation( track_index, name, looping, delay, time_scale )

## Queues an empty animation which returns the sprite to its starting pose.
## Use a 'mix_duration' of 0.5 or so for a smooth transition.
func add_empty_animation( delay:float=0.0, mix_duration:float=0.0, track_index:int=0 ):
	if is_ready():
		data.add_empty_animation( track_index, mix_duration, delay )

## If 'tracks' is a list of integers, clears specific animation tracks with those indices.
## If 'tracks' is omitted or null, all animation tracks are cleared.
## Clearing animation tracks freezes the animation in its current state.
## Use [method add_empty_animation] or [method set_empty_animations] instead to return the
## sprite to its starting pose.
func clear_tracks( tracks=null ):
	if is_ready():
		if tracks:
			for track_index in tracks:
				data.clear_track( track_index )
		else:
			data.clear_tracks()

## Returns an integer pointer to the bone with the specified name or 0 if not found.
func find_bone( bone_name:String )->int:
	if is_ready(): return data.find_bone( bone_name )
	else:          return 0

## Returns an integer pointer to the slot with the specified name or 0 if not found.
func find_slot( slot_name:String )->int:
	if is_ready(): return data.find_slot( slot_name )
	else:          return 0

## Returns the position of the specified bone.
func get_bone_position( bone_pointer:int )->Vector2:
	if is_ready(): return data.get_bone_position( bone_pointer )
	else:          return Vector2(0,0)

## Returns the rotation of the specified bone.
func get_bone_rotation( bone_pointer:int )->float:
	if is_ready(): return data.get_bone_rotation( bone_pointer )
	else:          return 0

## Returns an array of all bone names.
func get_bone_names()->Array[String]:
	if _bone_names: return _bone_names

	var result:Array[String] = []
	if not is_ready(): return result

	var bones = data.get_bones()
	for bone in bones:
		result.push_back( data.get_bone_name(bone) )

	result.sort_custom( func(a, b): return a.naturalnocasecmp_to(b) < 0 )

	_bone_names = result
	return result

## Returns a [SpinePointAttachment] or 'null'.
func get_point_attachment( slot_name:String, attachment_name:String )->Variant:
	if not is_ready(): return null
	var attachment_id = data.get_point_attachment( slot_name, attachment_name )
	if not attachment_id: return 0
	return SpinePointAttachment.new( self, attachment_id, slot_name )

## Returns an alphabetized array of all available skin names.
func get_skin_names()->Array[String]:
	if _skin_names: return _skin_names

	var result:Array[String] = []
	if not is_ready(): return result

	var skins = data.get_skins()
	for skin in skins:
		result.push_back( data.get_skin_name(skin) )

	result.sort_custom( func(a, b): return a.naturalnocasecmp_to(b) < 0 )

	_skin_names = result
	return result

## Returns an alphabetized array of all slot names.
func get_slot_names()->Array[String]:
	if _slot_names: return _slot_names

	var result:Array[String] = []
	if not is_ready(): return result

	var slots = get_slots()
	for slot_pointer in slots:
		result.push_back( data.get_slot_name(slot_pointer) )

	result.sort_custom( func(a, b): return a.naturalnocasecmp_to(b) < 0 )

	_slot_names = result
	return result

## Returns an alphabetized array of all slot names.
func get_slots()->Array:
	if _slots: return _slots
	if not is_ready(): return []

	_slots = data.get_slots()
	return _slots

## Returns the position of the specified slot.
func get_slot_position( slot_pointer:int )->Vector2:
	if is_ready(): return data.get_slot_position( slot_pointer )
	else:          return Vector2(0,0)

## Returns the rotation of the specified slot.
func get_slot_rotation( slot_pointer:int )->float:
	if is_ready(): return data.get_slot_rotation( slot_pointer )
	else:          return 0

func register_attachment( attachment:SpineSpriteAttachment ):
	_attachments.push_back( attachment )
	_attachments_staged = false
	_is_ready = false

func deregister_attachment( attachment:SpineSpriteAttachment ):
	_attachments.erase( attachment )
	_attachments_staged = false
	_is_ready = false

## Replaces any existing animation on the specified track.
func set_animation( name:String, looping:bool=false, track_index:int=0, time_scale:float=1 ):
	if is_ready():
		if name == "(none)":           data.set_empty_animation( track_index, 0.5 )
		elif name in animation_names: data.set_animation( track_index, name, looping, time_scale )

## Sets the position of the specified bone.
func set_bone_position( bone_pointer:int, pos:Vector2 ):
	if is_ready(): data.set_bone_position( bone_pointer, pos )

## Sets the rotation of the specified bone.
func set_bone_rotation( bone_pointer:int, radians:float ):
	if is_ready(): data.set_bone_rotation( bone_pointer, radians )

## Sets empty animations on all active tracks, interrupting current animations and
## returning the sprite to its starting pose.
## Use a 'mix_duration' of 0.5 or so for a smooth transition.
func set_empty_animations( mix_duration:float=0.0, tracks=null ):
	if is_ready():
		if tracks:
			for track_index in tracks:
				data.set_empty_animation( track_index, mix_duration )
		else:
			data.set_empty_animations( mix_duration )

## Sets the sprite to use the specified skin name. The skin must have already been
## defined in the Spine editor.
func set_skin( new_skin:Variant ):
	if is_ready():
		if new_skin is SpineSkin: data.set_skin( new_skin.elements )
		else:                     data.set_skin( new_skin )

#-------------------------------------------------------------------------------

func _enter_tree():
	renamed.connect( _configure_resources )

func _exit_tree():
	if renamed.is_connected( _configure_resources ):
		renamed.disconnect( _configure_resources )

func _process( _dt:float ):
	if is_ready():
		data.update( _dt )
		_used_fragment_count = 0

		_mesh_builder.clear()
		_mesh_builder.begin( Mesh.PRIMITIVE_TRIANGLES )

		data.draw( _mesh_builder, _construct_fragment )

		while _used_fragment_count < _fragments.size():
			_fragments[_used_fragment_count].visible = false
			_used_fragment_count += 1

		updated.emit()

func _construct_fragment( texture_index:int, blend_mode:BlendMode ):
	var atlas = definition.atlas
	if texture_index < atlas.textures.size():
		var texture = atlas.textures[texture_index]
		#var normal_map = atlas.normal_maps[texture_index] if texture_index < atlas.normal_maps.size() else null

		var fragment:SpineSpriteFragment
		if _used_fragment_count <  _fragments.size():
			fragment = _fragments[ _used_fragment_count ]
		else:
			fragment = SpineSpriteFragment.new()
			add_child( fragment )
			_fragments.push_back( fragment )
		_used_fragment_count += 1

		fragment.visible = true
		fragment.update_mesh( _mesh_builder, _materials[int(blend_mode)], texture )

	_mesh_builder.clear()
	_mesh_builder.begin( Mesh.PRIMITIVE_TRIANGLES )

func _get_skin()->int:
	if not is_ready(): return 0
	return data.get_skin()

func _handle_animation_event( type:SpineAnimationEvent.Type, track_entry_id:int, animation_name:String, track_index:int,
		user_event_name:Variant ):
	var e = SpineAnimationEvent.new( type, track_entry_id, animation_name, track_index, user_event_name )

	match e.type:
		SpineAnimationEvent.Type.START:
			_active_animations[e.track_entry_id] = e
		SpineAnimationEvent.Type.COMPLETE:
			pass  # not the end if looping
		SpineAnimationEvent.Type.USER_EVENT:
			pass
		_:
			if _active_animations:
				_active_animations.erase( e.track_entry_id )

	animation_event.emit( e )

func _on_definition_changed():
	_reset()

func _configure_resources():
	if definition or not Engine.is_editor_hint(): return

	var viewport = get_viewport()
	if not viewport: return

	var child_count = viewport.get_child_count()
	var scene = viewport.get_child( child_count - 1 )

	if not _configure_resources_with_folder( scene.scene_file_path.get_base_dir() ):  # Try scene folder first
		_configure_resources_with_folder( "res://" )  # Then root resource folder

func _configure_resources_with_folder( folder:String )->bool:
	var def_name = name + ".tres"
	var res = _find_resource( folder, func(filepath):return filepath.get_file()==def_name )
	if res is SpineSpriteDefinition:
		prints( "Auto-configuring SpineSprite %s with SpineSpriteDefinition %s" % [name,res.resource_path] )
		definition = res
		return true

	var skeleton_name = name + ".skel"
	var skeleton = _find_resource( folder, func(filepath):return filepath.get_file()==skeleton_name )
	if not skeleton:
		skeleton_name = name + ".spine-json"
		skeleton = _find_resource( folder, func(filepath):return filepath.get_file()==skeleton_name )
	if not skeleton: return false

	var atlas_name = name + ".atlas"
	var atlas = _find_resource( folder, func(filepath):return filepath.get_file()==atlas_name )
	if not atlas:
		# If there's only one .atlas, use that
		if _count_files( folder, func(filepath):return filepath.ends_with(".atlas") ) == 1:
			atlas = _find_resource( folder, func(filepath):return filepath.ends_with(".atlas") )

	if not atlas: return false

	prints( "Auto-configuring SpineSprite %s with %s and %s" % [name,skeleton.resource_path,atlas.resource_path] )

	definition = SpineSpriteDefinition.new()
	definition.skeleton = skeleton
	definition.atlas = atlas

	return true

func _count_files( folder:String, match_fn:Callable )->int:
	if folder == "res://bin": return 0

	var n = 0

	var dir = DirAccess.open( folder )
	if dir:
		dir.list_dir_begin()

		var filepath = dir.get_next()
		while filepath != "":
			if not filepath.ends_with(".import"):
				if folder == "res://": filepath = folder + filepath
				else:                  filepath = "%s/%s" % [folder,filepath]
				if dir.current_is_dir():
					n += _count_files( filepath, match_fn )
				elif match_fn.call( filepath ):
					n += 1

			filepath = dir.get_next()

		dir.list_dir_end()

	return n

func _find_resource( folder:String, match_fn:Callable ):
	if folder == "res://bin": return null

	var dir = DirAccess.open( folder )
	if dir:
		dir.list_dir_begin()

		var filepath = dir.get_next()
		while filepath != "":
			if not filepath.ends_with(".import"):
				if folder == "res://": filepath = folder + filepath
				else:                  filepath = "%s/%s" % [folder,filepath]
				if dir.current_is_dir():
					var result = _find_resource( filepath, match_fn )
					if result:
						dir.list_dir_end()
						return result
				elif match_fn.call( filepath ):
					dir.list_dir_end()
					return load( filepath )

			filepath = dir.get_next()

		dir.list_dir_end()

	return null

func _get_property_list():
	var property_usage = PROPERTY_USAGE_NO_EDITOR

	var hint_string = ""
	if animation_names:
		hint_string = ",".join( animation_names )
		property_usage = PROPERTY_USAGE_DEFAULT

	var properties = []
	properties.append({
		"name":  "default_animation",
		"type":  TYPE_STRING,
		"usage": property_usage,
		"hint":  PROPERTY_HINT_ENUM,
		"hint_string": hint_string
	})

	return properties

func _reset():
	if data:
		data.reset()
		if Engine.is_editor_hint() and default_animation != "" and is_ready():
			set_animation( default_animation, true )
	_skin_names = null
	_bone_names = null
	_slot_names = null
	_slots      = null
	_attachments_staged = false
	changed.emit()

func _stage_attachments():
	_attachments_staged = true

	var slots = get_slots()
	_staged_attachments.clear()
	_staged_attachments.resize( slots.size() )
	for attachment in _attachments:
		if attachment.is_ready():
			var slot_pointer = attachment._slot_pointer
			var i = slots.find( slot_pointer )  # slots are in drawing order
			if i >= 0:
				var list = _staged_attachments[i]
				if not list:
					list = []
					_staged_attachments[i] = list
				list.push_back( attachment )
	prints(_staged_attachments)

