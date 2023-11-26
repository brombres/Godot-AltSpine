#include "SpineSpriteDefinitionData.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

SpineSpriteDefinitionData::SpineSpriteDefinitionData()
{
}

SpineSpriteDefinitionData::~SpineSpriteDefinitionData()
{
  if (skeleton_data)
  {
    delete skeleton_data;
    skeleton_data = nullptr;
  }
}

void SpineSpriteDefinitionData::configure( Variant spine_sprite_definition )
{
  this->spine_sprite_definition = spine_sprite_definition;
}

bool SpineSpriteDefinitionData::prepare_to_draw()
{
  if ( !skeleton_data )
  {
    Variant skeleton_resource = ((Object*)spine_sprite_definition)->get( "skeleton" );
    if ( !skeleton_resource ) return false;

    Variant bytes_variant = ((Object*)skeleton_resource)->get( "bytes" );
    if (bytes_variant.get_type() != Variant::Type::PACKED_BYTE_ARRAY) return false;

    PackedByteArray bytes = (PackedByteArray) bytes_variant;
    if (bytes.size() < 9) return false;

    bool is_binary = (bytes[0] != '{');
    if ( !is_binary )
    {
      // One more check
      is_binary = false;
      for (int i=0; i<9; ++i)
      {
        if (bytes[i] < 32 && bytes[i] != '\n' && bytes[i] != '\r')
        {
          is_binary = true;
          break;
        }
      }
    }

    if (is_binary)
    {
    }
    else
    {
    }
  }

  return false;
}

void SpineSpriteDefinitionData::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("configure","spine_sprite_definition"), &SpineSpriteDefinitionData::configure );
	ClassDB::bind_method( D_METHOD("prepare_to_draw"), &SpineSpriteDefinitionData::prepare_to_draw );
}
