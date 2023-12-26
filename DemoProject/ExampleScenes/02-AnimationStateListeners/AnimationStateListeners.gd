@tool
extends Node2D

@onready var spineboy = $Spineboy
@onready var footstep = $Footstep

func _ready():
	if not Engine.is_editor_hint():
		spineboy.animation_event.connect( _on_animation_event )
		_enqueue_animation()

func _enqueue_animation():
	spineboy.set_animation( "jump", false, 0 )   # Jump, non-looping, track 0
	spineboy.add_animation( "walk", 1.25, true, 0 ) # Walk after 1 second, looping
	spineboy.add_animation( "run", 2, true, 0 )  # Run after 2 seconds, looping
	spineboy.add_empty_animation( 2, 0.5, 0 )    # After 2 more seconds, blend in a return to default pose over 0.5 seconds

func _on_animation_event( e:SpineAnimationEvent ):
	if e.type == SpineAnimationEvent.Type.USER_EVENT and e.user_event_name == "footstep":
		footstep.play()
	elif e.type == SpineAnimationEvent.Type.END and e.animation_name == "<empty>":
		_enqueue_animation()

	print( e )
