@tool
extends Node2D

@onready var spineboy = $Spineboy
@onready var raptor   = $Raptor


# Called when the node enters the scene tree for the first time.
func _ready():
	var spineboy_anim = create_tween()
	spineboy_anim.tween_callback( func():spineboy.set_animation( "walk", true ) )

#func add( tween:Tween, animation_name:String, duration:float ):


