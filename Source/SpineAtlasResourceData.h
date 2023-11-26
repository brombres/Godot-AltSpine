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

using namespace godot;

class SpineAtlasResourceData : public RefCounted
{
	GDCLASS( SpineAtlasResourceData, RefCounted );

	protected:
		static void _bind_methods();

	public:
		// PROPERTIES

		// CONSTRUCTOR/DESTRUCTOR
		SpineAtlasResourceData();
		~SpineAtlasResourceData();

		// METHODS

};

#endif // SPINEATLASRESOURCEDATA_H
