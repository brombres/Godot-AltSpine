#include "SpineSpriteData.h"
#include "SpineSpriteDefinitionData.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

//==============================================================================
// SpineSpriteAnimationEventListener
//==============================================================================
void SpineSpriteAnimationEventListener::callback( spine::AnimationState *state,
    spine::EventType type, spine::TrackEntry *entry, spine::Event *event )
{
  Variant user_event_name;
  if (event) user_event_name = String( event->getData().getName().buffer() );

  if (data && data->animation_state)
  {
    data->spine_sprite->call(
      "_handle_animation_event",
      (int)type,
      (int64_t)(intptr_t)entry,
      String(entry->getAnimation()->getName().buffer()),
      entry->getTrackIndex(),
      user_event_name
    );
  }
}

//==============================================================================
// SpineSpriteData
//==============================================================================

SpineSpriteData::SpineSpriteData()
{
}

SpineSpriteData::~SpineSpriteData()
{
  reset();
}

int64_t SpineSpriteData::add_animation( int track_index, String name, bool looping, float delay, float time_scale )
{
  if ( !animation_state ) return 0;

  spine::TrackEntry* entry = animation_state->addAnimation( track_index, (const char*)name.utf8(), looping, delay );

  if (time_scale < 0)
  {
    entry->setReverse( true );
    time_scale = -time_scale;
  }

  entry->setTimeScale( time_scale );
  return (int64_t)(intptr_t)entry;
}

void SpineSpriteData::add_empty_animation( int track_index, float mix_duration, float delay )
{
  if (animation_state) animation_state->addEmptyAnimation( track_index, mix_duration, delay );
}

void SpineSpriteData::clear_track( int track_index )
{
  if (animation_state) animation_state->clearTrack( track_index );
}

