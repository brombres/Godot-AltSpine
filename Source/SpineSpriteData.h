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

class SpineSpriteData;

//==============================================================================
// SpineSpriteAnimationEventListener
//==============================================================================
class SpineSpriteAnimationEventListener : public spine::AnimationStateListenerObject
{
  public:
    SpineSpriteData* data;

		virtual void callback( spine::AnimationState *state, spine::EventType type, spine::TrackEntry *entry, spine::Event *event );
};


//==============================================================================
// SpineSpriteData
//==============================================================================
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
    SpineSpriteAnimationEventListener listener;

		// CONSTRUCTOR METHODS
		SpineSpriteData();
		~SpineSpriteData();

		// GENERAL METHODS
    int64_t add_animation( int track_index, String name, bool looping, float delay, float time_scale );
    void    add_empty_animation( int track_index, float mix_duration, float delay );
    void    clear_track( int track_index );
    void    clear_tracks();
    void    configure( Node* spine_sprite );
    void    draw( SurfaceTool* mesh_builder, Variant on_draw_callback );
    int64_t get_skin();
    int64_t get_point_attachment( String slot_name, String attachment_name );
    Vector2 get_point_attachment_local_position( int64_t attachment_pointer );
    float   get_point_attachment_local_rotation( int64_t attachment_pointer );
    Vector2 get_point_attachment_position( int64_t attachment_pointer, String slot_name );
    float   get_point_attachment_rotation( int64_t attachment_pointer, String slot_name );
    String  get_skin_name( int64_t skin_pointer );
    Array   get_skins();
    float   get_time_scale();
    bool    is_ready();
    void    reset();
    int64_t set_animation( int track_index, String name, bool looping, float time_scale );
    void    set_attachment( String slot_name, Variant attachment_name );
    void    set_point_attachment_local_position( int64_t attachment_pointer, Vector2 position );
    void    set_point_attachment_local_rotation( int64_t attachment_pointer, float rotation );
    void    set_empty_animation( int track_index, float mix_duration );
    void    set_empty_animations( float mix_duration );
    void    set_skin( Variant name );
    void    set_time_scale( float scale );
    void    update( double dt );
};

#endif // SPINESPRITEDATA_H
