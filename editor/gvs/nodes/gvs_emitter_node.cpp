#include "gvs_emitter_node.h"

#include "core/object/class_db.h"

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
}

void GVSEmitterNode::ensure_id_counter(int p_id) {
	if (p_id > _node_id_counter) {
		_node_id_counter = p_id;
	}
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
