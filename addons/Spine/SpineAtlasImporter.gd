@tool
extends EditorImportPlugin

func _get_import_options( path:String, preset_index:int )->Array[Dictionary]:
	return []

func _get_import_order()->int:
	return 1  # Let images be imported first

func _get_importer_name()->String:
	return "Spine.atlas_importer"

func _get_preset_count()->int:
	return 1

func _get_preset_name( preset_index:int )->String:
	return "Default"

func _get_priority()->float:
	return 1.0

func _get_recognized_extensions()->PackedStringArray:
	return PackedStringArray( ["atlas"] )

func _get_resource_type()->String:
	return "Resource"

func _get_save_extension()->String:
	return "res"

func _get_visible_name()->String:
	return "Spine Texture Atlas"

func _import( source_filepath:String, save_path:String, options:Dictionary, platform_variants:Array[String], gen_files:Array[String] ):
	if FileAccess.file_exists(source_filepath):
		var resource = preload( "SpineAtlasResource.gd" ).new()
		resource.bytes = FileAccess.get_file_as_bytes( source_filepath )

		var basename = source_filepath.get_basename()
		var png_filepath = basename + ".png"
		if FileAccess.file_exists(png_filepath):
			resource.add_color_map( load(png_filepath) )
			for i in range(2,100):
				png_filepath = "%s_%d.png" % [basename,i]
				if FileAccess.file_exists(png_filepath):
					resource.add_color_map( load(png_filepath) )
				else:
					break

			# Optional normal maps
			png_filepath = "n_%s.png" % basename
			if FileAccess.file_exists(png_filepath):
				resource.add_normal_map( load(png_filepath) )
				for i in range(2,100):
					png_filepath = "n_%s_%d.png" % [basename,i]
					if FileAccess.file_exists(png_filepath):
						resource.add_normal_map( load(png_filepath) )
					else:
						break

			# Optional specular maps
			png_filepath = "s_%s.png" % basename
			if FileAccess.file_exists(png_filepath):
				resource.add_specular_map( load(png_filepath) )
				for i in range(2,100):
					png_filepath = "s_%s_%d.png" % [basename,i]
					if FileAccess.file_exists(png_filepath):
						resource.add_specular_map( load(png_filepath) )
					else:
						break

		ResourceSaver.save( resource, save_path+".res" )
		return OK

	else:
		return Error.ERR_FILE_NOT_FOUND


