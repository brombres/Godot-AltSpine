@tool
class_name SpineSpriteDefinition
extends Resource

const SpineSkeletonResource = preload("SpineSkeletonResource.gd")
const SpineAtlasResource = preload("SpineAtlasResource.gd")

@export var skeleton:SpineSkeletonResource :
	set(value):
		if skeleton != value:
			skeleton = value
			_configure()
			emit_changed()

@export var atlas:SpineAtlasResource :
	set(value):
		if atlas != value:
			atlas = value
			_configure()
			emit_changed()

var data:SpineSpriteDefinitionData
var _data_pointer:int

func is_ready()->bool:
	if not skeleton or not atlas or not data: return false

	if not atlas.is_ready():     return false
	if not data.is_ready(atlas): return false

	return true

func _notification( what ):
	if what == NOTIFICATION_PREDELETE:
		if data: data.reset()
		if skeleton and skeleton.changed.is_connected(_configure):
			skeleton.changed.disconnect( _configure )
		if atlas and atlas.changed.is_connected(_configure):
			atlas.changed.disconnect( _configure )

func _configure():
	if data: data.reset()
	if skeleton and not skeleton.changed.is_connected( _configure ):
		skeleton.changed.connect( _configure )
	if atlas and not atlas.changed.is_connected( _configure ):
		atlas.changed.connect( _configure )
	if skeleton and atlas:
		if not data: data = SpineSpriteDefinitionData.new()
		data.configure( self, atlas )

