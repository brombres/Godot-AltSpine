@tool
class_name SpineSkeletonResource extends Resource

@export var bytes:PackedByteArray :
	set(value):
		if bytes != value:
			bytes = value
			emit_changed()
