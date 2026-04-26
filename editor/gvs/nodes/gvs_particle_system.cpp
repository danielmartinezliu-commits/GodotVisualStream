#include "gvs_particle_system.h"

#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"
#include "editor/gvs/nodes/gvs_emitter_node.h"

namespace GodotVisualStream {

// Expone gvs_resource al inspector para poder asignar el recurso .gvs desde el editor
void GVSParticleSystem::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_gvs_resource", "resource"), &GVSParticleSystem::set_gvs_resource);
	ClassDB::bind_method(D_METHOD("get_gvs_resource"), &GVSParticleSystem::get_gvs_resource);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "gvs_resource", PROPERTY_HINT_RESOURCE_TYPE, "GVSResource"),
			"set_gvs_resource", "get_gvs_resource");
}

void GVSParticleSystem::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		_rebuild();
	}
}

// Destruye los hijos internos actuales y reconstruye un GPUParticles3D por cada emitter del recurso
void GVSParticleSystem::_rebuild() {
	for (int i = get_child_count(true) - 1; i >= 0; i--) {
		Node *child = get_child(i, true);
		remove_child(child);
		child->queue_free();
	}

	if (!resource.is_valid()) {
		return;
	}

	TypedArray<GVSEmitterNode> nodes = resource->get_nodes();
	for (int i = 0; i < nodes.size(); i++) {
		Ref<GVSEmitterNode> emitter = nodes[i];
		if (!emitter.is_valid()) {
			continue;
		}
		GPUParticles3D *gpu = emitter->create_gpu_particles();
		gpu->set_name(emitter->get_title());
		add_child(gpu, false, Node::INTERNAL_MODE_BACK);
	}
}

// Asigna el recurso y fuerza un rebuild solo si el nodo ya está en el árbol de escena
void GVSParticleSystem::set_gvs_resource(const Ref<GVSResource> &p_resource) {
	resource = p_resource;
	if (is_inside_tree()) {
		_rebuild();
	}
}

Ref<GVSResource> GVSParticleSystem::get_gvs_resource() const {
	return resource;
}

}
