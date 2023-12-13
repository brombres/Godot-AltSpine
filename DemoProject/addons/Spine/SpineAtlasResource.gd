@tool
class_name SpineAtlasResource
extends Resource

@export var bytes:PackedByteArray :
	set(value):
		if bytes != value:
			bytes = value
			configure()

@export var color_maps:Array[Texture2D] :
	set(value):
		if color_maps != value:
			color_maps = value
			configure()

@export var normal_maps:Array[Texture2D] :
	set(value):
		if normal_maps != value:
			normal_maps = value
			configure()

@export var specular_maps:Array[Texture2D] :
	set(value):
		if specular_maps != value:
			specular_maps = value
			configure()

var textures:Array[Texture2D]
var using_canvas_texture := false

var _atlas_data_pointer:int # Internal use only.

var data:SpineAtlasResourceData

func add_color_map( texture:Texture2D ):
	color_maps.push_back( texture )
	configure()

func add_normal_map( texture:Texture2D ):
	normal_maps.push_back( texture )
	configure()

func add_specular_map( texture:Texture2D ):
	specular_maps.push_back( texture )
	configure()

func configure():
	var use_canvas_texture = (normal_maps.size() > 0 or specular_maps.size() > 0)
	if use_canvas_texture != using_canvas_texture: textures = []

	using_canvas_texture = use_canvas_texture

	var limit = max( max(color_maps.size(), normal_maps.size()), specular_maps.size() )

	if use_canvas_texture:
		for i in range(limit):
			if i == textures.size(): textures.push_back( CanvasTexture.new() )
			var texture = textures[i]
			if i < color_maps.size():    texture.diffuse_texture = color_maps[i]
			else:                        texture.diffuse_texture = null
			if i < normal_maps.size():   texture.normal_texture = normal_maps[i]
			else:                        texture.normal_texture = null
			if i < specular_maps.size(): texture.specular_texture = specular_maps[i]
			else:                        texture.specular_texture = null
	else:
		for i in range(color_maps.size()):
			if i == textures.size(): textures.push_back( color_maps[i] )
			else:                    textures[i] = color_maps[i]

	while textures.size() > limit: textures.pop_back()

	if data: data.configure( self )

	emit_changed()

func is_ready():
	if not data:
		data = SpineAtlasResourceData.new()
		data.configure( self )
	return data.is_ready()
