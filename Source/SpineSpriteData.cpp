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
  //mesh_builder->set_color( Color(1,1,1) );

  //real_t x1 = 0;
  //real_t y1 = 0;
  //real_t x2 = 200;
  //real_t y2 = 200;
  //real_t u1 = 0.0;
  //real_t u2 = 1.0;
  //real_t v1 = 0.0;
  //real_t v2 = 1.0;

	////mesh_builder->set_uv( Vector2(u1,v1) ); mesh_builder->add_vertex( Vector3(  x1,  y1,0) );
	////mesh_builder->set_uv( Vector2(u1,v2) ); mesh_builder->add_vertex( Vector3(  x1,  y2,0) );
	////mesh_builder->set_uv( Vector2(u2,v2) ); mesh_builder->add_vertex( Vector3(  x2,  y2,0) );

	////mesh_builder->set_uv( Vector2(u1,v1) ); mesh_builder->add_vertex( Vector3(  x1,  y1,0) );
	////mesh_builder->set_uv( Vector2(u2,v2) ); mesh_builder->add_vertex( Vector3(  x2,  y2,0) );
	////mesh_builder->set_uv( Vector2(u2,v1) ); mesh_builder->add_vertex( Vector3(  x2,  y1,0) );
	//mesh_builder->set_uv( Vector2(u1,v1) ); mesh_builder->add_vertex( Vector3(  x1,  y1,0) );
	//mesh_builder->set_uv( Vector2(u2,v1) ); mesh_builder->add_vertex( Vector3(  x2,  y1,0) );
	//mesh_builder->set_uv( Vector2(u2,v2) ); mesh_builder->add_vertex( Vector3(  x2,  y2,0) );
	//mesh_builder->set_uv( Vector2(u1,v2) ); mesh_builder->add_vertex( Vector3(  x1,  y2,0) );

  //mesh_builder->add_index( 0 );
  //mesh_builder->add_index( 2 );
  //mesh_builder->add_index( 3 );
  //mesh_builder->add_index( 0 );
  //mesh_builder->add_index( 1 );
  //mesh_builder->add_index( 2 );

  int vertex_count = 0;
  int texture_index = 0;
  int blend_mode = 0;
	// NORMAL   = 0
	// ADDITIVE = 1
	// MULTIPLY = 2
	// SCREEN   = 3

  // For each slot in the draw order array of the skeleton
  for (size_t i = 0, n=skeleton->getSlots().size(); i < n; ++i)
  {
    spine::Slot* slot = skeleton->getDrawOrder()[i];

    // Fetch the currently active attachment, continue
    // with the next slot in the draw order if no
    // attachment is active on the slot
    spine::Attachment* attachment = slot->getAttachment();
    if (!attachment) continue;

    // Fetch the blend mode from the slot and
    // translate it to the engine blend mode
    int new_blend_mode;
    switch (slot->getData().getBlendMode()) {
      case spine::BlendMode_Normal:
        new_blend_mode = 0;
        break;
      case spine::BlendMode_Additive:
        new_blend_mode = 1;
        break;
      case spine::BlendMode_Multiply:
        new_blend_mode = 2;
        break;
      case spine::BlendMode_Screen:
        new_blend_mode = 3;
        break;
      default:
        // unknown Spine blend mode, fall back to
        // normal blend mode
        new_blend_mode = 0;
    }

    if (new_blend_mode != texture_index && vertex_count)
    {
      // Render the current mesh_builder contents using the old blend mode.
      on_draw_callback.call( "call", texture_index, blend_mode );
      vertex_count = 0;
    }
    blend_mode = new_blend_mode;

    // Calculate the tinting color based on the skeleton's color
    // and the slot's color. Each color channel is given in the
    // range [0-1], you may have to multiply by 255 and cast to
    // and int if your engine uses integer ranges for color channels.
    spine::Color skeletonColor = skeleton->getColor();
    spine::Color slotColor = slot->getColor();
    godot::Color tint( skeletonColor.r*slotColor.r, skeletonColor.g*slotColor.g, skeletonColor.b*slotColor.b, skeletonColor.a*slotColor.a );

    // Vertices, indices, and texture depends on the type of attachment
    if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
    {
      // Cast to an spRegionAttachment so we can get the rendererObject
      // and compute the world vertices
      spine::RegionAttachment* regionAttachment = (spine::RegionAttachment*)attachment;

      // Computed the world vertices positions for the 4 vertices that make up
      // the rectangular region attachment. This assumes the world transform of the
      // bone to which the slot (and hence attachment) is attached has been calculated
      // before rendering via Skeleton::updateWorldTransform(). The vertex positions
      // will be written directoy into the vertices array, with a stride of sizeof(Vertex)
      vertex_data.setSize( 8, 0.0f );
      regionAttachment->computeWorldVertices( *slot, vertex_data, 0 );

      // Our engine specific Texture is stored in the AtlasRegion which was
      // assigned to the attachment on load. It represents the texture atlas
      // page that contains the image the region attachment is mapped to.
			int new_texture_index = (int)(intptr_t)(((spine::AtlasRegion*)regionAttachment->getRegion())->page->texture);
      if (new_texture_index != texture_index && vertex_count)
      {
        // Render the current mesh_builder contents using the old texture before starting on the new indices.
        on_draw_callback.call( "call", texture_index, blend_mode );
        vertex_count = 0;
      }
      texture_index = new_texture_index;

      // Add vertex, UV, and color information to the mesh builder.
      float* uvs = regionAttachment->getUVs().buffer();
      float* v_buffer = vertex_data.buffer();
      for (size_t j=0; j<8; j+=2)
      {
        mesh_builder->set_color( tint );
        mesh_builder->set_uv( Vector2(uvs[j],uvs[j+1]) );
        mesh_builder->add_vertex( Vector3(v_buffer[j],-v_buffer[j+1],0) );
      }

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
      // Cast to an MeshAttachment so we can get the rendererObject
      // and compute the world vertices
      spine::MeshAttachment* mesh = (spine::MeshAttachment*)attachment;

      // Ensure there is enough room for vertices
      vertex_data.setSize( mesh->getWorldVerticesLength(), 0.0f );

      // Computed the world vertices positions for the vertices that make up
      // the mesh attachment. This assumes the world transform of the
      // bone to which the slot (and hence attachment) is attached has been calculated
      // before rendering via Skeleton::updateWorldTransform(). The vertex positions will
      // be written directly into the vertex_data array, with a stride of sizeof(Vertex)
      mesh->computeWorldVertices( *slot, vertex_data );

      // Our engine specific Texture is stored in the AtlasRegion which was
      // assigned to the attachment on load. It represents the texture atlas
      // page that contains the image the region attachment is mapped to.

			int new_texture_index = (int)(intptr_t)(((spine::AtlasRegion*) mesh->getRegion())->page->texture);
      if (new_texture_index != texture_index && vertex_count)
      {
        // Render the current mesh_builder contents using the old texture before starting on the new indices.
        on_draw_callback.call( "call", texture_index, blend_mode );
        vertex_count = 0;
      }
      texture_index = new_texture_index;

      float* uvs = mesh->getUVs().buffer();
      float* v_buffer = vertex_data.buffer();
      size_t v_count  = vertex_data.size();
      for (size_t j=0; j<v_count; j+=2)
      {
        mesh_builder->set_color( tint );
        mesh_builder->set_uv( Vector2(uvs[j],uvs[j+1]) );
        mesh_builder->add_vertex( Vector3(v_buffer[j],-v_buffer[j+1],0) );
      }

      spine::Vector<unsigned short> indices = mesh->getTriangles();
      size_t index_count = indices.size();
      for (size_t j=0; j<index_count; ++j)
      {
        mesh_builder->add_index( vertex_count+indices[j] );
      }

      //float* uvs = mesh->getUVs().buffer();
      //float* v_buffer = vertex_data.buffer();
      //spine::Vector<unsigned short> indices = mesh->getTriangles();
      //size_t index_count = indices.size();
      //for (size_t j=0; j<index_count; ++j)
      //{
      //  int v = indices[j] << 1;
      //  mesh_builder->set_color( tint );
      //  mesh_builder->set_uv(          Vector2(uvs[v],      uvs[v+1]) );
      //  mesh_builder->add_vertex( Vector3(v_buffer[v],-v_buffer[v+1],0) );
      //}

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
