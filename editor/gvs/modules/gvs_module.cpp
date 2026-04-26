#include "gvs_module.h"

#include "core/object/class_db.h"
#include "gvs_module_spawn_rate.h"
#include "gvs_module_initial_velocity.h"
#include "gvs_module_lifetime.h"
#include "gvs_module_render_material.h"

namespace GodotVisualStream {

void GVSModule::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_type_id", "type"), &GVSModule::set_type_id);
	ClassDB::bind_method(D_METHOD("get_type_id"), &GVSModule::get_type_id);
	ClassDB::bind_method(D_METHOD("set_enabled", "enabled"), &GVSModule::set_enabled);
	ClassDB::bind_method(D_METHOD("get_enabled"), &GVSModule::get_enabled);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "type_id"), "set_type_id", "get_type_id");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL,   "enabled"), "set_enabled", "get_enabled");
}

Ref<GVSModule> GVSModule::create(const String &p_type_id) {
	Ref<GVSModule> mod;

	if (p_type_id == GVSModuleSpawnRate::TYPE_ID) {
		Ref<GVSModuleSpawnRate> m;
		m.instantiate();
		mod = m;
	} else if (p_type_id == GVSModuleInitialVelocity::TYPE_ID) {
		Ref<GVSModuleInitialVelocity> m;
		m.instantiate();
		mod = m;
	} else if (p_type_id == GVSModuleLifetime::TYPE_ID) {
		Ref<GVSModuleLifetime> m;
		m.instantiate();
		mod = m;
	} else if (p_type_id == GVSModuleRenderMaterial::TYPE_ID) {
		Ref<GVSModuleRenderMaterial> m;
		m.instantiate();
		mod = m;
	} else {
		mod.instantiate();
	}

	mod->set_type_id(p_type_id);
	return mod;
}

}
