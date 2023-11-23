@tool
extends EditorPlugin

var atlas_importer:EditorImportPlugin
var skeleton_importer:EditorImportPlugin

func _enter_tree():
	add_custom_type( "SpineAtlasResource", "Resource", preload("SpineAtlasResource.gd"), null )
	add_custom_type( "SpineSkeletonResource", "Resource", preload("SpineSkeletonResource.gd"), null )
	#add_custom_type( "SpineSkeletonLoader", "ResourceFormatLoader", preload("SpineSkeletonLoader.gd"), null )

	atlas_importer = preload( "SpineAtlasImporter.gd" ).new()
	add_import_plugin( atlas_importer )
	skeleton_importer = preload( "SpineSkeletonImporter.gd" ).new()
	add_import_plugin( skeleton_importer )

	add_custom_type( "SpineAtlas", "Resource", preload("SpineAtlas.gd"), null )
	add_custom_type( "SpineSpriteDefinition", "Resource", preload("SpineSpriteDefinition.gd"), null )
	add_custom_type( "SpineSprite", "Resource", preload("SpineSprite.gd"), null )

func _exit_tree():
	remove_custom_type( "SpineSprite" )
	remove_custom_type( "SpineSpriteDefinition" )
	remove_custom_type( "SpineAtlas" )

	remove_import_plugin( atlas_importer )
	atlas_importer = null

	remove_import_plugin( skeleton_importer )
	skeleton_importer = null

	#remove_custom_type( "SpineSkeletonLoader" )
	remove_custom_type( "SpineSkeletonResource" )
	remove_custom_type( "SpineAtlasResource" )
