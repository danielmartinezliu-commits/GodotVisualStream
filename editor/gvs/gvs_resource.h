#pragma once

#include "core/io/resource.h"
#include "core/variant/typed_array.h"
#include "editor/gvs/nodes/gvs_emitter_node.h"

namespace GodotVisualStream {

class GVSResource : public Resource {
	GDCLASS(GVSResource, Resource);

	int version = 1;
	TypedArray<GVSEmitterNode> nodes;

protected:
	static void _bind_methods();

public:
	void set_version(int p_version);
	int  get_version() const;

	void                       set_nodes(const TypedArray<GVSEmitterNode> &p_nodes);
	TypedArray<GVSEmitterNode> get_nodes() const;

	GVSResource() = default;
};

}
