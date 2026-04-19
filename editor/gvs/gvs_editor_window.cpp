#include "gvs_editor_window.h"

#include "core/io/resource_saver.h"
#include "core/object/callable_mp.h"
#include "editor/editor_data.h"
#include "editor/editor_interface.h"
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

// Barra superior con acciones globales del editor
void GVSEditorWindow::_build_toolbar(VBoxContainer *p_root) {
	HBoxContainer *toolbar = memnew(HBoxContainer);
	toolbar->add_theme_constant_override("separation", 4);
	p_root->add_child(toolbar);

	Button *btn_add_to_scene = memnew(Button);
	btn_add_to_scene->set_text("Add to Scene");
	btn_add_to_scene->connect("pressed", callable_mp(this, &GVSEditorWindow::_on_add_to_scene_pressed));
	toolbar->add_child(btn_add_to_scene);
}

// Panel izquierdo: preview 3D del sistema de partículas encima del panel de variables
VSplitContainer *GVSEditorWindow::_build_left_panel() {
	left_split = memnew(VSplitContainer);
	left_split->add_theme_constant_override("separation", 1);
	left_split->add_theme_constant_override("autohide", 1);

	particle_preview = memnew(GVSParticlePreview);
	particle_preview->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_split->add_child(particle_preview);

	variables_panel = memnew(GVSVariablesPanel);
	variables_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	left_split->add_child(variables_panel);

	return left_split;
}

// Panel central: lista de emisores (con margen) encima de la timeline
VSplitContainer *GVSEditorWindow::_build_center_panel() {
	center_split = memnew(VSplitContainer);
	center_split->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	center_split->add_theme_constant_override("separation", 1);
	center_split->add_theme_constant_override("autohide", 1);

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

	return center_split;
}

// Panel derecho: inspector de propiedades del nodo seleccionado
GVSInspectorPanel *GVSEditorWindow::_build_right_panel() {
	inspector_panel = memnew(GVSInspectorPanel);
	inspector_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	return inspector_panel;
}

// Conexiones entre paneles: selección de emisor → inspector, cambio en inspector → redibujado y guardado
void GVSEditorWindow::_connect_panel_signals() {
	emitters_panel->connect("node_selected",
			callable_mp(inspector_panel, &GVSInspectorPanel::inspect_node));

	inspector_panel->connect("node_changed",
			callable_mp((CanvasItem *)emitters_panel, &CanvasItem::queue_redraw));
	inspector_panel->connect("node_changed",
			callable_mp(this, &GVSEditorWindow::_on_graph_changed));

	variables_panel->connect("variable_changed",
			callable_mp(this, &GVSEditorWindow::_on_graph_changed));
}

void GVSEditorWindow::_build_ui() {
	if (main_split) {
		return;
	}

	VBoxContainer *root = memnew(VBoxContainer);
	root->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(root);

	_build_toolbar(root);

	// Split principal horizontal: izquierda | centro+derecha
	main_split = memnew(HSplitContainer);
	main_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	main_split->add_theme_constant_override("separation", 1);
	main_split->add_theme_constant_override("autohide", 1);
	root->add_child(main_split);

	main_split->add_child(_build_left_panel());

	// Split horizontal secundario: centro | derecha
	center_right_split = memnew(HSplitContainer);
	center_right_split->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	center_right_split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	center_right_split->add_theme_constant_override("separation", 1);
	center_right_split->add_theme_constant_override("autohide", 1);
	main_split->add_child(center_right_split);

	center_right_split->add_child(_build_center_panel());
	center_right_split->add_child(_build_right_panel());

	_connect_panel_signals();
}

void GVSEditorWindow::_on_add_to_scene_pressed() {
	ERR_FAIL_COND_MSG(!current_resource.is_valid(), "GVS: No resource loaded.");

	Node *scene_root = EditorInterface::get_singleton()->get_edited_scene_root();
	ERR_FAIL_NULL_MSG(scene_root, "GVS: No scene is currently open.");

	GVSParticleSystem *ps = memnew(GVSParticleSystem);
	ps->set_name("GVSParticleSystem");
	ps->set_gvs_resource(current_resource);
	scene_root->add_child(ps, true);
	ps->set_owner(scene_root);

	EditorInterface::get_singleton()->get_selection()->clear();
	EditorInterface::get_singleton()->get_selection()->add_node(ps);
}

void GVSEditorWindow::_on_graph_changed() {
	if (current_resource.is_valid() && !current_resource->get_path().is_empty()) {
		ResourceSaver::save(current_resource, current_resource->get_path());
	}
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
