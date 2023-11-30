// REFERENCE
// https://docs.godotengine.org/en/stable/contributing/development/core_and_modules/object_class.html

#ifndef SPINEATLASRESOURCEDATA_H
#define SPINEATLASRESOURCEDATA_H

#ifdef WIN32
	#include <windows.h>
#endif

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <spine/Atlas.h>

using namespace godot;

class SpineAtlasResourceData : public RefCounted
{
	GDCLASS( SpineAtlasResourceData, RefCounted );

	protected:
		static void _bind_methods();

	public:
		// PROPERTIES
    Variant spine_atlas_resource;
    spine::Atlas* atlas = nullptr;
    //SpineSpriteTextureLoader* texture_loader = nullptr;

		// CONSTRUCTOR/DESTRUCTOR
		SpineAtlasResourceData();
		~SpineAtlasResourceData();
    void reset();

		// METHODS
    bool configure( Variant spine_atlas_resource );
    bool prepare_to_draw();

};

#endif // SPINEATLASRESOURCEDATA_H
