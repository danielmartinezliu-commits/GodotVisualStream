#include "gvs_particle_preview.h"

#include "scene/gui/box_container.h"
#include "scene/resources/style_box_flat.h"

class VBoxContainer;

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

	viewport_container = memnew(SubViewportContainer);
	viewport_container->set_stretch(true);
	viewport_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	viewport_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tab_root->add_child(viewport_container);

	viewport = memnew(SubViewport);
	viewport->set_transparent_background(false);
	viewport_container->add_child(viewport);

	scene_root = memnew(Node3D);
	viewport->add_child(scene_root);

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
	camera->set_position(Vector3(0.0f, 1.5f, 4.0f));
	camera->set_rotation_degrees(Vector3(-15.0f, 0.0f, 0.0f));
	scene_root->add_child(camera);
}

void GVSParticlePreview::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
}

GVSParticlePreview::GVSParticlePreview() {
	set_custom_minimum_size(Size2(200, 200));
	set_theme_type_variation("Tree");
}

}
