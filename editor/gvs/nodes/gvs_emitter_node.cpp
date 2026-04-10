#include "gvs_emitter_node.h"

namespace GodotVisualStream {

static int _node_id_counter = 0;

void GVSEmitterNode::_bind_methods() {
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
