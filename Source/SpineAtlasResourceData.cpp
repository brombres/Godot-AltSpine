#include "SpineAtlasResourceData.h"

#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/core/class_db.hpp>

#include <spine/Extension.h>
#include <spine/TextureLoader.h>

using namespace godot;

spine::SpineExtension* spine::getDefaultExtension()
{
   return new DefaultSpineExtension();
}

class SpineSpriteTextureLoader : public spine::TextureLoader
{
  public:
    Variant textures;

    SpineSpriteTextureLoader( Variant textures ) : textures(textures)
    {
    }

    virtual ~SpineSpriteTextureLoader()
    {
    }

		virtual void load( spine::AtlasPage &page, const spine::String &path )
    {
      if (textures)
      {
        Array array = (Array)textures;
        String filepath = (const char*) path.buffer();
        for (int i=0; i<array.size(); ++i)
        {
          Texture2D* texture = (Texture2D*)(Object*)array[i];
          if (texture && texture->get("resource_path") == filepath)
          {
            page.texture = texture;
            break;
          }
        }
      }
    }

		virtual void unload( void *texture )
    {
      // noAction
    }
};


SpineAtlasResourceData::SpineAtlasResourceData()
{
}

SpineAtlasResourceData::~SpineAtlasResourceData()
{
  if (atlas)
  {
    delete atlas;
    atlas = nullptr;
  }
}

void SpineAtlasResourceData::configure( Variant spine_atlas_resource )
{
  this->spine_atlas_resource = spine_atlas_resource;
}

bool SpineAtlasResourceData::prepare_to_draw()
{
  if ( !spine_atlas_resource ) return false;

  if ( !atlas )
  {
    Variant v_bytes = ((Object*)spine_atlas_resource)->get( "bytes" );
    if (v_bytes.get_type() != Variant::Type::PACKED_BYTE_ARRAY) return false;

    Variant v_textures = ((Object*)spine_atlas_resource)->get( "textures" );

    PackedByteArray bytes = (PackedByteArray)v_bytes;
    atlas = new spine::Atlas( (const char*)bytes.ptr(), bytes.size(), "res://", new SpineSpriteTextureLoader(v_textures) );
  }

  //Variant normal_maps = ((Object*)spine_atlas_resource)->get( "normal_maps" );
  return true;
}

void SpineAtlasResourceData::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("configure","spine_atlas_resource"), &SpineAtlasResourceData::configure );
	ClassDB::bind_method( D_METHOD("prepare_to_draw"), &SpineAtlasResourceData::prepare_to_draw );
}
