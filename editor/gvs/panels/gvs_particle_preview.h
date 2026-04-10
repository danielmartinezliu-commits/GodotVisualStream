#pragma once

#include "core/input/input_event.h"
#include "core/math/math_funcs.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/subviewport_container.h"
#include "scene/main/viewport.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/node_3d.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/world_environment.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "editor/gvs/gvs_resource.h"

namespace GodotVisualStream {

class GVSParticlePreview : public PanelContainer {
	GDCLASS(GVSParticlePreview, PanelContainer);

	Ref<GVSResource> current_resource;

	SubViewportContainer *viewport_container = nullptr;
	SubViewport          *viewport           = nullptr;
	Node3D               *scene_root         = nullptr;
	Camera3D             *camera             = nullptr;

	float   orbit_yaw   = 0.0f;
	float   orbit_pitch = 0.35f;
	float   orbit_dist  = 4.5f;
	bool    orbiting    = false;
	Vector2 orbit_last_mouse;

	void _build_ui();
	void _update_camera();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	virtual void gui_input(const Ref<InputEvent> &p_event) override;

public:
	void load_resource(const Ref<GVSResource> &p_resource);

	Node3D *get_scene_root() const { return scene_root; }

	GVSParticlePreview();
};

}
