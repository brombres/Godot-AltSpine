@tool
extends SpineSprite

@export var randomize := false :
	set(value):
		_randomize()

# Called when the node enters the scene tree for the first time.
func _ready():
	super()
	prints( "skin names:", get_skin_names() )
	_randomize()

func _randomize():
	var custom_skin = SpineSkin.new()
	custom_skin.add( "skin-base" )
	for category in ["accessories","clothes","eyelids","eyes","hair","nose","legs"]:
		_add_random_skin( custom_skin, category )
	set_skin( custom_skin )
	prints()

func _add_random_skin( custom_skin:SpineSkin, category ):
	category += '/'
	var choices = []
	for skin_name in get_skin_names():
		if skin_name.begins_with(category): choices.push_back( skin_name )

	if choices.size():
		var choice = choices[ randi() % choices.size() ]
		custom_skin.add( choice )

