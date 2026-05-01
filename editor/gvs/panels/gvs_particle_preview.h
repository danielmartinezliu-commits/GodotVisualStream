#pragma once

#include "core/input/input_event.h"
#include "core/math/math_funcs.h"
#include "scene/gui/button.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/tab_container.h"
#include "scene/gui/subviewport_container.h"
#include "scene/main/viewport.h"
#include "scene/3d/node_3d.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/world_environment.h"
#include "scene/resources/immediate_mesh.h"
#include "scene/resources/material.h"
#include "editor/gvs/gvs_resource.h"

namespace GodotVisualStream {

class GVSParticlePreview : public PanelContainer {
	GDCLASS(GVSParticlePreview, PanelContainer);

	Ref<GVSResource> current_resource;

	SubViewportContainer *viewport_container = nullptr;
	SubViewport *viewport = nullptr;
	Node3D *scene_root    = nullptr;
	Node3D *particle_root = nullptr;
	Camera3D *camera = nullptr;
	MeshInstance3D *grid_instance = nullptr;

	// Camera orbit
	float orbit_yaw   = 0.0f;
	float orbit_pitch = 0.35f;
	float orbit_dist  = 4.5f;
	bool orbiting = false;
	Vector2 orbit_last_mouse;

	// Camera FOV / zoom
	static constexpr float FOV_MIN = 15.0f;
	static constexpr float FOV_MAX = 120.0f;
	float fov = 75.0f;

	void _build_ui();
	void _build_viewport();
	void _build_scene_objects();
	void _build_grid();
	void _update_camera();
	void _rebuild_particles();
	void _on_grid_toggled(bool p_pressed);

protected:
	static void _bind_methods();
	void _notification(int p_what);
	virtual void gui_input(const Ref<InputEvent> &p_event) override;

public:
	void load_resource(const Ref<GVSResource> &p_resource);
	void refresh();

	Node3D *get_scene_root() const { return scene_root; }

	GVSParticlePreview();
};

}
