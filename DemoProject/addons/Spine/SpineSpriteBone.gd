@tool
class_name SpineSpriteBone extends Node2D

const SpineSprite = preload( "SpineSprite.gd" )

var bone := "" :
	set(value):
		bone = value
		_bone_pointer = 0

var mode := "Follow" :
	set(value):
		mode = value
		_drives = (mode == "Drive")

var _bone_pointer:int
var _drives := false

func is_ready()->bool:
	if _bone_pointer: return true
	var parent = get_parent()
	if parent is SpineSprite:
		_bone_pointer = parent.find_bone( bone )
		#if _drives and _bone_pointer: _notification( NOTIFICATION_TRANSFORM_CHANGED )
	return _bone_pointer

func _enter_tree():
	var parent = get_parent()
	if parent is SpineSprite:
		if not parent.changed.is_connected( _on_sprite_changed ): parent.changed.connect( _on_sprite_changed )
		if not parent.updated.is_connected( _on_sprite_updated ): parent.updated.connect( _on_sprite_updated )
	_bone_pointer = 0
	#set_notify_transform( true )

func _exit_tree():
	var parent = get_parent()
	if parent is SpineSprite:
		if parent.changed.is_connected( _on_sprite_changed ): parent.changed.disconnect( _on_sprite_changed )
		if parent.updated.is_connected( _on_sprite_changed ): parent.updated.disconnect( _on_sprite_updated )
	_bone_pointer = 0
	#set_notify_transform( false )

func _get_bone_names()->Array[String]:
	var parent = get_parent()
	if parent is SpineSprite:
		return parent.get_bone_names()
	return []

func _get_property_list():
	var properties = []

	var hint_string = ",".join( _get_bone_names() )
	if hint_string != "":
		properties.append({
			"name":  "bone",
			"type":  TYPE_STRING,
			"usage": PROPERTY_USAGE_DEFAULT,
			"hint":  PROPERTY_HINT_ENUM,
			"hint_string": hint_string
		})

	properties.append({
		"name":  "mode",
		"type":  TYPE_STRING_NAME,
		"usage": PROPERTY_USAGE_DEFAULT,
		"hint":  PROPERTY_HINT_ENUM,
		"hint_string": "Follow,Drive"
	})

	return properties

#func _notification( what:int ):
#	# If global_position is set, we get a TRANSFORM_CHANGED notification in the editor
#	# but not at runtime...?!
#	if _drives and what == NOTIFICATION_TRANSFORM_CHANGED and is_ready():
#		var parent = get_parent()
#		parent.set_bone_position( _bone_pointer, position )
#		parent.set_bone_rotation( _bone_pointer, rotation )

func _on_sprite_changed():
	_bone_pointer = 0

func _on_sprite_updated():
	if is_ready():
		var parent = get_parent()
		if _drives:
			parent.set_bone_position( _bone_pointer, position )
			parent.set_bone_rotation( _bone_pointer, rotation )
		else:
			position = parent.get_bone_position(_bone_pointer)
			rotation = parent.get_bone_rotation(_bone_pointer)

