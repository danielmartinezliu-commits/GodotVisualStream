#include "gvs_module_lifetime.h"

#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/resources/particle_process_material.h"

namespace GodotVisualStream {

const char *GVSModuleLifetime::TYPE_ID = "lifetime";

void GVSModuleLifetime::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_lifetime", "val"), &GVSModuleLifetime::set_lifetime);
	ClassDB::bind_method(D_METHOD("get_lifetime"), &GVSModuleLifetime::get_lifetime);
	ClassDB::bind_method(D_METHOD("set_lifetime_var", "var"), &GVSModuleLifetime::set_lifetime_var);
	ClassDB::bind_method(D_METHOD("get_lifetime_var"), &GVSModuleLifetime::get_lifetime_var);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT,  "lifetime"),     "set_lifetime",     "get_lifetime");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "lifetime_var"), "set_lifetime_var", "get_lifetime_var");
}

void GVSModuleLifetime::serialize_to(Dictionary &p_dict) const {
	p_dict["lifetime"]     = lifetime;
	p_dict["lifetime_var"] = lifetime_var;
}

void GVSModuleLifetime::deserialize_from(const Dictionary &p_dict) {
	if (p_dict.has("lifetime"))     { lifetime     = float(p_dict["lifetime"]); }
	if (p_dict.has("lifetime_var")) { lifetime_var = String(p_dict["lifetime_var"]); }
}

Vector<GVSModule::InspectorProp> GVSModuleLifetime::get_inspector_props() const {
	Vector<InspectorProp> props;
	InspectorProp p;
	p.label     = "Lifetime";
	p.prop_name = "lifetime";
	p.value     = lifetime;
	p.bound_var = lifetime_var;
	p.min_val   = 0.01;
	p.max_val   = 600.0;
	p.step      = 0.01;
	props.push_back(p);
	return props;
}

void GVSModuleLifetime::set_prop_value(const String &p_name, double p_val) {
	if (p_name == "lifetime") { lifetime = float(p_val); }
}

void GVSModuleLifetime::set_prop_binding(const String &p_name, const String &p_var) {
	if (p_name == "lifetime") { lifetime_var = p_var; }
}

void GVSModuleLifetime::apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const {
	p_gpu->set_lifetime(lifetime);
}

}
