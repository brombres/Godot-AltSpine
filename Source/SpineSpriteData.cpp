#include "SpineSpriteData.h"
#include "SpineSpriteDefinitionData.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

SpineSpriteData::SpineSpriteData()
{
}

SpineSpriteData::~SpineSpriteData()
{
  if (animation_state)
  {
    delete animation_state;
    animation_state = nullptr;
  }

  if (skeleton)
  {
    delete skeleton;
    skeleton = nullptr;
  }
}

void SpineSpriteData::configure( Node* spine_sprite )
{
  this->spine_sprite = spine_sprite;
}

void SpineSpriteData::draw( SurfaceTool* mesh_builder, Variant on_draw_callback )
{
  //UtilityFunctions::print( "draw()" );
  mesh_builder->set_color( Color(1,1,1) );

  real_t x1 = 0;
  real_t y1 = 0;
  real_t x2 = 200;
  real_t y2 = 200;
  real_t u1 = 0.0;
  real_t u2 = 1.0;
  real_t v1 = 0.0;
  real_t v2 = 1.0;

	mesh_builder->set_uv( Vector2(u1,v1) ); mesh_builder->add_vertex( Vector3(  x1,  y1,0) );
	mesh_builder->set_uv( Vector2(u1,v2) ); mesh_builder->add_vertex( Vector3(  x1,  y2,0) );
	mesh_builder->set_uv( Vector2(u2,v2) ); mesh_builder->add_vertex( Vector3(  x2,  y2,0) );

	mesh_builder->set_uv( Vector2(u1,v1) ); mesh_builder->add_vertex( Vector3(  x1,  y1,0) );
	mesh_builder->set_uv( Vector2(u2,v2) ); mesh_builder->add_vertex( Vector3(  x2,  y2,0) );
	mesh_builder->set_uv( Vector2(u2,v1) ); mesh_builder->add_vertex( Vector3(  x2,  y1,0) );

  on_draw_callback.call( "call", 0 );
}

bool SpineSpriteData::prepare_to_draw()
{
  if ( !skeleton ) return false;
  return true;
}

void SpineSpriteData::update( double dt )
{
  if ( !skeleton )
  {
    Variant v_def = ((Object*)spine_sprite)->get("definition");
    if ( !v_def ) return;

    SpineSpriteDefinitionData* def = (SpineSpriteDefinitionData*)(void*)(intptr_t)(uint64_t)(((Object*)v_def)->get("_data_pointer"));
    if ( !def ) return;
    if ( !def->skeleton_data || !def->animation_state_data ) return;

    skeleton = new spine::Skeleton( def->skeleton_data );
    animation_state = new spine::AnimationState( def->animation_state_data );
  }

  animation_state->update( dt );
  animation_state->apply( *skeleton );
  skeleton->updateWorldTransform();
}

void SpineSpriteData::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("configure","spine_sprite"),	              &SpineSpriteData::configure );
	ClassDB::bind_method( D_METHOD("draw","mesh_builder","on_draw_callback"), &SpineSpriteData::draw );
	ClassDB::bind_method( D_METHOD("prepare_to_draw"),	                      &SpineSpriteData::prepare_to_draw );
	ClassDB::bind_method( D_METHOD("update","dt"),	                          &SpineSpriteData::update );
}
