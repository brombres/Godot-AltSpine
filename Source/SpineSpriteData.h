// REFERENCE
// https://docs.godotengine.org/en/stable/contributing/development/core_and_modules/object_class.html

#ifndef SPINESPRITEDATA_H
#define SPINESPRITEDATA_H

#ifdef WIN32
	#include <windows.h>
#endif

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/surface_tool.hpp>

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <spine/spine.h>
#include <spine/AnimationState.h>
#include <spine/AnimationStateData.h>
#include <spine/Extension.h>
#include <spine/Skeleton.h>
#include <spine/SkeletonData.h>

using namespace godot;

class SpineSpriteData : public RefCounted
{
	GDCLASS( SpineSpriteData, RefCounted );

	protected:
		static void _bind_methods();

	public:
		// PROPERTIES
    Node*                  spine_sprite;
    spine::Skeleton*       skeleton = nullptr;
    spine::AnimationState* animation_state = nullptr;
    spine::Vector<float>   vertex_data;

		// CONSTRUCTOR METHODS
		SpineSpriteData();
		~SpineSpriteData();

		// GENERAL METHODS
    void configure( Node* spine_sprite );
    void draw( SurfaceTool* mesh_builder, Variant on_draw_callback );
    bool is_ready();
    void reset();
    void update( double dt );
};

#endif // SPINESPRITEDATA_H
