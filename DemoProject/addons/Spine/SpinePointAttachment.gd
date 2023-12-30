class_name SpinePointAttachment extends RefCounted

const SpineSprite = preload( "SpineSprite.gd" )

var spine_sprite:SpineSprite
var point_attachment_id:int
var slot_name:String

var local_position:Vector2 :
	set(value):
		if spine_sprite.is_ready():
			spine_sprite.data.set_point_attachment_local_position( point_attachment_id, value )
	get:
		if spine_sprite.is_ready():
			return spine_sprite.data.get_point_attachment_local_position( point_attachment_id )
		else:
			return Vector2(0,0)

var local_rotation:float :
	set(value):
		if spine_sprite.is_ready():
			spine_sprite.data.set_point_attachment_local_rotation( point_attachment_id, value )
	get:
		if spine_sprite.is_ready():
			return spine_sprite.data.get_point_attachment_local_rotation( point_attachment_id )
		else:
			return 0

func _init( spine_sprite:SpineSprite, point_attachment_id:int, slot_name:String ):
	self.spine_sprite = spine_sprite
	self.point_attachment_id = point_attachment_id
	self.slot_name = slot_name

func get_position()->Vector2:
	if spine_sprite.is_ready():
		return spine_sprite.data.get_point_attachment_position( point_attachment_id, slot_name )
	else:
		return Vector2(0,0)

func set_position( position:Vector2 ):
	if spine_sprite.is_ready():
		spine_sprite.data.set_point_attachment_position( point_attachment_id, position )

func get_rotation()->float:
	if spine_sprite.is_ready():
		return spine_sprite.data.get_point_attachment_rotation( point_attachment_id, slot_name )
	else:
		return 0

func set_rotation( rotation:float ):
	if spine_sprite.is_ready():
		spine_sprite.data.set_point_attachment_position( point_attachment_id, rotation )
