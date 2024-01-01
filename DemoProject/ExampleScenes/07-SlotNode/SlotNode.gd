@tool
extends Node2D

func _ready():
	prints( $Spineboy.get_slot_names() )
