@tool
class_name SpineSpriteDefinition
extends Resource

const SpineSkeletonResource = preload("SpineSkeletonResource.gd")
const SpineAtlasResource = preload("SpineAtlasResource.gd")

@export var skeleton:SpineSkeletonResource
@export var atlas:SpineAtlasResource

var data:SpineSpriteDefinitionData

func prepare_to_draw()->bool:
	if not skeleton or not atlas: return false

	if not atlas.prepare_to_draw(): return false

	if not data:
		data = SpineSpriteDefinitionData.new()
		data.configure( self )

	if not data.prepare_to_draw(): return false

	return true
