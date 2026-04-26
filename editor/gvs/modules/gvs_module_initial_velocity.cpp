#include "gvs_module_initial_velocity.h"

#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/resources/particle_process_material.h"

namespace GodotVisualStream {

const char *GVSModuleInitialVelocity::TYPE_ID = "initial_velocity";

void GVSModuleInitialVelocity::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_velocity_min", "v"), &GVSModuleInitialVelocity::set_velocity_min);
	ClassDB::bind_method(D_METHOD("get_velocity_min"), &GVSModuleInitialVelocity::get_velocity_min);
	ClassDB::bind_method(D_METHOD("set_velocity_min_var", "var"), &GVSModuleInitialVelocity::set_velocity_min_var);
	ClassDB::bind_method(D_METHOD("get_velocity_min_var"), &GVSModuleInitialVelocity::get_velocity_min_var);

	ClassDB::bind_method(D_METHOD("set_velocity_max", "v"), &GVSModuleInitialVelocity::set_velocity_max);
	ClassDB::bind_method(D_METHOD("get_velocity_max"), &GVSModuleInitialVelocity::get_velocity_max);
	ClassDB::bind_method(D_METHOD("set_velocity_max_var", "var"), &GVSModuleInitialVelocity::set_velocity_max_var);
	ClassDB::bind_method(D_METHOD("get_velocity_max_var"), &GVSModuleInitialVelocity::get_velocity_max_var);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT,  "velocity_min"),     "set_velocity_min",     "get_velocity_min");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "velocity_min_var"), "set_velocity_min_var", "get_velocity_min_var");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT,  "velocity_max"),     "set_velocity_max",     "get_velocity_max");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "velocity_max_var"), "set_velocity_max_var", "get_velocity_max_var");
}

void GVSModuleInitialVelocity::serialize_to(Dictionary &p_dict) const {
	p_dict["velocity_min"]     = velocity_min;
	p_dict["velocity_min_var"] = velocity_min_var;
	p_dict["velocity_max"]     = velocity_max;
	p_dict["velocity_max_var"] = velocity_max_var;
}

void GVSModuleInitialVelocity::deserialize_from(const Dictionary &p_dict) {
	if (p_dict.has("velocity_min"))     { velocity_min     = float(p_dict["velocity_min"]); }
	if (p_dict.has("velocity_min_var")) { velocity_min_var = String(p_dict["velocity_min_var"]); }
	if (p_dict.has("velocity_max"))     { velocity_max     = float(p_dict["velocity_max"]); }
	if (p_dict.has("velocity_max_var")) { velocity_max_var = String(p_dict["velocity_max_var"]); }
}

Vector<GVSModule::InspectorProp> GVSModuleInitialVelocity::get_inspector_props() const {
	Vector<InspectorProp> props;

	InspectorProp pmin;
	pmin.label     = "Vel Min";
	pmin.prop_name = "velocity_min";
	pmin.value     = velocity_min;
	pmin.bound_var = velocity_min_var;
	pmin.min_val   = -10000.0;
	pmin.max_val   =  10000.0;
	pmin.step      = 0.01;
	props.push_back(pmin);

	InspectorProp pmax;
	pmax.label     = "Vel Max";
	pmax.prop_name = "velocity_max";
	pmax.value     = velocity_max;
	pmax.bound_var = velocity_max_var;
	pmax.min_val   = -10000.0;
	pmax.max_val   =  10000.0;
	pmax.step      = 0.01;
	props.push_back(pmax);

	return props;
}

void GVSModuleInitialVelocity::set_prop_value(const String &p_name, double p_val) {
	if (p_name == "velocity_min")      { velocity_min = float(p_val); }
	else if (p_name == "velocity_max") { velocity_max = float(p_val); }
}

void GVSModuleInitialVelocity::set_prop_binding(const String &p_name, const String &p_var) {
	if (p_name == "velocity_min")      { velocity_min_var = p_var; }
	else if (p_name == "velocity_max") { velocity_max_var = p_var; }
}

void GVSModuleInitialVelocity::apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const {
	p_mat->set_param_min(ParticleProcessMaterial::PARAM_INITIAL_LINEAR_VELOCITY, velocity_min);
	p_mat->set_param_max(ParticleProcessMaterial::PARAM_INITIAL_LINEAR_VELOCITY, velocity_max);
}

}
