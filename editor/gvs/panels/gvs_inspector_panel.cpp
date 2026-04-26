#include "gvs_inspector_panel.h"

#include "core/object/callable_mp.h"
#include "core/object/class_db.h"
#include "scene/resources/style_box_flat.h"
#include "editor/gvs/modules/gvs_module_spawn_rate.h"
#include "editor/gvs/modules/gvs_module_initial_velocity.h"
#include "editor/gvs/modules/gvs_module_lifetime.h"

namespace GodotVisualStream {

// =============================================================================
// GVSPropertyDropZone
// =============================================================================

void GVSPropertyDropZone::_bind_methods() {
	ADD_SIGNAL(MethodInfo("value_changed",   PropertyInfo(Variant::FLOAT,  "value")));
	ADD_SIGNAL(MethodInfo("binding_changed", PropertyInfo(Variant::STRING, "var_name")));

	ClassDB::bind_method(D_METHOD("_can_drop_fwd", "pos", "data"), &GVSPropertyDropZone::_can_drop_fwd);
	ClassDB::bind_method(D_METHOD("_drop_fwd",     "pos", "data"), &GVSPropertyDropZone::_drop_fwd);
}

void GVSPropertyDropZone::setup(float p_value, const String &p_bound_var, int p_accepted_var_type,
		double p_min, double p_max, double p_step) {
	value             = p_value;
	bound_var         = p_bound_var;
	accepted_var_type = p_accepted_var_type;
	spin_min          = p_min;
	spin_max          = p_max;
	spin_step         = p_step;
	_rebuild_display();
}

void GVSPropertyDropZone::_rebuild_display() {
	for (int i = get_child_count() - 1; i >= 0; i--) {
		Node *child = get_child(i);
		remove_child(child);
		child->queue_free();
	}
	spin_box   = nullptr;
	var_badge  = nullptr;
	unbind_btn = nullptr;

	set_mouse_filter(Control::MOUSE_FILTER_STOP);

	if (bound_var.is_empty()) {
		spin_box = memnew(SpinBox);
		spin_box->set_min(spin_min);
		spin_box->set_max(spin_max);
		spin_box->set_step(spin_step);
		spin_box->set_value(value);
		spin_box->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		spin_box->connect("value_changed",
				callable_mp(this, &GVSPropertyDropZone::_on_spin_changed));
		spin_box->set_drag_forwarding(
				Callable(),
				callable_mp(this, &GVSPropertyDropZone::_can_drop_fwd),
				callable_mp(this, &GVSPropertyDropZone::_drop_fwd));
		add_child(spin_box);
	} else {
		var_badge = memnew(Label);
		var_badge->set_text(bound_var);
		var_badge->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		var_badge->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
		var_badge->add_theme_color_override("font_color", Color(0.4f, 0.8f, 1.0f));

		Ref<StyleBoxFlat> chip_style;
		chip_style.instantiate();
		chip_style->set_bg_color(Color(0.4f, 0.8f, 1.0f, 0.12f));
		chip_style->set_border_color(Color(0.4f, 0.8f, 1.0f, 0.5f));
		chip_style->set_border_width_all(1);
		chip_style->set_content_margin_all(3);
		var_badge->add_theme_style_override("normal", chip_style);
		add_child(var_badge);

		unbind_btn = memnew(Button);
		unbind_btn->set_text("x");
		unbind_btn->set_flat(true);
		unbind_btn->set_custom_minimum_size(Size2(20, 0));
		unbind_btn->connect("pressed",
				callable_mp(this, &GVSPropertyDropZone::_on_unbind_pressed));
		add_child(unbind_btn);
	}
}

void GVSPropertyDropZone::_on_spin_changed(double p_val) {
	value = float(p_val);
	emit_signal(SNAME("value_changed"), value);
}

void GVSPropertyDropZone::_on_unbind_pressed() {
	bound_var = "";
	_rebuild_display();
	emit_signal(SNAME("binding_changed"), String(""));
}

bool GVSPropertyDropZone::_can_drop_fwd(const Point2 &p_pos, const Variant &p_data) {
	return can_drop_data(p_pos, p_data);
}
void GVSPropertyDropZone::_drop_fwd(const Point2 &p_pos, const Variant &p_data) {
	drop_data(p_pos, p_data);
}

bool GVSPropertyDropZone::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
	if (p_data.get_type() != Variant::DICTIONARY) { return false; }
	Dictionary d = p_data;
	if (!d.has("type") || String(d["type"]) != "gvs_variable") { return false; }
	if (!d.has("var_type") || int(d["var_type"]) != accepted_var_type) { return false; }
	return true;
}

