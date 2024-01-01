@tool
class_name SpineSpriteAttachment extends Node2D

#const SpineSprite = preload( "SpineSprite.gd" )

var slot := "" :
	set(value):
		slot = value
		_slot_pointer = 0

var _slot_pointer:int
var _drives := false

func is_ready()->bool:
	if _slot_pointer: return true
	var parent = get_parent()
	if parent is SpineSprite:
		_slot_pointer = parent.find_slot( slot )
	return _slot_pointer

func _enter_tree():
	var parent = get_parent()
	if parent is SpineSprite:
		if not parent.changed.is_connected( _on_sprite_changed ): parent.changed.connect( _on_sprite_changed )
		if not parent.updated.is_connected( _on_sprite_updated ): parent.updated.connect( _on_sprite_updated )
	_slot_pointer = 0
	set_notify_transform( true )

func _exit_tree():
	var parent = get_parent()
	if parent is SpineSprite:
		if parent.changed.is_connected( _on_sprite_changed ): parent.changed.disconnect( _on_sprite_changed )
		if parent.updated.is_connected( _on_sprite_changed ): parent.updated.disconnect( _on_sprite_updated )
	_slot_pointer = 0
	set_notify_transform( false )

func _get_slot_names()->Array[String]:
	var parent = get_parent()
	if parent is SpineSprite:
		return parent.get_slot_names()
	return []

func _get_property_list():
	var properties = []

	var hint_string = ",".join( _get_slot_names() )
	if hint_string != "":
		properties.append({
			"name":  "slot",
			"type":  TYPE_STRING,
			"usage": PROPERTY_USAGE_DEFAULT,
			"hint":  PROPERTY_HINT_ENUM,
			"hint_string": hint_string
		})

	return properties

func _on_sprite_changed():
	_slot_pointer = 0

func _on_sprite_updated():
	pass

func _process( _dt ):
	if is_ready():
		var parent = get_parent()
		position = parent.get_slot_position(_slot_pointer)
		rotation = parent.get_slot_rotation(_slot_pointer)

