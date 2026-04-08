#include "gvs_editor_window.h"

namespace GodotVisualStream {

void GVSEditorWindow::_bind_methods() {
}

void GVSEditorWindow::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		_build_ui();
	}
}

void GVSEditorWindow::_build_ui() {
	if (main_split) {
		return;
	}

	VBoxContainer *root = memnew(VBoxContainer);
	root->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(root);

	// Split principal: izquierda | (centro + derecha)
	main_split = memnew(HSplitContainer);
	main_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	root->add_child(main_split);

	// Columna izquierda: preview arriba, variables abajo
	left_split = memnew(VSplitContainer);
	left_split->set_custom_minimum_size(Size2(220, 0));
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
	main_split->add_child(center_right_split);

	// Columna central: emitters arriba (amplio), timeline abajo
	center_split = memnew(VSplitContainer);
	center_split->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	center_right_split->add_child(center_split);

	emitters_panel = memnew(GVSEmittersPanel);
	emitters_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	emitters_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->add_child(emitters_panel);

	timeline_panel = memnew(GVSTimelinePanel);
	timeline_panel->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->add_child(timeline_panel);

	// Columna derecha: inspector (altura completa)
	inspector_panel = memnew(GVSInspectorPanel);
	inspector_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	center_right_split->add_child(inspector_panel);
}

void GVSEditorWindow::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
	if (current_resource.is_valid()) {
		set_title("GVS Editor — " + current_resource->get_path().get_file());
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
	set_ok_button_text("Cerrar");
	set_min_size(Size2(1200, 700));
}

} // namespace GodotVisualStream