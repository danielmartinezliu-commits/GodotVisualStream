#include "gvs_emitter_node.h"

#include "core/object/class_db.h"
#include "scene/3d/gpu_particles_3d.h"

namespace GodotVisualStream {

// Contador global de IDs; se incrementa con cada nodo creado en runtime
static int _node_id_counter = 0;

// Expone id, title y canvas_pos al sistema de propiedades de Godot (inspector y serialización)
void GVSEmitterNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_id", "id"), &GVSEmitterNode::set_id);
	ClassDB::bind_method(D_METHOD("get_id"), &GVSEmitterNode::get_id);
	ClassDB::bind_method(D_METHOD("set_title", "title"), &GVSEmitterNode::set_title);
	ClassDB::bind_method(D_METHOD("get_title"), &GVSEmitterNode::get_title);
	ClassDB::bind_method(D_METHOD("set_canvas_pos", "pos"), &GVSEmitterNode::set_canvas_pos);
	ClassDB::bind_method(D_METHOD("get_canvas_pos"), &GVSEmitterNode::get_canvas_pos);
	ClassDB::bind_method(D_METHOD("set_lifetime", "lifetime"), &GVSEmitterNode::set_lifetime);
	ClassDB::bind_method(D_METHOD("get_lifetime"), &GVSEmitterNode::get_lifetime);
	ClassDB::bind_method(D_METHOD("set_lifetime_var", "var"), &GVSEmitterNode::set_lifetime_var);
	ClassDB::bind_method(D_METHOD("get_lifetime_var"), &GVSEmitterNode::get_lifetime_var);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "id"), "set_id", "get_id");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "title"), "set_title", "get_title");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "canvas_pos"), "set_canvas_pos", "get_canvas_pos");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lifetime"), "set_lifetime", "get_lifetime");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "lifetime_var"), "set_lifetime_var", "get_lifetime_var");
}

// Avanza el contador al ID más alto conocido al cargar nodos desde disco, evitando colisiones
void GVSEmitterNode::ensure_id_counter(int p_id) {
	if (p_id > _node_id_counter) {
		_node_id_counter = p_id;
	}
}

void GVSEmitterNode::recreate_particle_gpu() {
	GPUParticles3D *gpu_particles = memnew(GPUParticles3D);
	Ref<GPUParticles3D> gpu_instance;
	gpu_instance.instantiate();
	gpu_instance->set_lifetime(10.0f);
}

// Factory: crea un nodo con ID único autoincremental, título y posición en canvas
Ref<GVSEmitterNode> GVSEmitterNode::create(const String &p_title, Vector2 p_pos) {
	Ref<GVSEmitterNode> node;
	node.instantiate();
	node->id = ++_node_id_counter;
	node->title = p_title;
	node->canvas_pos = p_pos;
	return node;
}

}
