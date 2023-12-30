@tool
extends Node2D

var mesh:ArrayMesh
var texture:Texture2D

func _draw():
	if mesh: draw_mesh( mesh, texture )

func update_mesh( mesh_builder:SurfaceTool, material:Material, texture:Texture2D ):
	if mesh:
		mesh.clear_surfaces()
		mesh_builder.commit( mesh )
	else:
		mesh = mesh_builder.commit()

	self.material = material
	self.texture = texture
	queue_redraw()

