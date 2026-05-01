#include "gvs_emitter_node.h"

#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "scene/resources/particle_process_material.h"
#include "editor/gvs/modules/gvs_module.h"

namespace GodotVisualStream {

static int _node_id_counter = 0;

void GVSEmitterNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_id", "id"), &GVSEmitterNode::set_id);
	ClassDB::bind_method(D_METHOD("get_id"), &GVSEmitterNode::get_id);
	ClassDB::bind_method(D_METHOD("set_title", "title"), &GVSEmitterNode::set_title);
	ClassDB::bind_method(D_METHOD("get_title"), &GVSEmitterNode::get_title);
	ClassDB::bind_method(D_METHOD("set_canvas_pos", "pos"), &GVSEmitterNode::set_canvas_pos);
	ClassDB::bind_method(D_METHOD("get_canvas_pos"), &GVSEmitterNode::get_canvas_pos);

	ADD_PROPERTY(PropertyInfo(Variant::INT,     "id"),         "set_id",         "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING,  "title"),      "set_title",      "get_title");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "canvas_pos"), "set_canvas_pos", "get_canvas_pos");

	ClassDB::bind_method(D_METHOD("set_spawn_modules",  "m"), &GVSEmitterNode::set_spawn_modules);
	ClassDB::bind_method(D_METHOD("get_spawn_modules"),       &GVSEmitterNode::get_spawn_modules);
	ClassDB::bind_method(D_METHOD("add_spawn_module",   "m"), &GVSEmitterNode::add_spawn_module);
	ClassDB::bind_method(D_METHOD("set_update_modules", "m"), &GVSEmitterNode::set_update_modules);
	ClassDB::bind_method(D_METHOD("get_update_modules"),      &GVSEmitterNode::get_update_modules);
	ClassDB::bind_method(D_METHOD("add_update_module",  "m"), &GVSEmitterNode::add_update_module);
	ClassDB::bind_method(D_METHOD("set_render_modules", "m"), &GVSEmitterNode::set_render_modules);
	ClassDB::bind_method(D_METHOD("get_render_modules"),      &GVSEmitterNode::get_render_modules);
	ClassDB::bind_method(D_METHOD("add_render_module",  "m"), &GVSEmitterNode::add_render_module);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "spawn_modules",  PROPERTY_HINT_ARRAY_TYPE, "GVSModule"), "set_spawn_modules",  "get_spawn_modules");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "update_modules", PROPERTY_HINT_ARRAY_TYPE, "GVSModule"), "set_update_modules", "get_update_modules");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "render_modules", PROPERTY_HINT_ARRAY_TYPE, "GVSModule"), "set_render_modules", "get_render_modules");
}

void GVSEmitterNode::ensure_id_counter(int p_id) {
	if (p_id > _node_id_counter) {
		_node_id_counter = p_id;
	}
}

GPUParticles3D *GVSEmitterNode::create_gpu_particles() const {
	GPUParticles3D *gpu = memnew(GPUParticles3D);
	// Neutral defaults — modules are the sole controllers.
	// emitting=false + amount=1 (Godot minimum) means no visible particles without a SpawnRate module.
	gpu->set_amount(1);
	gpu->set_lifetime(1.0);
	gpu->set_speed_scale(1.0);
	gpu->set_explosiveness_ratio(0.0);
	gpu->set_randomness_ratio(0.0);
	gpu->set_emitting(false);

	Ref<ParticleProcessMaterial> mat;
	mat.instantiate();
	gpu->set_process_material(mat);

	Ref<QuadMesh> quad;
	quad.instantiate();
	quad->set_size(Vector2(0.1f, 0.1f));
	gpu->set_draw_passes(1);
	gpu->set_draw_pass_mesh(0, quad);

	// Render modules run FIRST so they can replace the process material.
	// Spawn/update modules then modify whatever process material is active.
	auto apply_array = [&](const TypedArray<GVSModule> &arr, ParticleProcessMaterial *p_active_mat) {
		for (int i = 0; i < arr.size(); i++) {
			Ref<GVSModule> mod = arr[i];
			if (mod.is_valid() && mod->get_enabled()) {
				mod->apply_to_particles(gpu, p_active_mat);
			}
		}
	};

	apply_array(render_modules, mat.ptr());

	// After render modules may have replaced the process material, resolve the active one.
	Ref<ParticleProcessMaterial> active_mat = gpu->get_process_material();
	ParticleProcessMaterial *active_ptr = active_mat.is_valid() ? active_mat.ptr() : mat.ptr();

	apply_array(spawn_modules,  active_ptr);
	apply_array(update_modules, active_ptr);

	return gpu;
}

Ref<GVSEmitterNode> GVSEmitterNode::create(const String &p_title, Vector2 p_pos) {
	Ref<GVSEmitterNode> node;
	node.instantiate();
	node->id         = ++_node_id_counter;
	node->title      = p_title;
	node->canvas_pos = p_pos;
	return node;
}

}
