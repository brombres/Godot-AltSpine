extends Node2D


# Called when the node enters the scene tree for the first time.
func _ready():
	$Spineboy.set_animation( "walk", true )