void GVSPropertyDropZone::drop_data(const Point2 &p_point, const Variant &p_data) {
	Dictionary d = p_data;
	bound_var = String(d["name"]);
	_rebuild_display();
	emit_signal(SNAME("binding_changed"), bound_var);
}

// =============================================================================
// GVSInspectorPanel
// =============================================================================

void GVSInspectorPanel::_bind_methods() {
	ADD_SIGNAL(MethodInfo("node_changed"));

	ClassDB::bind_method(D_METHOD("_on_module_enabled_toggled", "enabled", "mod"),
			&GVSInspectorPanel::_on_module_enabled_toggled);
	ClassDB::bind_method(D_METHOD("_on_module_value_changed", "val", "mod", "prop"),
			&GVSInspectorPanel::_on_module_value_changed);
	ClassDB::bind_method(D_METHOD("_on_module_binding_changed", "var", "mod", "prop"),
			&GVSInspectorPanel::_on_module_binding_changed);
	ClassDB::bind_method(D_METHOD("_on_module_color_changed", "color", "mod", "prop"),
			&GVSInspectorPanel::_on_module_color_changed);
	ClassDB::bind_method(D_METHOD("_on_module_bool_changed", "val", "mod", "prop"),
			&GVSInspectorPanel::_on_module_bool_changed);
	ClassDB::bind_method(D_METHOD("_on_module_resource_changed", "res", "mod", "prop"),
			&GVSInspectorPanel::_on_module_resource_changed);
}

void GVSInspectorPanel::_notification(int p_what) {
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

// Builds the small emitter identity grid (Name / ID / Position).
void GVSInspectorPanel::_build_emitter_grid(VBoxContainer *p_container) {
	MarginContainer *grid_margin = memnew(MarginContainer);
	grid_margin->add_theme_constant_override("margin_left",   12);
	grid_margin->add_theme_constant_override("margin_right",   6);
	grid_margin->add_theme_constant_override("margin_top",     4);
	grid_margin->add_theme_constant_override("margin_bottom",  4);
	p_container->add_child(grid_margin);

	GridContainer *grid = memnew(GridContainer);
	grid->set_columns(2);
	grid->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	grid->add_theme_constant_override("h_separation", 8);
	grid->add_theme_constant_override("v_separation", 6);
	grid_margin->add_child(grid);

	auto make_key = [](const String &p_text) -> Label * {
		Label *lbl = memnew(Label);
		lbl->set_text(p_text);
		lbl->set_custom_minimum_size(Size2(60, 0));
		lbl->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
		lbl->add_theme_color_override("font_color", Color(0.65f, 0.65f, 0.65f));
		return lbl;
	};

	grid->add_child(make_key("Name"));
	name_edit = memnew(LineEdit);
	name_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	name_edit->connect("text_changed", callable_mp(this, &GVSInspectorPanel::_on_name_changed));
	grid->add_child(name_edit);

	grid->add_child(make_key("ID"));
	id_label = memnew(Label);
	id_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	id_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	grid->add_child(id_label);

	grid->add_child(make_key("Position"));
	pos_label = memnew(Label);
	pos_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	pos_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	grid->add_child(pos_label);
}

void GVSInspectorPanel::_build_ui() {
	if (props_container) { return; }

	TabContainer *tabs = memnew(TabContainer);
	tabs->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tabs->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(tabs);

	ScrollContainer *scroll = memnew(ScrollContainer);
	scroll->set_name("Inspector");
	scroll->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);
	tabs->add_child(scroll);

	VBoxContainer *tab_root = memnew(VBoxContainer);
	tab_root->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->add_child(tab_root);

	empty_label = memnew(Label);
	empty_label->set_text("No node selected");
	empty_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	empty_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	empty_label->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	empty_label->add_theme_color_override("font_color", Color(0.55f, 0.55f, 0.55f));
	tab_root->add_child(empty_label);

	props_container = memnew(VBoxContainer);
	props_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tab_root->add_child(props_container);

	_build_emitter_grid(props_container);

	HSeparator *sep = memnew(HSeparator);
	props_container->add_child(sep);

	modules_container = memnew(VBoxContainer);
	modules_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	modules_container->add_theme_constant_override("separation", 0);
	props_container->add_child(modules_container);

	props_container->hide();
}

