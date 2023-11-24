@tool
class_name SpineSprite
extends Node2D

const SpineSpriteDefinition = preload("SpineSpriteDefinition.gd")
@export var definition:SpineSpriteDefinition


# Called when the node enters the scene tree for the first time.
func _ready():
	_configure()

func _enter_tree():
	renamed.connect( _configure )

func _exit_tree():
	if renamed.is_connected( _configure ):
		renamed.disconnect( _configure )

func _configure():
	if definition or not Engine.is_editor_hint(): return

	var viewport = get_viewport()
	if not viewport: return

	var child_count = viewport.get_child_count()
	var scene = viewport.get_child( child_count - 1 )

	if not _configure_resources( scene.scene_file_path.get_base_dir() ):  # Try scene folder first
		_configure_resources( "res://" )  # Then root resource folder

func _configure_resources( folder:String )->bool:
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

