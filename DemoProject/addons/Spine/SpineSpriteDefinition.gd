@tool
class_name SpineSpriteDefinition
extends Resource

const SpineSkeletonResource = preload("SpineSkeletonResource.gd")
const SpineAtlasResource = preload("SpineAtlasResource.gd")

@export var skeleton:SpineSkeletonResource
@export var atlas:SpineAtlasResource
