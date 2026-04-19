#include "gvs_particle_system.h"

#include "core/object/class_db.h"

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

// Destruye los hijos internos actuales y reconstruye la jerarquía de nodos a partir del recurso
void GVSParticleSystem::_rebuild() {
	// Limpia los hijos internos del frame anterior
	for (int i = get_child_count(true) - 1; i >= 0; i--) {
		Node* child = get_child(i, true);
		remove_child(child);
		child->queue_free();
	}

	if (!resource.is_valid()) {
		return;
	}

	// TODO: De momento esto es falso, hay que hacerlo de verdad para que funcione con lo que se instancia en el preview
	MeshInstance3D *plane = memnew(MeshInstance3D);
	Ref<PlaneMesh> plane_mesh;
	plane_mesh.instantiate();
	plane_mesh->set_size(Vector2(4.0f, 4.0f));
	plane->set_mesh(plane_mesh);
	add_child(plane, false, Node::INTERNAL_MODE_BACK);

	MeshInstance3D *sphere = memnew(MeshInstance3D);
	Ref<SphereMesh> sphere_mesh;
	sphere_mesh.instantiate();
	sphere_mesh->set_radius(0.5f);
	sphere_mesh->set_height(1.0f);
	sphere->set_position(Vector3(0.0f, 0.5f, 0.0f));
	sphere->set_mesh(sphere_mesh);
	add_child(sphere, false, Node::INTERNAL_MODE_BACK);
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
