#include "SpineSpriteDefinitionData.h"
#include "SpineAtlasResourceData.h"

#include <godot_cpp/core/class_db.hpp>

#include <spine/Atlas.h>
#include <spine/SkeletonBinary.h>
#include <spine/SkeletonJson.h>

using namespace godot;

SpineSpriteDefinitionData::SpineSpriteDefinitionData()
{
}

SpineSpriteDefinitionData::~SpineSpriteDefinitionData()
{
  reset();
}

bool SpineSpriteDefinitionData::configure( Variant spine_sprite_definition, Variant gd_atlas )
{
  reset();

  this->spine_sprite_definition = spine_sprite_definition;
  ((Object*)spine_sprite_definition)->set( "_data_pointer", (uint64_t)(intptr_t)(void*)this );

  Variant skeleton_resource = ((Object*)spine_sprite_definition)->get( "skeleton" );
  if ( !skeleton_resource ) return false;

  Variant bytes_variant = ((Object*)skeleton_resource)->get( "bytes" );
  if (bytes_variant.get_type() != Variant::Type::PACKED_BYTE_ARRAY) return false;

  PackedByteArray bytes = (PackedByteArray) bytes_variant;
  if (bytes.size() < 9) return false;

  bool is_binary = (bytes[0] != '{');
  if ( !is_binary )
  {
    // Probably not binary but one more check
    for (int i=0; i<9; ++i)
    {
      if (bytes[i] < 32 && bytes[i] != '\n' && bytes[i] != '\r')
      {
        is_binary = true;
        break;
      }
    }
  }

  SpineAtlasResourceData* atlas_data = (SpineAtlasResourceData*)(void*)(intptr_t)(uint64_t)(((Object*)gd_atlas)->get("_atlas_data_pointer"));
  if ( !atlas_data ) return false;

  spine::Atlas* atlas = atlas_data->atlas;
  if ( !atlas ) return false;

  if (is_binary)
  {
    spine::SkeletonBinary binary_loader( atlas );
    skeleton_data = binary_loader.readSkeletonData( bytes.ptr(), bytes.size() );
    if (!skeleton_data)
    {
      UtilityFunctions::print( "Error loading binary skeleton ", ((Object*)skeleton_resource)->get("resource_path"), ":", binary_loader.getError().buffer() );
      return false;
    }
  }
  else
  {
    spine::SkeletonJson json_loader( atlas );
    skeleton_data = json_loader.readSkeletonData( (const char*)bytes.get_string_from_utf8().ptr() );
    if (!skeleton_data)
    {
      UtilityFunctions::print( "Error loading JSON skeleton ", ((Object*)skeleton_resource)->get("resource_path"), ":", json_loader.getError().buffer() );
      return false;
    }
  }

  animation_state_data = new spine::AnimationStateData( skeleton_data );
  update_mix_times();

  return true;
}

bool SpineSpriteDefinitionData::is_ready( Variant gd_atlas )
{
  if ( !skeleton_data && !configure(spine_sprite_definition,gd_atlas) ) return false;
  return true;
}

void SpineSpriteDefinitionData::reset()
{
  if (skeleton_data)
  {
    delete skeleton_data;
    skeleton_data = nullptr;
  }
  if (animation_state_data)
  {
    delete animation_state_data;
    animation_state_data = nullptr;
  }
}

void SpineSpriteDefinitionData::update_mix_times()
{
  // TODO
  if ( !animation_state_data ) return;
  animation_state_data->setDefaultMix( 0.5f );
}

void SpineSpriteDefinitionData::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("configure","spine_sprite_definition"), &SpineSpriteDefinitionData::configure );
	ClassDB::bind_method( D_METHOD("is_ready","atlas"),             &SpineSpriteDefinitionData::is_ready );
	ClassDB::bind_method( D_METHOD("reset"),                               &SpineSpriteDefinitionData::reset );
	ClassDB::bind_method( D_METHOD("update_mix_times"),                    &SpineSpriteDefinitionData::update_mix_times );
}
