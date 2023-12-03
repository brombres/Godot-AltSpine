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
  int vertex_count = 0;
  int texture_index = 0;
  int blend_mode = 0;
	// NORMAL   = 0
	// ADDITIVE = 1
	// MULTIPLY = 2
	// SCREEN   = 3

  for (size_t slot_index=0, n=skeleton->getSlots().size(); slot_index < n; ++slot_index)
  {
    spine::Slot* slot = skeleton->getDrawOrder()[slot_index];

    spine::Attachment* attachment = slot->getAttachment();
    if (!attachment) continue;

    int new_blend_mode;
    switch (slot->getData().getBlendMode())
    {
      case spine::BlendMode_Normal:   new_blend_mode = 0; break;
      case spine::BlendMode_Additive: new_blend_mode = 1; break;
      case spine::BlendMode_Multiply: new_blend_mode = 2; break;
      case spine::BlendMode_Screen:   new_blend_mode = 3; break;
      default:                        new_blend_mode = 0;
    }

    if (new_blend_mode != blend_mode && vertex_count)
    {
      on_draw_callback.call( "call", texture_index, blend_mode );
      vertex_count = 0;
    }
    blend_mode = new_blend_mode;

    spine::Color skeletonColor = skeleton->getColor();
    spine::Color slotColor = slot->getColor();
    godot::Color tint( skeletonColor.r*slotColor.r, skeletonColor.g*slotColor.g, skeletonColor.b*slotColor.b, skeletonColor.a*slotColor.a );

    if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
    {
      spine::RegionAttachment* region = (spine::RegionAttachment*)attachment;

      vertex_data.setSize( 8, 0.0f );  // 8 floats to hold 4 (x,y) vertices
      region->computeWorldVertices( *slot, vertex_data, 0 );

			int new_texture_index = (int)(intptr_t)(((spine::AtlasRegion*)region->getRegion())->page->texture);
      if (new_texture_index != texture_index && vertex_count)
      {
        on_draw_callback.call( "call", texture_index, blend_mode );
        vertex_count = 0;
      }
      texture_index = new_texture_index;

      spine::Color attachment_color = region->getColor();
			tint.r *= attachment_color.r;
			tint.g *= attachment_color.g;
			tint.b *= attachment_color.b;
			tint.a *= attachment_color.a;

      // Add vertex, UV, and color information to the mesh builder.
      float* uvs = region->getUVs().buffer();
      float* v_buffer = vertex_data.buffer();
      for (size_t i=0; i<8; i+=2)
      {
        mesh_builder->set_color( tint );
        mesh_builder->set_uv( Vector2(uvs[i],uvs[i+1]) );
        mesh_builder->add_vertex( Vector3(v_buffer[i],-v_buffer[i+1],0) );
      }

      // Add triangle vertex indices
      mesh_builder->add_index( vertex_count+0 );
      mesh_builder->add_index( vertex_count+1 );
      mesh_builder->add_index( vertex_count+2 );
      mesh_builder->add_index( vertex_count+2 );
      mesh_builder->add_index( vertex_count+3 );
      mesh_builder->add_index( vertex_count+0 );

      vertex_count += 4;
    }
    else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
    {
      spine::MeshAttachment* mesh = (spine::MeshAttachment*)attachment;

      vertex_data.setSize( mesh->getWorldVerticesLength(), 0.0f );
      mesh->computeWorldVertices( *slot, vertex_data );

			int new_texture_index = (int)(intptr_t)(((spine::AtlasRegion*) mesh->getRegion())->page->texture);
      if (new_texture_index != texture_index && vertex_count)
      {
        // Render the current mesh_builder contents using the old texture before starting on the new indices.
        on_draw_callback.call( "call", texture_index, blend_mode );
        vertex_count = 0;
      }
      texture_index = new_texture_index;

      spine::Color attachment_color = mesh->getColor();
			tint.r *= attachment_color.r;
			tint.g *= attachment_color.g;
			tint.b *= attachment_color.b;
			tint.a *= attachment_color.a;

      // Add vertex, UV, and color information to the mesh builder.
      float* uvs = mesh->getUVs().buffer();
      float* v_buffer = vertex_data.buffer();
      size_t v_count  = vertex_data.size();
      for (size_t i=0; i<v_count; i+=2)
      {
        mesh_builder->set_color( tint );
        mesh_builder->set_uv( Vector2(uvs[i],uvs[i+1]) );
        mesh_builder->add_vertex( Vector3(v_buffer[i],-v_buffer[i+1],0) );
      }

      // Add triangle vertex indices
      spine::Vector<unsigned short> indices = mesh->getTriangles();
      size_t index_count = indices.size();
      for (size_t i=0; i<index_count; ++i)
      {
        mesh_builder->add_index( vertex_count+indices[i] );
      }

      vertex_count += (int)(vertex_data.size()/2);
    }
  }

  if (vertex_count)
  {
    on_draw_callback.call( "call", texture_index, blend_mode );
    vertex_count = 0;
  }
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
    animation_state->addAnimation(0, "idle_shield_1", true, 0);
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