void GVSInspectorPanel::_on_name_changed(const String &p_text) {
	if (!inspected_node.is_valid()) { return; }
	inspected_node->set_title(p_text);
	emit_signal(SNAME("node_changed"));
}

void GVSInspectorPanel::_on_module_enabled_toggled(bool p_enabled, Ref<GVSModule> p_mod) {
	if (p_mod.is_valid()) {
		p_mod->set_enabled(p_enabled);
		emit_signal(SNAME("node_changed"));
	}
}

void GVSInspectorPanel::_on_module_value_changed(float p_val, Ref<GVSModule> p_mod, String p_prop) {
	if (p_mod.is_valid()) {
		p_mod->set_prop_value(p_prop, p_val);
		emit_signal(SNAME("node_changed"));
	}
}

void GVSInspectorPanel::_on_module_binding_changed(const String &p_var, Ref<GVSModule> p_mod, String p_prop) {
	if (p_mod.is_valid()) {
		p_mod->set_prop_binding(p_prop, p_var);
		emit_signal(SNAME("node_changed"));
	}
}

void GVSInspectorPanel::_on_module_color_changed(const Color &p_color, Ref<GVSModule> p_mod, String p_prop) {
	if (p_mod.is_valid()) {
		p_mod->set_prop_color(p_prop, p_color);
		emit_signal(SNAME("node_changed"));
	}
}

void GVSInspectorPanel::_on_module_bool_changed(bool p_val, Ref<GVSModule> p_mod, String p_prop) {
	if (p_mod.is_valid()) {
		p_mod->set_prop_bool(p_prop, p_val);
		emit_signal(SNAME("node_changed"));
	}
}

void GVSInspectorPanel::_on_module_resource_changed(const Ref<Resource> &p_res, Ref<GVSModule> p_mod, String p_prop) {
	if (p_mod.is_valid()) {
		p_mod->set_prop_resource(p_prop, p_res);
		emit_signal(SNAME("node_changed"));
	}
}

