@tool
class_name SpineAtlasResource
extends Resource

@export var bytes:PackedByteArray :
	set(value):
		if bytes != value:
			bytes = value
			if data: data.configure( self )
			emit_changed()

@export var textures:Array[Texture2D] :
	set(value):
		if textures != value:
			textures = value
			if data: data.configure( self )
			emit_changed()

@export var normal_maps:Array[Texture2D] :
	set(value):
		if normal_maps != value:
			normal_maps = value
			if data: data.configure( self )
			emit_changed()

var _atlas_data_pointer:int # Internal use only.

var data:SpineAtlasResourceData

func is_ready():
	if not data:
		data = SpineAtlasResourceData.new()
		data.configure( self )
	return data.is_ready()
