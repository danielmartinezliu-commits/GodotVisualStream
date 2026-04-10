#include "gvs_editor_window.h"

#include "core/object/callable_mp.h"
#include "scene/gui/margin_container.h"

namespace GodotVisualStream {

void GVSEditorWindow::_bind_methods() {
}

void GVSEditorWindow::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		_build_ui();

		Ref<StyleBoxFlat> dialog_style = get_theme_stylebox(SNAME("panel"));
		Ref<StyleBoxFlat> dark_ref = get_theme_stylebox(SNAME("panel"), SNAME("Panel"));
		if (dialog_style.is_valid() && dark_ref.is_valid()) {
			Ref<StyleBoxFlat> custom = dialog_style->duplicate();
			custom->set_bg_color(dark_ref->get_bg_color());
			custom->set_content_margin_all(0.0f);
			add_theme_style_override(SNAME("panel"), custom);
		}
	}
}

void GVSEditorWindow::_build_ui() {
	if (main_split) {
		return;
	}

	VBoxContainer *root = memnew(VBoxContainer);
	root->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(root);

	// Helper para aplicar el estilo mínimo a todos los splits:
	// separation=4 (línea fina), autohide=1 (grabber solo al hover).
	auto setup_split = [](SplitContainer *s) {
		s->add_theme_constant_override("separation", 1);
		s->add_theme_constant_override("autohide", 1);
	};

	// Split principal: izquierda | (centro + derecha)
	main_split = memnew(HSplitContainer);
	main_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	setup_split(main_split);
	root->add_child(main_split);

	// Columna izquierda: preview arriba, variables abajo
	left_split = memnew(VSplitContainer);
	setup_split(left_split);
	main_split->add_child(left_split);

	particle_preview = memnew(GVSParticlePreview);
	particle_preview->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_split->add_child(particle_preview);

	variables_panel = memnew(GVSVariablesPanel);
	variables_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_split->add_child(variables_panel);

	// Split secundario: centro | derecha (inspector)
	center_right_split = memnew(HSplitContainer);
	center_right_split->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	center_right_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	setup_split(center_right_split);
	main_split->add_child(center_right_split);

	// Columna central: emitters arriba (amplio), timeline abajo
	center_split = memnew(VSplitContainer);
	center_split->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	setup_split(center_split);
	center_right_split->add_child(center_split);

	MarginContainer *emitters_margin = memnew(MarginContainer);
	emitters_margin->add_theme_constant_override("margin_left",   5);
	emitters_margin->add_theme_constant_override("margin_right",  5);
	emitters_margin->add_theme_constant_override("margin_top",    5);
	emitters_margin->add_theme_constant_override("margin_bottom", 5);
	emitters_margin->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	emitters_margin->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->add_child(emitters_margin);

	emitters_panel = memnew(GVSEmittersPanel);
	emitters_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	emitters_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	emitters_margin->add_child(emitters_panel);

	timeline_panel = memnew(GVSTimelinePanel);
	timeline_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->add_child(timeline_panel);

	inspector_panel = memnew(GVSInspectorPanel);
	inspector_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	center_right_split->add_child(inspector_panel);

	emitters_panel->connect("node_selected",
			callable_mp(inspector_panel, &GVSInspectorPanel::inspect_node));

	inspector_panel->connect("node_changed",
			callable_mp((CanvasItem *)emitters_panel, &CanvasItem::queue_redraw));
}

void GVSEditorWindow::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
	if (current_resource.is_valid()) {
		set_title("GVS Editor - " + current_resource->get_path().get_file());
	}
	if (particle_preview) {
		particle_preview->load_resource(current_resource);
	}
	if (variables_panel) {
		variables_panel->load_resource(current_resource);
	}
	if (emitters_panel) {
		emitters_panel->load_resource(current_resource);
	}
	if (timeline_panel) {
		timeline_panel->load_resource(current_resource);
	}
	if (inspector_panel) {
		inspector_panel->load_resource(current_resource);
	}
}

Ref<GVSResource> GVSEditorWindow::get_current_resource() const {
	return current_resource;
}

GVSEditorWindow::GVSEditorWindow() {
	set_title("GVS Editor");
	set_exclusive(false);
	set_ok_button_text("Close");
	set_min_size(Size2(1280, 720));
}

}
