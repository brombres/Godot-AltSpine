@tool
extends EditorPlugin

var atlas_importer:EditorImportPlugin
var skeleton_importer:EditorImportPlugin

func _enter_tree():
	add_custom_type( "SpineAtlasResource", "Resource", preload("SpineAtlasResource.gd"), null )
	add_custom_type( "SpineSkeletonResource", "Resource", preload("SpineSkeletonResource.gd"), null )

	atlas_importer = preload( "SpineAtlasImporter.gd" ).new()

	add_import_plugin( atlas_importer )
	skeleton_importer = preload( "SpineSkeletonImporter.gd" ).new()
	add_import_plugin( skeleton_importer )

	add_custom_type( "SpineSpriteDefinition", "Resource", preload("SpineSpriteDefinition.gd"), null )
	add_custom_type( "SpineSprite",         "Node2D", preload("SpineSprite.gd"),         null )
	add_custom_type( "SpineSpriteFragment", "Node2D", preload("SpineSpriteFragment.gd"), null )

func _exit_tree():
	remove_custom_type( "SpineSpriteFragment" )
	remove_custom_type( "SpineSprite" )
	remove_custom_type( "SpineSpriteDefinition" )

	remove_import_plugin( atlas_importer )
	atlas_importer = null

	remove_import_plugin( skeleton_importer )
	skeleton_importer = null

	remove_custom_type( "SpineSkeletonResource" )
	remove_custom_type( "SpineAtlasResource" )
