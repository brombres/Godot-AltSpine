#include "SpineSpriteData.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

SpineSpriteData::SpineSpriteData()
{
}

SpineSpriteData::~SpineSpriteData()
{
}

void SpineSpriteData::configure( Node* spine_sprite )
{
  this->spine_sprite = spine_sprite;

  if (spine_sprite)
  {
    UtilityFunctions::print( "[SpineSpriteData.configure] sprite exists" );
    Variant def = spine_sprite->get( "definition" );
    UtilityFunctions::print( "definition exists:", !!def );
  }
}

//Camera3D* SpineSpriteData::get_camera()
//{
//	// Result can be 'nullptr'.
//	return Object::cast_to<Camera3D>( ObjectDB::get_instance(camera) );
//}
//
//void SpineSpriteData::set_camera( Camera3D* p_camera )
//{
//	camera = p_camera ? p_camera->get_instance_id() : ObjectID();
//}
//
//int SpineSpriteData::add_one( int parameter )
//{
//	return parameter + 1;
//}

void SpineSpriteData::_bind_methods()
{
	ClassDB::bind_method( D_METHOD("configure","spine_sprite"),	&SpineSpriteData::configure );
	//ClassDB::bind_method( D_METHOD("get_material" ),				 &SpineSpriteData::get_material );
	//ClassDB::bind_method( D_METHOD("get_camera" ),					 &SpineSpriteData::get_camera );
	//ClassDB::bind_method( D_METHOD("get_position" ),				 &SpineSpriteData::get_position );
	//ClassDB::bind_method( D_METHOD("get_scale" ),						&SpineSpriteData::get_scale );
	//ClassDB::bind_method( D_METHOD("set_camera", "value"),	 &SpineSpriteData::set_camera );
	//ClassDB::bind_method( D_METHOD("set_material", "value"), &SpineSpriteData::set_material );
	//ClassDB::bind_method( D_METHOD("set_position", "value"), &SpineSpriteData::set_position );
	//ClassDB::bind_method( D_METHOD("set_scale", "value"),		&SpineSpriteData::set_scale );
	//ClassDB::bind_method( D_METHOD("add_one", "parameter"),	&SpineSpriteData::add_one, DEFVAL(1) );
	//ADD_PROPERTY( PropertyInfo(Variant::OBJECT,	"material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_material", "get_material");
	//ADD_PROPERTY( PropertyInfo(Variant::OBJECT,	"camera",	 PROPERTY_HINT_NODE_TYPE, "Camera3D"), "set_camera", "get_camera");
	//ADD_PROPERTY( PropertyInfo(Variant::VECTOR3, "position"), "set_position", "get_position");
	//ADD_PROPERTY( PropertyInfo(Variant::FLOAT,	 "scale",		PROPERTY_HINT_RANGE, "0.0,1.0,0.05,or_less,or_greater"), "set_scale", "get_scale");
}