void SpineSpriteData::clear_tracks()
{
  if (animation_state) animation_state->clearTracks();
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

String SpineSpriteData::get_bone_name( int64_t bone_pointer )
{
  if ( !bone_pointer ) return "null";
  spine::String name = ((spine::Bone*)(intptr_t)bone_pointer)->getData().getName();
  return (const char*) name.buffer();
}

Array SpineSpriteData::get_bones()
{
  Array result;
  if ( !skeleton ) return result;
  spine::SkeletonData* skeleton_data = skeleton->getData();
  if ( !skeleton_data ) return result;

  spine::Vector<spine::Bone*> bones = skeleton->getBones();
  for (int i=0; i<bones.size(); ++i)
  {
    result.push_back( (int64_t)(intptr_t)bones[i] );
  }

  return result;
}

int64_t SpineSpriteData::get_point_attachment( String slot_name, String attachment_name )
{
  if ( !skeleton ) return 0;
  spine::Attachment* attachment = skeleton->getAttachment( (const char*)slot_name.utf8(), (const char*)attachment_name.utf8() );
  if (attachment && attachment->getRTTI().isExactly(spine::PointAttachment::rtti))
  {
    return (int64_t)(intptr_t)attachment;
  }
  else
  {
    return 0;
  }
}

Vector2 SpineSpriteData::get_point_attachment_local_position( int64_t attachment_pointer )
{
  spine::PointAttachment* attachment = (spine::PointAttachment*)(int64_t)attachment_pointer;
  if ( !attachment ) return Vector2(0,0);
  return Vector2( attachment->getX(), -attachment->getY() );
}

float SpineSpriteData::get_point_attachment_local_rotation( int64_t attachment_pointer )
{
  spine::PointAttachment* attachment = (spine::PointAttachment*)(int64_t)attachment_pointer;
  if ( !attachment ) return 0;
  return attachment->getRotation() * acos(-1.0) / -180.0;  // DEG to RAD
}

Vector2 SpineSpriteData::get_point_attachment_position( int64_t attachment_pointer, String slot_name )
{
  spine::PointAttachment* attachment = (spine::PointAttachment*)(int64_t)attachment_pointer;
  if ( !attachment ) return Vector2(0,0);

  spine::Slot* slot = skeleton->findSlot( (const char*)slot_name.utf8() );
  if ( !slot ) return Vector2(0,0);

  float x, y;
  attachment->computeWorldPosition( slot->getBone(), x, y );
  return Vector2( x, -y );
}

float SpineSpriteData::get_point_attachment_rotation( int64_t attachment_pointer, String slot_name )
{
  spine::PointAttachment* attachment = (spine::PointAttachment*)(int64_t)attachment_pointer;
  if ( !attachment ) return 0;

  spine::Slot* slot = skeleton->findSlot( (const char*)slot_name.utf8() );
  if ( !slot ) return 0;

  return attachment->computeWorldRotation( slot->getBone() ) * acos(-1.0) / -180.0;  // DEG to RAD, negated
}

int64_t SpineSpriteData::get_skin()
{
  if ( !skeleton ) return 0;
  return (int64_t)(intptr_t)skeleton->getSkin();
}

String SpineSpriteData::get_skin_name( int64_t skin_pointer )
{
  if ( !skin_pointer ) return "null";
  spine::String name = ((spine::Skin*)(intptr_t)skin_pointer)->getName();
  return (const char*) name.buffer();
}

Array SpineSpriteData::get_skins()
{
  Array result;
  if ( !skeleton ) return result;
  spine::SkeletonData* skeleton_data = skeleton->getData();
  if ( !skeleton_data ) return result;

  spine::Vector<spine::Skin*> skins = skeleton_data->getSkins();
  for (int i=0; i<skins.size(); ++i)
  {
    result.push_back( (int64_t)(intptr_t)skins[i] );
  }

  return result;
}

float SpineSpriteData::get_time_scale()
{
  return animation_state && animation_state->getTimeScale();
}

bool SpineSpriteData::is_ready()
{
  if ( !skeleton )
  {
    Variant v_def = ((Object*)spine_sprite)->get("definition");
    if ( !v_def ) return false;

    SpineSpriteDefinitionData* def = (SpineSpriteDefinitionData*)(void*)(intptr_t)(uint64_t)(((Object*)v_def)->get("_data_pointer"));
    if ( !def ) return false;
    if ( !def->skeleton_data || !def->animation_state_data ) return false;

    skeleton = new spine::Skeleton( def->skeleton_data );
    animation_state = new spine::AnimationState( def->animation_state_data );
    listener.data = this;
    animation_state->setListener( &listener );
  }
  return true;
}

void SpineSpriteData::reset()
{
  if (custom_skin)
  {
    delete custom_skin;
    custom_skin = nullptr;
    if (skeleton) skeleton->setSkin( custom_skin );
  }

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

int64_t SpineSpriteData::set_animation( int track_index, String name, bool looping, float time_scale )
{
  if ( !animation_state ) return 0;

  spine::TrackEntry* entry = animation_state->setAnimation( track_index, (const char*)name.utf8(), looping );

  if (time_scale < 0)
  {
    entry->setReverse( true );
    time_scale = -time_scale;
  }

  entry->setTimeScale( time_scale );
  return (int64_t)(intptr_t)entry;
}

void SpineSpriteData::set_attachment( String slot_name, Variant attachment_name )
{
  if (skeleton)
  {
    if ( !attachment_name ) attachment_name = "";
    skeleton->setAttachment( (const char*)slot_name.utf8(), (const char*)String(attachment_name).utf8() );
  }
}

void SpineSpriteData::set_point_attachment_local_position( int64_t attachment_pointer, Vector2 position )
{
  spine::PointAttachment* attachment = (spine::PointAttachment*)(int64_t)attachment_pointer;
  if ( !attachment ) return;
  attachment->setX( position.x );
  attachment->setY( -position.y );
}

void SpineSpriteData::set_point_attachment_local_rotation( int64_t attachment_pointer, float rotation )
{
  spine::PointAttachment* attachment = (spine::PointAttachment*)(int64_t)attachment_pointer;
  if ( !attachment ) return;
  attachment->setRotation( rotation * -180.0 / acos(-1.0) );   // RAD to DEG
}

void SpineSpriteData::set_empty_animation( int track_index, float mix_duration )
{
  if (animation_state) animation_state->setEmptyAnimation( track_index, mix_duration );
}

void SpineSpriteData::set_empty_animations( float mix_duration )
{
  if (animation_state) animation_state->setEmptyAnimations( mix_duration );
}

// new_skin can be:
//   - null to reset to default
//   - String name
//   - int Skin pointer
//   - Array of skin name strings
void SpineSpriteData::set_skin( Variant new_skin )
{
  if (skeleton)
  {
    if (custom_skin)
    {
      delete custom_skin;
      custom_skin = nullptr;
      skeleton->setSkin( custom_skin );
    }

    spine::Skin* skin = nullptr;
    spine::SkeletonData* skeleton_data = skeleton->getData();

    switch (new_skin.get_type())
    {
      case Variant::Type::STRING:
      {
        skin = skeleton_data->findSkin( (const char*)String(new_skin).utf8() );
        break;
      }
      case Variant::Type::INT:
      {
        skin = (spine::Skin*)(intptr_t)(int64_t)new_skin;
        break;
      }
      case Variant::Type::ARRAY:
      {
        custom_skin = new spine::Skin( "godot-custom-skin" );
        Array array = (Array) new_skin;
        for (int i=0; i<array.size(); ++i)
        {
          Variant element = array[i];
          spine::Skin* skin_element = nullptr;
          switch (element.get_type())
          {
            case Variant::Type::STRING:
            {
              skin_element = skeleton_data->findSkin( (const char*)String(element).utf8() );
              break;
            }
            case Variant::Type::INT:
            {
              skin_element = (spine::Skin*)(intptr_t)(int64_t)element;
              break;
            }
            default:
            {
            }
          }

          if (skin_element)
          {
            custom_skin->addSkin( skin_element );
          }
        }
        skin = custom_skin;
        break;
      }
      default:
      {
      }
    }

    skeleton->setSkin( skin );
    skeleton->setSlotsToSetupPose();
  }
}

void SpineSpriteData::set_time_scale( float scale )
{
  if (animation_state) animation_state->setTimeScale( scale );
}

void SpineSpriteData::update( double dt )
{
  animation_state->update( dt );
  animation_state->apply( *skeleton );
  skeleton->updateWorldTransform();
}

void SpineSpriteData::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("add_animation","track_index","name","looping","delay","time_scale"), &SpineSpriteData::add_animation );
	ClassDB::bind_method( D_METHOD("add_empty_animation","track_index","mix_duration","delay"), &SpineSpriteData::add_empty_animation );
	ClassDB::bind_method( D_METHOD("clear_track","track_index"),              &SpineSpriteData::clear_track );
	ClassDB::bind_method( D_METHOD("clear_tracks"),                           &SpineSpriteData::clear_tracks );
	ClassDB::bind_method( D_METHOD("configure","spine_sprite"),	              &SpineSpriteData::configure );
	ClassDB::bind_method( D_METHOD("draw","mesh_builder","on_draw_callback"), &SpineSpriteData::draw );
	ClassDB::bind_method( D_METHOD("get_bone_name","bone_pointer"),           &SpineSpriteData::get_bone_name );
	ClassDB::bind_method( D_METHOD("get_bones"),                              &SpineSpriteData::get_bones );
	ClassDB::bind_method( D_METHOD("get_point_attachment","slot_name","attachment_name"), &SpineSpriteData::get_point_attachment );
	ClassDB::bind_method( D_METHOD("get_point_attachment_local_position","attachment_pointer"),
                        &SpineSpriteData::get_point_attachment_local_position );
	ClassDB::bind_method( D_METHOD("get_point_attachment_local_rotation","attachment_pointer"),
                        &SpineSpriteData::get_point_attachment_local_rotation );
	ClassDB::bind_method( D_METHOD("get_point_attachment_position","attachment_pointer","slot_name"),
                        &SpineSpriteData::get_point_attachment_position );
	ClassDB::bind_method( D_METHOD("get_point_attachment_rotation","attachment_pointer","slot_name"),
                        &SpineSpriteData::get_point_attachment_rotation );
	ClassDB::bind_method( D_METHOD("get_skin"),                               &SpineSpriteData::get_skin );
	ClassDB::bind_method( D_METHOD("get_skin_name"),                            &SpineSpriteData::get_skin_name ),
	ClassDB::bind_method( D_METHOD("get_skins"),                                &SpineSpriteData::get_skins );
	ClassDB::bind_method( D_METHOD("get_time_scale"),                           &SpineSpriteData::get_time_scale );
	ClassDB::bind_method( D_METHOD("is_ready"),	                                &SpineSpriteData::is_ready );
	ClassDB::bind_method( D_METHOD("reset"),                                    &SpineSpriteData::reset );
	ClassDB::bind_method( D_METHOD("set_animation","track_index","name","looping","time_scale"), &SpineSpriteData::set_animation );
	ClassDB::bind_method( D_METHOD("set_attachment","slot_name","attachment_name"), &SpineSpriteData::set_attachment );
	ClassDB::bind_method( D_METHOD("set_point_attachment_local_position","position"),
                        &SpineSpriteData::set_point_attachment_local_position );
	ClassDB::bind_method( D_METHOD("set_point_attachment_local_rotation","rotation"),
                        &SpineSpriteData::set_point_attachment_local_rotation );
	ClassDB::bind_method( D_METHOD("set_empty_animation","track_index","mix_duration"), &SpineSpriteData::set_empty_animation );
	ClassDB::bind_method( D_METHOD("set_empty_animations","mix_duration"),    &SpineSpriteData::set_empty_animations );
	ClassDB::bind_method( D_METHOD("set_skin","new_skin"),                    &SpineSpriteData::set_skin );
	ClassDB::bind_method( D_METHOD("set_time_scale","scale"),                 &SpineSpriteData::set_time_scale );
	ClassDB::bind_method( D_METHOD("update","dt"),	                          &SpineSpriteData::update );
}
