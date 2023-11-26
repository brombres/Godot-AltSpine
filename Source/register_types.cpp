#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include "SpineSpriteData.h"
#include "SpineSpriteDefinitionData.h"
#include "SpineAtlasResourceData.h"

using namespace godot;

void initialize_GDSpine_types(ModuleInitializationLevel p_level)
{
  // --- Initialization levels ---
  // MODULE_INITIALIZATION_LEVEL_CORE
  // MODULE_INITIALIZATION_LEVEL_SERVERS
  // MODULE_INITIALIZATION_LEVEL_SCENE
  // MODULE_INITIALIZATION_LEVEL_EDITOR
  //godot::UtilityFunctions::print( "[GDSpine] Initializing (", p_level, ")" );

  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
  {
    return;
  }

  // ClassDB::register_class<ClassName>();
  ClassDB::register_class<SpineSpriteData>();
  ClassDB::register_class<SpineSpriteDefinitionData>();
  ClassDB::register_class<SpineAtlasResourceData>();
}

void uninitialize_GDSpine_types(ModuleInitializationLevel p_level)
{
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE)
  {
    return;
  }
}

extern "C"
{
  GDExtensionBool GDE_EXPORT GDSpine_library_init(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization *r_initialization )
  {
    GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_GDSpine_types);
    init_obj.register_terminator(uninitialize_GDSpine_types);
    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
  }
}
