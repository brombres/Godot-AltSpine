@tool
class_name SpineSpriteFragment
extends Node2D

var mesh:ArrayMesh
var current_texture:Texture2D

func _draw():
	if mesh: draw_mesh( mesh, current_texture )

func update_mesh( mesh_builder:SurfaceTool, blend_mode_material:Material, texture:Texture2D ):
	if mesh:
		mesh.clear_surfaces()
		mesh_builder.commit( mesh )
	else:
		mesh = mesh_builder.commit()

	material = blend_mode_material
	current_texture = texture
	queue_redraw()

