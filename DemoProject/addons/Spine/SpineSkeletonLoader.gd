#@tool
#class_name SpineSkeletonLoader
#extends ResourceFormatLoader
#
#func _exists(path)->bool:
#	prints(1,path)
#	return FileAccess.file_exists(path)
#
#func _get_recognized_extensions()->PackedStringArray:
#	prints(2)
#	return PackedStringArray( ["skel","spine-json"] )
#	#return PackedStringArray( ["tres"] )
#
#func _get_resource_script_class ( path:String )->String:
#	prints(3,path)
#	return "SpineSkeletonResource"
#
#func _get_resource_type ( path:String )->String:
#	prints(4,path)
#	return "Resource"
#
#func _handles_type ( type:StringName )->bool:
#	prints(type,type=="Resource")
#	return type == "Resource"
#
#func _load( path:String, original_path:String, use_sub_threads:bool, cache_mode:int )->Variant:
#	prints(5,path,original_path)
#	return ResourceLoader.load( path, "", cache_mode )
