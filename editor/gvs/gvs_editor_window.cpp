#include "gvs_editor_window.h"

#include "core/io/resource_saver.h"
#include "core/object/callable_mp.h"
#include "editor/editor_data.h"
#include "editor/editor_interface.h"
#include "editor/gvs/modules/gvs_module.h"
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
	emitters_panel->connect("node_changed",
			callable_mp(particle_preview, &GVSParticlePreview::refresh));
	emitters_panel->connect("node_changed",
			callable_mp(this, &GVSEditorWindow::_on_graph_changed));

	inspector_panel->connect("node_changed",
			callable_mp((CanvasItem *)emitters_panel, &CanvasItem::queue_redraw));
	inspector_panel->connect("node_changed",
			callable_mp(this, &GVSEditorWindow::_on_graph_changed));
	inspector_panel->connect("node_changed",
			callable_mp(particle_preview, &GVSParticlePreview::refresh));

	variables_panel->connect("variable_changed",
			callable_mp(this, &GVSEditorWindow::_on_variable_changed));
	variables_panel->connect("variable_renamed",
			callable_mp(this, &GVSEditorWindow::_on_variable_renamed));
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

void GVSEditorWindow::_sync_variable_bindings() {
	if (!current_resource.is_valid()) { return; }

	TypedArray<GVSVariable> vars   = current_resource->get_variables();
	TypedArray<GVSEmitterNode> nodes = current_resource->get_nodes();

	for (int ni = 0; ni < nodes.size(); ni++) {
		Ref<GVSEmitterNode> node = nodes[ni];
		if (!node.is_valid()) { continue; }

		const TypedArray<GVSModule> *lists[3] = { nullptr, nullptr, nullptr };
		TypedArray<GVSModule> spawn  = node->get_spawn_modules();
		TypedArray<GVSModule> update = node->get_update_modules();
		TypedArray<GVSModule> render = node->get_render_modules();
		lists[0] = &spawn; lists[1] = &update; lists[2] = &render;

		for (int li = 0; li < 3; li++) {
			for (int mi = 0; mi < lists[li]->size(); mi++) {
				Ref<GVSModule> mod = (*lists[li])[mi];
				if (!mod.is_valid()) { continue; }

				Vector<GVSModule::InspectorProp> props = mod->get_inspector_props();
				for (int pi = 0; pi < props.size(); pi++) {
					const GVSModule::InspectorProp &prop = props[pi];
					if (prop.bound_var.is_empty()) { continue; }

					for (int vi = 0; vi < vars.size(); vi++) {
						Ref<GVSVariable> var = vars[vi];
						if (!var.is_valid() || var->get_var_name() != prop.bound_var) { continue; }

						if (prop.is_vector3) {
							mod->set_prop_vector3(prop.prop_name, Vector3(var->get_default_value()));
						} else if (!prop.is_bool && !prop.is_color && !prop.is_enum && !prop.is_resource) {
							mod->set_prop_value(prop.prop_name, double(var->get_default_value()));
						}
						break;
					}
				}
			}
		}
	}
}

void GVSEditorWindow::_on_variable_changed() {
	_sync_variable_bindings();
	if (particle_preview) { particle_preview->refresh(); }
	if (inspector_panel)  { inspector_panel->refresh(); }
	_on_graph_changed();
}

void GVSEditorWindow::_on_variable_renamed(const String &p_old_name, const String &p_new_name) {
	if (!current_resource.is_valid() || p_old_name.is_empty() || p_new_name.is_empty()) { return; }

	TypedArray<GVSEmitterNode> nodes = current_resource->get_nodes();
	for (int ni = 0; ni < nodes.size(); ni++) {
		Ref<GVSEmitterNode> node = nodes[ni];
		if (!node.is_valid()) { continue; }

		TypedArray<GVSModule> spawn  = node->get_spawn_modules();
		TypedArray<GVSModule> update = node->get_update_modules();
		TypedArray<GVSModule> render = node->get_render_modules();
		const TypedArray<GVSModule> *lists[3] = { &spawn, &update, &render };

		for (int li = 0; li < 3; li++) {
			for (int mi = 0; mi < lists[li]->size(); mi++) {
				Ref<GVSModule> mod = (*lists[li])[mi];
				if (!mod.is_valid()) { continue; }

				Vector<GVSModule::InspectorProp> props = mod->get_inspector_props();
				for (int pi = 0; pi < props.size(); pi++) {
					if (props[pi].bound_var == p_old_name) {
						mod->set_prop_binding(props[pi].prop_name, p_new_name);
					}
				}
			}
		}
	}

	_on_graph_changed();
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
	get_ok_button()->hide();
	set_min_size(Size2(1600, 900));
}

}
