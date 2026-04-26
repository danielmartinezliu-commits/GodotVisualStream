#include "gvs_module_render_material.h"

#include "core/io/resource_loader.h"
#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/resources/material.h"
#include "scene/resources/particle_process_material.h"

namespace GodotVisualStream {

const char *GVSModuleRenderMaterial::TYPE_ID = "render_material";

void GVSModuleRenderMaterial::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_process_material", "material"), &GVSModuleRenderMaterial::set_process_material);
	ClassDB::bind_method(D_METHOD("get_process_material"),             &GVSModuleRenderMaterial::get_process_material);

	// Mirror the same restriction GPUParticles3D uses for its process_material property.
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "process_material",
			PROPERTY_HINT_RESOURCE_TYPE, "ParticleProcessMaterial,ShaderMaterial"),
			"set_process_material", "get_process_material");
}

void GVSModuleRenderMaterial::serialize_to(Dictionary &p_dict) const {
	if (process_material.is_valid()) {
		String path = process_material->get_path();
		if (!path.is_empty() && path.begins_with("res://")) {
			p_dict["process_material_path"] = path;
		}
	}
}

void GVSModuleRenderMaterial::deserialize_from(const Dictionary &p_dict) {
	if (p_dict.has("process_material_path")) {
		String path = p_dict["process_material_path"];
		if (!path.is_empty()) {
			process_material = ResourceLoader::load(path);
		}
	}
}

Vector<GVSModule::InspectorProp> GVSModuleRenderMaterial::get_inspector_props() const {
	Vector<InspectorProp> props;

	InspectorProp p;
	p.label         = "Material";
	p.prop_name     = "process_material";
	p.is_resource   = true;
	// Same type restriction as GPUParticles3D.process_material.
	p.resource_type  = "ParticleProcessMaterial,ShaderMaterial";
	p.resource_value = process_material;
	props.push_back(p);

	return props;
}

void GVSModuleRenderMaterial::set_prop_resource(const String &p_name, const Ref<Resource> &p_res) {
	if (p_name == "process_material") {
		process_material = p_res;
	}
}

void GVSModuleRenderMaterial::apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const {
	if (!process_material.is_valid()) {
		return;
	}
	// Replaces the default ParticleProcessMaterial with the user-assigned one.
	// This triggers Godot's internal particle shader compilation (same path as
	// setting GPUParticles3D.process_material in the editor).
	p_gpu->set_process_material(process_material);
}

}
