#pragma once

#include "scene/gui/control.h"
#include "scene/gui/subviewport_container.h"
#include "scene/main/viewport.h"
#include "scene/3d/node_3d.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/world_environment.h"
#include "editor/gvs/gvs_resource.h"

namespace GodotVisualStream {

class GVSParticlePreview : public Control {
	GDCLASS(GVSParticlePreview, Control);

	Ref<GVSResource> current_resource;

	SubViewportContainer *viewport_container = nullptr;
	SubViewport          *viewport           = nullptr;
	Node3D               *scene_root         = nullptr;
	Camera3D             *camera             = nullptr;

	void _build_ui();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void load_resource(const Ref<GVSResource> &p_resource);

	// Devuelve el nodo raíz 3D para que a futuro se puedan añadir partículas
	Node3D *get_scene_root() const { return scene_root; }

	GVSParticlePreview();
};

}