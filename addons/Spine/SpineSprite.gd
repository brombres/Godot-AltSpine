@tool
class_name SpineSprite
extends Node2D

enum BlendMode
{
	NORMAL   = 0,
	ADDITIVE = 1,
	MULTIPLY = 2,
	SCREEN   = 3
}

const SpineSpriteDefinition = preload("SpineSpriteDefinition.gd")
const screen_shader = preload("SpineScreenBlendShader.gdshader")

@export var definition:SpineSpriteDefinition :
	set(value):
		if definition != value:
			if definition and definition.changed.is_connected(_on_definition_changed):
				definition.changed.disconnect( _on_definition_changed )

			definition = value

			if definition and not definition.changed.is_connected(_on_definition_changed):
				definition.changed.connect( _on_definition_changed )

			_on_definition_changed()

@export var premultiplied_alpha := false :
	set(value):
		if premultiplied_alpha != value:
			premultiplied_alpha = value
		_prepare_materials()

@export var normal_material:Material
@export var additive_material:Material
@export var multiply_material:Material
@export var screen_material:Material

@export var preview_animation := true

@export var preview_frame := 0

var default_animation := "" :
	set(value):
		if default_animation != value:
			default_animation = value
			if Engine.is_editor_hint() and preview_animation and value != "":
				set_animation( value, true )

var data:SpineSpriteData
var _mesh_builder:SurfaceTool = SurfaceTool.new()
var _fragments:Array[SpineSpriteFragment] = []
var _used_fragment_count := 0
var _active_blend_mode := BlendMode.NORMAL

var _animation_names = null
var _materials:Array[Material] = []
var _is_ready := false

func _ready():
	material = ShaderMaterial.new()
	material.shader = screen_shader

	_configure_resources()
	data = SpineSpriteData.new()
	data.configure( self )

	_prepare_materials()

	# Remove any old fragments (I don't *think* this could happen but just in case)
	for child in get_children(true):
		if child is SpineSpriteFragment:
			child.queue_free()

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

func is_ready()->bool:
	## Returns true if this SpineSprite is ready to update or draw.
	if _is_ready: return true
	if not definition or not definition.is_ready(): return false
	if not data or not data.is_ready(): return false
	if not _animation_names:
		_animation_names = definition.get_animation_names()
		_animation_names.push_front( "(none)" )
		notify_property_list_changed()

		if default_animation == "" and _animation_names.count > 1:
			default_animation = _animation_names[1];
		elif Engine.is_editor_hint() and preview_animation:
			set_animation( default_animation, true )

	if not Engine.is_editor_hint():
		# Don't cache the result in the Editor because the linked resources can be altered.
		_is_ready = true

	return true

func add_animation( name:String, delay:float=0.0, looping:bool=false, track_index:int=0 ):
	if is_ready():
		if name == "(none)":           set_empty_animations()
		elif name in _animation_names: data.add_animation( track_index, name, looping, delay )

func add_empty_animation( delay:float=0.0, mix_duration:float=0.0, track_index:int=0 ):
	if is_ready():
		data.add_empty_animation( track_index, mix_duration, delay )

func clear_tracks( tracks=null ):
	if is_ready():
		if tracks:
			for track_index in tracks:
				data.clear_track( track_index )
		else:
			data.clear_tracks()

func set_animation( name:String, looping:bool=false, track_index:int=0 ):
	if is_ready():
		if name == "(none)":           data.set_empty_animation( track_index, 0.5 )
		elif name in _animation_names: data.set_animation( track_index, name, looping )

func set_empty_animations( mix_duration:float=0.0, tracks=null ):
	if is_ready():
		if tracks:
			for track_index in tracks:
				data.set_empty_animation( track_index, mix_duration )
		else:
			data.set_empty_animations( mix_duration )

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

func _on_definition_changed():
	if data: data.reset()

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
	if _animation_names:
		hint_string = ",".join( _animation_names )
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
