#include "gvs_module_position.h"

#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"

namespace GodotVisualStream {

const char *GVSModulePosition::TYPE_ID = "position";

void GVSModulePosition::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_offset", "v"), &GVSModulePosition::set_offset);
	ClassDB::bind_method(D_METHOD("get_offset"),      &GVSModulePosition::get_offset);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "offset"), "set_offset", "get_offset");
}

void GVSModulePosition::serialize_to(Dictionary &p_dict) const {
	Dictionary v;
	v["x"] = offset.x;
	v["y"] = offset.y;
	v["z"] = offset.z;
	p_dict["offset"] = v;
	if (!offset_binding.is_empty()) {
		p_dict["offset_binding"] = offset_binding;
	}
}

void GVSModulePosition::deserialize_from(const Dictionary &p_dict) {
	if (p_dict.has("offset")) {
		Dictionary v = p_dict["offset"];
		offset.x = v.has("x") ? float(v["x"]) : 0.0f;
		offset.y = v.has("y") ? float(v["y"]) : 0.0f;
		offset.z = v.has("z") ? float(v["z"]) : 0.0f;
	}
	offset_binding = p_dict.has("offset_binding") ? String(p_dict["offset_binding"]) : String();
}

Vector<GVSModule::InspectorProp> GVSModulePosition::get_inspector_props() const {
	Vector<InspectorProp> props;
	InspectorProp p;
	p.label         = "Offset";
	p.prop_name     = "offset";
	p.is_vector3    = true;
	p.vector3_value = offset;
	p.bound_var     = offset_binding;
	props.push_back(p);
	return props;
}

void GVSModulePosition::set_prop_binding(const String &p_name, const String &p_var) {
	if (p_name == "offset") { offset_binding = p_var; }
}

void GVSModulePosition::set_prop_vector3(const String &p_name, const Vector3 &p_val) {
	if (p_name == "offset") { offset = p_val; }
}

Vector3 GVSModulePosition::get_prop_vector3(const String &p_name) const {
	if (p_name == "offset") { return offset; }
	return Vector3();
}

void GVSModulePosition::apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *) const {
	p_gpu->set_position(offset);
}

}