void GVSInspectorPanel::_rebuild_modules() {
	for (int i = modules_container->get_child_count() - 1; i >= 0; i--) {
		Node *c = modules_container->get_child(i);
		modules_container->remove_child(c);
		c->queue_free();
	}

	if (!inspected_node.is_valid()) { return; }

	struct SectionDef { const char *label; Color hdr_bg; Color hdr_text; };
	static const SectionDef defs[3] = {
		{ "PARTICLE SPAWN",  Color(0.12f, 0.20f, 0.12f), Color(0.45f, 0.85f, 0.50f) },
		{ "PARTICLE UPDATE", Color(0.11f, 0.14f, 0.22f), Color(0.45f, 0.65f, 1.00f) },
		{ "RENDER",          Color(0.18f, 0.12f, 0.20f), Color(0.80f, 0.50f, 0.85f) },
	};

	for (int s = 0; s < 3; s++) {
		const TypedArray<GVSModule> mods = (s == 0)
				? inspected_node->get_spawn_modules()
				: (s == 1 ? inspected_node->get_update_modules() : inspected_node->get_render_modules());

		// Section header
		PanelContainer *sec_hdr = memnew(PanelContainer);
		Ref<StyleBoxFlat> hdr_style;
		hdr_style.instantiate();
		hdr_style->set_bg_color(defs[s].hdr_bg);
		hdr_style->set_content_margin(SIDE_LEFT, 8);
		hdr_style->set_content_margin(SIDE_RIGHT, 6);
		hdr_style->set_content_margin(SIDE_TOP, 3);
		hdr_style->set_content_margin(SIDE_BOTTOM, 3);
		sec_hdr->add_theme_style_override("panel", hdr_style);
		modules_container->add_child(sec_hdr);

		Label *hdr_lbl = memnew(Label);
		hdr_lbl->set_text(defs[s].label);
		hdr_lbl->add_theme_color_override("font_color", defs[s].hdr_text);
		sec_hdr->add_child(hdr_lbl);

		if (mods.is_empty()) {
			MarginContainer *em = memnew(MarginContainer);
			em->add_theme_constant_override("margin_left", 8);
			em->add_theme_constant_override("margin_top", 2);
			em->add_theme_constant_override("margin_bottom", 2);
			Label *empty = memnew(Label);
			empty->set_text("(empty)");
			empty->add_theme_color_override("font_color", Color(0.38f, 0.38f, 0.38f));
			em->add_child(empty);
			modules_container->add_child(em);
			continue;
		}

		for (int m = 0; m < mods.size(); m++) {
			Ref<GVSModule> mod = mods[m];
			if (!mod.is_valid()) { continue; }

			const Color cat = mod->get_category_color();

			// Module name row
			PanelContainer *mod_hdr = memnew(PanelContainer);
			Ref<StyleBoxFlat> mod_style;
			mod_style.instantiate();
			mod_style->set_bg_color(Color(0.17f, 0.17f, 0.17f));
			mod_style->set_content_margin_all(0);
			mod_hdr->add_theme_style_override("panel", mod_style);
			modules_container->add_child(mod_hdr);

			HBoxContainer *name_row = memnew(HBoxContainer);
			name_row->add_theme_constant_override("separation", 4);
			mod_hdr->add_child(name_row);

			ColorRect *stripe = memnew(ColorRect);
			stripe->set_color(Color(cat.r, cat.g, cat.b, mod->get_enabled() ? 1.0f : 0.3f));
			stripe->set_custom_minimum_size(Size2(4, 0));
			name_row->add_child(stripe);

			CheckBox *cb = memnew(CheckBox);
			cb->set_pressed(mod->get_enabled());
			cb->set_focus_mode(Control::FOCUS_NONE);
			cb->connect("toggled",
					callable_mp(this, &GVSInspectorPanel::_on_module_enabled_toggled).bind(mod));
			name_row->add_child(cb);

			Label *mod_lbl = memnew(Label);
			mod_lbl->set_text(mod->get_display_name());
			Color nc(cat.r * 0.7f + 0.3f, cat.g * 0.7f + 0.3f, cat.b * 0.7f + 0.3f);
			mod_lbl->add_theme_color_override("font_color", nc);
			mod_lbl->set_h_size_flags(Control::SIZE_EXPAND_FILL);
			name_row->add_child(mod_lbl);

			// Property rows
			Vector<GVSModule::InspectorProp> props = mod->get_inspector_props();
			if (!props.is_empty()) {
				MarginContainer *prop_margin = memnew(MarginContainer);
				prop_margin->add_theme_constant_override("margin_left",   14);
				prop_margin->add_theme_constant_override("margin_right",   6);
				prop_margin->add_theme_constant_override("margin_top",     4);
				prop_margin->add_theme_constant_override("margin_bottom",  6);
				modules_container->add_child(prop_margin);

				GridContainer *prop_grid = memnew(GridContainer);
				prop_grid->set_columns(2);
				prop_grid->set_h_size_flags(Control::SIZE_EXPAND_FILL);
				prop_grid->add_theme_constant_override("h_separation", 8);
				prop_grid->add_theme_constant_override("v_separation", 4);
				prop_margin->add_child(prop_grid);

				for (int pi = 0; pi < props.size(); pi++) {
					const GVSModule::InspectorProp &prop = props[pi];

					Label *key = memnew(Label);
					key->set_text(prop.label);
					key->set_custom_minimum_size(Size2(60, 0));
					key->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
					key->add_theme_color_override("font_color", Color(0.65f, 0.65f, 0.65f));
					prop_grid->add_child(key);

					if (prop.is_resource) {
						EditorResourcePicker *picker = memnew(EditorResourcePicker);
						picker->set_base_type(prop.resource_type);
						picker->set_edited_resource(prop.resource_value);
						picker->set_h_size_flags(Control::SIZE_EXPAND_FILL);
						picker->connect("resource_changed",
								callable_mp(this, &GVSInspectorPanel::_on_module_resource_changed)
								.bind(mod, String(prop.prop_name)));
						prop_grid->add_child(picker);
					} else if (prop.is_color) {
						ColorPickerButton *cpb = memnew(ColorPickerButton);
						cpb->set_pick_color(prop.color_value);
						cpb->set_h_size_flags(Control::SIZE_EXPAND_FILL);
						cpb->set_edit_alpha(true);
						cpb->connect("color_changed",
								callable_mp(this, &GVSInspectorPanel::_on_module_color_changed)
								.bind(mod, String(prop.prop_name)));
						prop_grid->add_child(cpb);
					} else if (prop.is_bool) {
						CheckBox *cb = memnew(CheckBox);
						cb->set_pressed(prop.bool_value);
						cb->set_h_size_flags(Control::SIZE_EXPAND_FILL);
						cb->connect("toggled",
								callable_mp(this, &GVSInspectorPanel::_on_module_bool_changed)
								.bind(mod, String(prop.prop_name)));
						prop_grid->add_child(cb);
					} else {
						GVSPropertyDropZone *dz = memnew(GVSPropertyDropZone);
						dz->set_h_size_flags(Control::SIZE_EXPAND_FILL);
						int var_type = prop.is_int ? GVSVariable::VAR_INT : GVSVariable::VAR_FLOAT;
						dz->setup(float(prop.value), prop.bound_var, var_type,
								prop.min_val, prop.max_val, prop.step);
						dz->connect("value_changed",
								callable_mp(this, &GVSInspectorPanel::_on_module_value_changed)
								.bind(mod, String(prop.prop_name)));
						dz->connect("binding_changed",
								callable_mp(this, &GVSInspectorPanel::_on_module_binding_changed)
								.bind(mod, String(prop.prop_name)));
						prop_grid->add_child(dz);
					}
				}
			}
		}
	}
}

void GVSInspectorPanel::inspect_node(Ref<GVSEmitterNode> p_node) {
	if (!props_container) { return; }

	inspected_node = p_node;

	if (!p_node.is_valid()) {
		props_container->hide();
		empty_label->show();
		return;
	}

	id_label->set_text(itos(p_node->get_id()));

	name_edit->disconnect("text_changed", callable_mp(this, &GVSInspectorPanel::_on_name_changed));
	name_edit->set_text(p_node->get_title());
	name_edit->connect("text_changed", callable_mp(this, &GVSInspectorPanel::_on_name_changed));

	pos_label->set_text(vformat("%.1f,  %.1f",
			p_node->get_canvas_pos().x, p_node->get_canvas_pos().y));

	_rebuild_modules();

	props_container->show();
	empty_label->hide();
}

void GVSInspectorPanel::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
}

GVSInspectorPanel::GVSInspectorPanel() {
	set_custom_minimum_size(Size2(220, 0));
	set_theme_type_variation("Tree");
}

}
