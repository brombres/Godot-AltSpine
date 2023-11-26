@tool
class_name SpineAtlasResource
extends Resource

@export var bytes:PackedByteArray
@export var textures:Array[Texture2D]
@export var normal_maps:Array[Texture2D]

var data:SpineAtlasResourceData

func prepare_to_draw():
	if not data:
		data = SpineAtlasResourceData.new()
		data.configure( self )
	return data.prepare_to_draw()
