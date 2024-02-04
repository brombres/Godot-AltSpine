extends Node2D

@onready var spineboy = $Spineboy
@onready var center_bone = $Spineboy/HoverboardCenterBone
@onready var center_ray = $Spineboy/HoverboardCenterBone/CenterRay
@onready var target_bone = $Spineboy/HoverboardTargetBone
@onready var target_ray = $Spineboy/HoverboardTargetBone/TargetRay
@onready var hip_bone = $Spineboy/HipBone

var center_hip_distance = 0

func _ready():
	spineboy.set_animation( "hoverboard", true )
	spineboy.data.update( 0 )
	center_hip_distance = hip_bone.global_position.y - center_bone.global_position.y
	prints(center_hip_distance)

func _physics_process(delta):
	if target_ray.is_colliding():
		prints( target_ray.get_collision_point().y - 30 )
		target_bone.global_position.y = target_ray.get_collision_point().y - 30
		prints( target_bone.global_position )

	if center_ray.is_colliding():
		center_bone.global_position.y = center_ray.get_collision_point().y - 30

	if abs(hip_bone.global_position.y - center_bone.global_position.y) - abs(center_hip_distance) < 20:
		hip_bone.global_position.y = center_bone.global_position.y + center_hip_distance

	spineboy.global_position.x += delta * 150;
