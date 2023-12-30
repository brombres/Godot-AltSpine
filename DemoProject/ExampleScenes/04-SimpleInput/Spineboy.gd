@tool
extends SpineSprite


# Called when the node enters the scene tree for the first time.
func _ready():
	super()

func _process(_delta):
	super(_delta)

	if Input.is_action_just_pressed("ui_left"):
		set_animation("run", true, 0)
		scale.x = -(abs(scale.x))

	if Input.is_action_just_released("ui_left"):
		set_animation("idle", true, 0)

	if (Input.is_action_just_pressed("ui_right")):
		set_animation("run", true, 0)
		scale.x = abs(scale.x)

	if Input.is_action_just_released("ui_right"):
		set_animation("idle", true, 0)
