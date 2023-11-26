// REFERENCE
// https://docs.godotengine.org/en/stable/contributing/development/core_and_modules/object_class.html

#ifndef SPINESPRITEDEFINITIONDATA_H
#define SPINESPRITEDEFINITIONDATA_H

#ifdef WIN32
	#include <windows.h>
#endif

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <spine/spine.h>
#include <spine/Extension.h>
#include <spine/SkeletonBinary.h>

using namespace godot;

class SpineSpriteDefinitionData : public RefCounted
{
	GDCLASS( SpineSpriteDefinitionData, RefCounted );

	protected:
		static void _bind_methods();

	public:
		// PROPERTIES
    Variant spine_sprite_definition;
    spine::SkeletonData* skeleton_data = nullptr;

		// CONSTRUCTOR METHODS
		SpineSpriteDefinitionData();
		~SpineSpriteDefinitionData();

		// PROPERTY ACCESS METHODS

		// GENERAL METHODS
    void configure( Variant spine_sprite_definition );
    bool prepare_to_draw();
};

#endif // SPINESPRITEDEFINITIONDATA_H
