#include "gvs_particle_preview.h"

#include "scene/gui/box_container.h"
#include "scene/resources/style_box_flat.h"

namespace GodotVisualStream {

void GVSParticlePreview::_bind_methods() {
}

void GVSParticlePreview::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		_build_ui();
		Ref<StyleBoxFlat> base = get_theme_stylebox(SNAME("panel"), SNAME("Tree"));
		if (base.is_valid()) {
			Ref<StyleBoxFlat> bordered = base->duplicate();
			bordered->set_border_width_all(1);
			bordered->set_border_color(base->get_bg_color().lightened(0.35f));
			add_theme_style_override(SNAME("panel"), bordered);
		}
	}
}

// Crea el SubViewportContainer, el SubViewport y el Node3D raíz de la escena 3D
void GVSParticlePreview::_build_viewport() {
	viewport_container = memnew(SubViewportContainer);
	viewport_container->set_stretch(true);
	viewport_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	viewport_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	viewport_container->set_mouse_filter(Control::MOUSE_FILTER_PASS);

	viewport = memnew(SubViewport);
	viewport->set_transparent_background(false);
	viewport->set_use_own_world_3d(true);
	viewport_container->add_child(viewport);

	scene_root = memnew(Node3D);
	viewport->add_child(scene_root);
}

// Escena 3D: entorno, luz, cámara orbital, suelo y esfera de referencia
void GVSParticlePreview::_build_scene_objects() {
	WorldEnvironment *world_env = memnew(WorldEnvironment);
	Ref<Environment> env;
	env.instantiate();
	env->set_background(Environment::BG_COLOR);
	env->set_bg_color(Color(0.08f, 0.08f, 0.08f));
	env->set_ambient_source(Environment::AMBIENT_SOURCE_COLOR);
	env->set_ambient_light_color(Color(0.2f, 0.2f, 0.2f));
	world_env->set_environment(env);
	scene_root->add_child(world_env);

	DirectionalLight3D *light = memnew(DirectionalLight3D);
	light->set_rotation_degrees(Vector3(-45.0f, 45.0f, 0.0f));
	scene_root->add_child(light);

	camera = memnew(Camera3D);
	scene_root->add_child(camera);
	_update_camera();

	// Plano
	MeshInstance3D *plane_instance = memnew(MeshInstance3D);
	Ref<PlaneMesh> plane_mesh;
	plane_mesh.instantiate();
	plane_mesh->set_size(Vector2(4.0f, 4.0f));
	plane_instance->set_mesh(plane_mesh);
	scene_root->add_child(plane_instance);

	// Esfera
	MeshInstance3D *sphere_instance = memnew(MeshInstance3D);
	Ref<SphereMesh> sphere_mesh;
	sphere_mesh.instantiate();
	sphere_mesh->set_radius(0.5f);
	sphere_mesh->set_height(1.0f);
	sphere_instance->set_mesh(sphere_mesh);
	sphere_instance->set_position(Vector3(0.0f, 0.5f, 0.0f));
	scene_root->add_child(sphere_instance);
}

void GVSParticlePreview::_build_ui() {
	if (viewport_container) {
		return;
	}

	TabContainer *tabs = memnew(TabContainer);
	tabs->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tabs->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(tabs);

	VBoxContainer *tab_root = memnew(VBoxContainer);
	tab_root->set_name("Preview");
	tab_root->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tab_root->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tabs->add_child(tab_root);

	_build_viewport();
	tab_root->add_child(viewport_container);

	_build_scene_objects();
}

void GVSParticlePreview::_update_camera() {
	if (!camera) {
		return;
	}
	const float x = orbit_dist * Math::sin(orbit_yaw) * Math::cos(orbit_pitch);
	const float y = orbit_dist * Math::sin(orbit_pitch);
	const float z = orbit_dist * Math::cos(orbit_yaw) * Math::cos(orbit_pitch);
	camera->set_position(Vector3(x, y, z));
	camera->look_at(Vector3(0.0f, 0.5f, 0.0f));
	camera->set_fov(fov);
}

void GVSParticlePreview::gui_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid() && mb->get_button_index() == MouseButton::LEFT) {
		orbiting = mb->is_pressed();
		if (orbiting) {
			orbit_last_mouse = mb->get_position();
		}
		accept_event();
		return;
	}

	// TODO : Poder hacer zoom out y zoom in en la preview
	if (mb.is_valid() && mb->get_button_index() == MouseButton::WHEEL_UP && fov <= ZOOM_MAX) {
		fov += 15.0f;
		accept_event();
	}

	if (mb.is_valid() && mb->get_button_index() == MouseButton::WHEEL_UP && fov >= ZOOM_MIN) {
		fov -= 15.0f;
		accept_event();
	}

	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_valid() && orbiting) {
		const Vector2 delta = mm->get_position() - orbit_last_mouse;
		orbit_last_mouse = mm->get_position();

		orbit_yaw -= delta.x * 0.01f;
		orbit_pitch += delta.y * 0.01f;
		orbit_pitch = CLAMP(orbit_pitch, Math::deg_to_rad(-80.0f), Math::deg_to_rad(80.0f));

		_update_camera();
		accept_event();
	}
}

void GVSParticlePreview::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
}

GVSParticlePreview::GVSParticlePreview() {
	set_custom_minimum_size(Size2(200, 200));
	set_theme_type_variation("Tree");
	set_mouse_filter(Control::MOUSE_FILTER_STOP);
}

}
