class_name SpineSkin extends RefCounted

var elements:Array[Variant] = []

func add( other_skin:Variant ):
	elements.push_back( other_skin )

func clear():
	elements.clear()
