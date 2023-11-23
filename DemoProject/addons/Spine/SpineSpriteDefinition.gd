@tool
class_name SpineSpriteDefinition
extends Resource

const SpineSkeletonResource = preload("SpineSkeletonResource.gd")
const SpineAtlas = preload("SpineAtlas.gd")

@export var skeleton:SpineSkeletonResource
@export var atlas:SpineAtlas
