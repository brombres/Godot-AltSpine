@tool
class_name SpineEditorPlugin extends EditorPlugin

var atlas_importer:EditorImportPlugin
var skeleton_importer:EditorImportPlugin

func _enter_tree():
	add_custom_type( "SpineAnimationEvent", "RefCounted", preload("SpineAnimationEvent.gd"), null )
	add_custom_type( "SpineSkin", "RefCounted", preload("SpineSkin.gd"), null )

	add_custom_type( "SpineAtlasResource", "Resource", preload("SpineAtlasResource.gd"), null )
	add_custom_type( "SpineSkeletonResource", "Resource", preload("SpineSkeletonResource.gd"), null )

	atlas_importer = preload( "SpineAtlasImporter.gd" ).new()
	add_import_plugin( atlas_importer )

	skeleton_importer = preload( "SpineSkeletonImporter.gd" ).new()
	add_import_plugin( skeleton_importer )

	add_custom_type( "SpineSpriteDefinition", "Resource",  preload("SpineSpriteDefinition.gd"), null )
	add_custom_type( "SpineSprite",           "Node2D",    preload("SpineSprite.gd"),         load("addons/Spine/Icons/SpineSprite.svg") )
	add_custom_type( "SpineSpriteBone",       "Node2D",    preload("SpineSpriteBone.gd"),     load("addons/Spine/Icons/SpineSprite.svg") )
	add_custom_type( "SpinePointAttachment", "RefCounted", preload("SpinePointAttachment.gd"), null )

func _exit_tree():
	remove_custom_type( "SpinePointAttachment" )
	remove_custom_type( "SpineSpriteBone" )
	remove_custom_type( "SpineSprite" )
	remove_custom_type( "SpineSpriteDefinition" )

	remove_import_plugin( atlas_importer )
	atlas_importer = null

	remove_import_plugin( skeleton_importer )
	skeleton_importer = null

	remove_custom_type( "SpineSkeletonResource" )
	remove_custom_type( "SpineAtlasResource" )

	remove_custom_type( "SpineSkin" )
	remove_custom_type( "SpineAnimationEvent" )
