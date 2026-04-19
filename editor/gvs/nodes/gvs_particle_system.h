#pragma once

#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/node_3d.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "editor/gvs/gvs_resource.h"

namespace GodotVisualStream {

class GVSParticleSystem : public Node3D {
	GDCLASS(GVSParticleSystem, Node3D);

	Ref<GVSResource> resource;

	void _rebuild();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void set_gvs_resource(const Ref<GVSResource> &p_resource);
	Ref<GVSResource> get_gvs_resource() const;

	GVSParticleSystem() = default;
};

}
