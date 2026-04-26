#include "gvs_module_spawn_rate.h"

#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/resources/particle_process_material.h"

namespace GodotVisualStream {

const char *GVSModuleSpawnRate::TYPE_ID = "spawn_rate";

void GVSModuleSpawnRate::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_rate", "rate"), &GVSModuleSpawnRate::set_rate);
	ClassDB::bind_method(D_METHOD("get_rate"), &GVSModuleSpawnRate::get_rate);
	ClassDB::bind_method(D_METHOD("set_rate_var", "var"), &GVSModuleSpawnRate::set_rate_var);
	ClassDB::bind_method(D_METHOD("get_rate_var"), &GVSModuleSpawnRate::get_rate_var);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT,  "rate"),     "set_rate",     "get_rate");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "rate_var"), "set_rate_var", "get_rate_var");
}

void GVSModuleSpawnRate::serialize_to(Dictionary &p_dict) const {
	p_dict["rate"]     = rate;
	p_dict["rate_var"] = rate_var;
}

void GVSModuleSpawnRate::deserialize_from(const Dictionary &p_dict) {
	if (p_dict.has("rate"))     { rate     = float(p_dict["rate"]); }
	if (p_dict.has("rate_var")) { rate_var = String(p_dict["rate_var"]); }
}

Vector<GVSModule::InspectorProp> GVSModuleSpawnRate::get_inspector_props() const {
	Vector<InspectorProp> props;
	InspectorProp p;
	p.label    = "Rate";
	p.prop_name = "rate";
	p.value    = rate;
	p.bound_var = rate_var;
	p.min_val  = 0.0;
	p.max_val  = 100000.0;
	p.step     = 0.1;
	props.push_back(p);
	return props;
}

void GVSModuleSpawnRate::set_prop_value(const String &p_name, double p_val) {
	if (p_name == "rate") { rate = float(p_val); }
}

void GVSModuleSpawnRate::set_prop_binding(const String &p_name, const String &p_var) {
	if (p_name == "rate") { rate_var = p_var; }
}

void GVSModuleSpawnRate::apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const {
	// Godot doesn't have a direct spawn-rate property.
	// Rate (particles/sec) = amount / lifetime  →  amount = rate * lifetime.
	const double lifetime = p_gpu->get_lifetime();
	if (lifetime > 0.0) {
		p_gpu->set_amount(MAX(1, int(rate * lifetime)));
	}
}

}
