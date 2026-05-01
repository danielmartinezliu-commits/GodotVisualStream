#include "gvs_module_facing.h"

#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/resources/material.h"
#include "scene/resources/3d/primitive_meshes.h"

namespace GodotVisualStream {

const char *GVSModuleFacing::TYPE_ID = "facing";

void GVSModuleFacing::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_facing_mode", "mode"), &GVSModuleFacing::set_facing_mode);
	ClassDB::bind_method(D_METHOD("get_facing_mode"), &GVSModuleFacing::get_facing_mode);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "facing_mode"), "set_facing_mode", "get_facing_mode");
}

void GVSModuleFacing::serialize_to(Dictionary &p_dict) const {
	p_dict["facing_mode"] = facing_mode;
}

void GVSModuleFacing::deserialize_from(const Dictionary &p_dict) {
	if (p_dict.has("facing_mode")) {
		facing_mode = CLAMP(int(p_dict["facing_mode"]), 0, 2);
	}
}

Vector<GVSModule::InspectorProp> GVSModuleFacing::get_inspector_props() const {
	Vector<InspectorProp> props;

	InspectorProp p;
	p.label      = "Mode";
	p.prop_name  = "facing_mode";
	p.is_enum    = true;
	p.enum_value = facing_mode;
	p.enum_items.push_back("Camera Plane");
	p.enum_items.push_back("Camera Position");
	p.enum_items.push_back("Velocity");
	props.push_back(p);

	return props;
}

void GVSModuleFacing::set_prop_enum(const String &p_name, int p_idx) {
	if (p_name == "facing_mode") {
		facing_mode = CLAMP(p_idx, 0, 2);
	}
}

void GVSModuleFacing::apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const {
	Ref<Mesh> draw_mesh = p_gpu->get_draw_pass_mesh(0);
	if (!draw_mesh.is_valid()) { return; }

	Ref<PrimitiveMesh> prim = draw_mesh;
	if (!prim.is_valid()) { return; }

	Ref<StandardMaterial3D> mat = prim->get_material();
	if (!mat.is_valid()) {
		mat.instantiate();
		prim->set_material(mat);
	}

	BaseMaterial3D::BillboardMode bm;
	switch (facing_mode) {
		case 0:  bm = BaseMaterial3D::BILLBOARD_ENABLED;   break; // Camera Plane
		case 1:  bm = BaseMaterial3D::BILLBOARD_ENABLED;   break; // Camera Position
		case 2:  bm = BaseMaterial3D::BILLBOARD_PARTICLES; break; // Velocity
		default: bm = BaseMaterial3D::BILLBOARD_ENABLED;   break;
	}
	mat->set_billboard_mode(bm);
}

}
