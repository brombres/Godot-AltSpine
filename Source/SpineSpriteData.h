// REFERENCE
// https://docs.godotengine.org/en/stable/contributing/development/core_and_modules/object_class.html

#ifndef SPINESPRITEDATA_H
#define SPINESPRITEDATA_H

#ifdef WIN32
	#include <windows.h>
#endif

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <spine/spine.h>
#include <spine/Extension.h>

using namespace godot;

class SpineSpriteData : public RefCounted
{
	GDCLASS( SpineSpriteData, RefCounted );

	protected:
		static void _bind_methods();

	public:
		// PROPERTIES
    Node*     spine_sprite;
		//float				 scale = 1.0f;
		//Vector3			 position;
		//ObjectID			camera;
		//Ref<Material> material;

		// CONSTRUCTOR METHODS
		SpineSpriteData();
		~SpineSpriteData();

		// PROPERTY ACCESS METHODS
		//Ref<Material> get_material() { return material; }
		//void					set_material( const Ref<Material> &p_material ) { material = p_material; }

		//Camera3D* get_camera();
		//void			set_camera( Camera3D* p_camera );

		//Vector3 get_position() { return position; }
		//void		set_position( Vector3 p_position ) { position = p_position; }

		//float get_scale() { return scale; }
		//void	set_scale( float p_scale ) { scale = p_scale; }

		// GENERAL METHODS
    void configure( Node* spine_sprite );
    bool prepare_to_draw();

		//int add_one( int parameter );
};

#endif // SPINESPRITEDATA_H
