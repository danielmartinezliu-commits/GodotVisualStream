#include "gvs_emitter_node.h"

namespace GodotVisualStream {

void GVSEmitterNode::_bind_methods() {
}

Ref<GVSEmitterNode> GVSEmitterNode::create(const String &p_title, Vector2 p_pos) {
	Ref<GVSEmitterNode> node;
	node.instantiate();
	node->title      = p_title;
	node->canvas_pos = p_pos;
	return node;
}

}
