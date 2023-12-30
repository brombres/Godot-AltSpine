@tool
class_name SpineSpriteBone extends Node2D

#const SpineSprite = preload( "SpineSprite.gd" )

var bone := ""

var mode := "Follow" :
	set(value):
		mode = value
		drives = (mode == "Drive")

var drives := false

func _get_bone_names()->Array[String]:
	var parent = get_parent()
	if parent:
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
