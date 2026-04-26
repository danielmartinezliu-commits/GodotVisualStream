#include "gvs_variables_panel.h"

#include "core/object/callable_mp.h"
#include "scene/gui/popup_menu.h"
#include "scene/resources/style_box_flat.h"

namespace GodotVisualStream {

// ---------------------------------------------------------------------------
// GVSVariableRow — drag source
// ---------------------------------------------------------------------------

Variant GVSVariableRow::get_drag_data(const Point2 &p_at_position) {
	if (!variable.is_valid()) {
		return Variant();
	}

	Dictionary data;
	data["type"]     = "gvs_variable";
	data["name"]     = variable->get_var_name();
	data["var_type"] = variable->get_type();

	Label *preview = memnew(Label);
	preview->set_text("[" + GVSVariablesPanel::_type_name(variable->get_type()) + "] " + variable->get_var_name());
	preview->add_theme_color_override("font_color", GVSVariablesPanel::_type_color(variable->get_type()));
	set_drag_preview(preview);

	return data;
}

// GVSVariablesPanel

void GVSVariablesPanel::_bind_methods() {
	ADD_SIGNAL(MethodInfo("variable_changed"));
}

void GVSVariablesPanel::_notification(int p_what) {
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

void GVSVariablesPanel::_build_variables_tab(TabContainer *p_tabs) {
	VBoxContainer *variables_tab = memnew(VBoxContainer);
	variables_tab->set_name("Variables");
	p_tabs->add_child(variables_tab);

	MarginContainer *toolbar_margin = memnew(MarginContainer);
	toolbar_margin->add_theme_constant_override("margin_left",   4);
	toolbar_margin->add_theme_constant_override("margin_right",  4);
	toolbar_margin->add_theme_constant_override("margin_top",    4);
	toolbar_margin->add_theme_constant_override("margin_bottom", 4);
	variables_tab->add_child(toolbar_margin);

	HBoxContainer *toolbar = memnew(HBoxContainer);
	toolbar_margin->add_child(toolbar);

	MenuButton *add_var_btn = memnew(MenuButton);
	add_var_btn->set_text("Add Variable");
	add_var_btn->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	add_var_btn->set_flat(true);
	toolbar->add_child(add_var_btn);

	PopupMenu *popup = add_var_btn->get_popup();
	popup->add_item("Float", GVSVariable::VAR_FLOAT);
	popup->add_item("Int",   GVSVariable::VAR_INT);
	popup->add_item("Bool",  GVSVariable::VAR_BOOL);
	popup->add_item("Color", GVSVariable::VAR_COLOR);
	// TODO: Vectores
	popup->connect("id_pressed", callable_mp(this, &GVSVariablesPanel::_on_add_variable));

	ScrollContainer *scroll = memnew(ScrollContainer);
	scroll->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);
	variables_tab->add_child(scroll);

	variable_list = memnew(VBoxContainer);
	variable_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->add_child(variable_list);
}

void GVSVariablesPanel::_build_config_tab(TabContainer *p_tabs) {
	VBoxContainer *config_tab = memnew(VBoxContainer);
	config_tab->set_name("Configuration");
	p_tabs->add_child(config_tab);

	Label *placeholder = memnew(Label);
	placeholder->set_text("No config enabled.");
	placeholder->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	placeholder->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	placeholder->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	config_tab->add_child(placeholder);
}

void GVSVariablesPanel::_build_ui() {
	if (tab_container) {
		return;
	}

	tab_container = memnew(TabContainer);
	tab_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tab_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(tab_container);

	_build_variables_tab(tab_container);
	_build_config_tab(tab_container);
}

// Variable CRUD

void GVSVariablesPanel::_on_add_variable(int p_type) {
	if (!current_resource.is_valid()) {
		return;
	}
	TypedArray<GVSVariable> vars = current_resource->get_variables();
	String new_name = "var_" + itos(vars.size());
	vars.push_back(GVSVariable::create(new_name, p_type));
	current_resource->set_variables(vars);
	_rebuild_variable_list();
	_save_to_resource();
}

void GVSVariablesPanel::_on_delete_variable(int p_index) {
	if (!current_resource.is_valid()) {
		return;
	}
	TypedArray<GVSVariable> vars = current_resource->get_variables();
	if (p_index < 0 || p_index >= vars.size()) {
		return;
	}
	vars.remove_at(p_index);
	current_resource->set_variables(vars);
	_rebuild_variable_list();
	_save_to_resource();
}

void GVSVariablesPanel::_on_variable_name_changed(const String &p_name, int p_index) {
	if (!current_resource.is_valid()) {
		return;
	}
	TypedArray<GVSVariable> vars = current_resource->get_variables();
	if (p_index < 0 || p_index >= vars.size()) {
		return;
	}
	Ref<GVSVariable> var = vars[p_index];
	if (var.is_valid()) {
		var->set_var_name(p_name);
	}
	_save_to_resource();
}

void GVSVariablesPanel::_on_value_float_changed(double p_val, int p_index) {
	if (!current_resource.is_valid()) { return; }
	TypedArray<GVSVariable> vars = current_resource->get_variables();
	if (p_index < 0 || p_index >= vars.size()) { return; }
	Ref<GVSVariable> var = vars[p_index];
	if (var.is_valid()) { var->set_default_value(float(p_val)); }
	_save_to_resource();
}

void GVSVariablesPanel::_on_value_int_changed(double p_val, int p_index) {
	if (!current_resource.is_valid()) { return; }
	TypedArray<GVSVariable> vars = current_resource->get_variables();
	if (p_index < 0 || p_index >= vars.size()) { return; }
	Ref<GVSVariable> var = vars[p_index];
	if (var.is_valid()) { var->set_default_value(int(p_val)); }
	_save_to_resource();
}

void GVSVariablesPanel::_on_value_bool_changed(bool p_val, int p_index) {
	if (!current_resource.is_valid()) { return; }
	TypedArray<GVSVariable> vars = current_resource->get_variables();
	if (p_index < 0 || p_index >= vars.size()) { return; }
	Ref<GVSVariable> var = vars[p_index];
	if (var.is_valid()) { var->set_default_value(p_val); }
	_save_to_resource();
}

void GVSVariablesPanel::_on_value_color_changed(const Color &p_color, int p_index) {
	if (!current_resource.is_valid()) { return; }
	TypedArray<GVSVariable> vars = current_resource->get_variables();
	if (p_index < 0 || p_index >= vars.size()) { return; }
	Ref<GVSVariable> var = vars[p_index];
	if (var.is_valid()) { var->set_default_value(p_color); }
	_save_to_resource();
}

// Rebuild list

void GVSVariablesPanel::_rebuild_variable_list() {
	if (!variable_list) {
		return;
	}

	for (int i = variable_list->get_child_count() - 1; i >= 0; i--) {
		Node *child = variable_list->get_child(i);
		variable_list->remove_child(child);
		child->queue_free();
	}

	if (!current_resource.is_valid()) {
		return;
	}

	TypedArray<GVSVariable> vars = current_resource->get_variables();

	for (int i = 0; i < vars.size(); i++) {
		Ref<GVSVariable> var = vars[i];
		if (!var.is_valid()) {
			continue;
		}

		MarginContainer *row_margin = memnew(MarginContainer);
		row_margin->add_theme_constant_override("margin_left",   4);
		row_margin->add_theme_constant_override("margin_right",  4);
		row_margin->add_theme_constant_override("margin_top",    2);
		row_margin->add_theme_constant_override("margin_bottom", 2);
		variable_list->add_child(row_margin);

		GVSVariableRow *row = memnew(GVSVariableRow);
		row->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		row->set_mouse_filter(Control::MOUSE_FILTER_STOP);
		row->setup(var);
		row_margin->add_child(row);

		// Type badge — also acts as drag handle (MOUSE_FILTER_PASS lets clicks reach the row)
		Label *type_label = memnew(Label);
		type_label->set_text("[" + _type_name(var->get_type()) + "]");
		type_label->set_custom_minimum_size(Size2(52, 0));
		type_label->add_theme_color_override("font_color", _type_color(var->get_type()));
		type_label->set_mouse_filter(Control::MOUSE_FILTER_PASS);
		row->add_child(type_label);

		// Name editor
		LineEdit *name_edit = memnew(LineEdit);
		name_edit->set_text(var->get_var_name());
		name_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		name_edit->connect("text_changed",
				callable_mp(this, &GVSVariablesPanel::_on_variable_name_changed).bind(i));
		row->add_child(name_edit);

		// Default value widget
		int vtype = var->get_type();
		if (vtype == GVSVariable::VAR_FLOAT) {
			SpinBox *spin = memnew(SpinBox);
			spin->set_min(-1e6);
			spin->set_max(1e6);
			spin->set_step(0.01);
			spin->set_value(double(var->get_default_value()));
			spin->set_custom_minimum_size(Size2(72, 0));
			spin->connect("value_changed",
					callable_mp(this, &GVSVariablesPanel::_on_value_float_changed).bind(i));
			row->add_child(spin);

		} else if (vtype == GVSVariable::VAR_INT) {
			SpinBox *spin = memnew(SpinBox);
			spin->set_min(-1e6);
			spin->set_max(1e6);
			spin->set_step(1);
			spin->set_value(double(int(var->get_default_value())));
			spin->set_custom_minimum_size(Size2(72, 0));
			spin->connect("value_changed",
					callable_mp(this, &GVSVariablesPanel::_on_value_int_changed).bind(i));
			row->add_child(spin);

		} else if (vtype == GVSVariable::VAR_BOOL) {
			CheckBox *check = memnew(CheckBox);
			check->set_pressed(bool(var->get_default_value()));
			check->set_text("");
			check->connect("toggled",
					callable_mp(this, &GVSVariablesPanel::_on_value_bool_changed).bind(i));
			row->add_child(check);

		} else if (vtype == GVSVariable::VAR_COLOR) {
			ColorPickerButton *cpb = memnew(ColorPickerButton);
			cpb->set_pick_color(Color(var->get_default_value()));
			cpb->set_custom_minimum_size(Size2(40, 0));
			cpb->connect("color_changed",
					callable_mp(this, &GVSVariablesPanel::_on_value_color_changed).bind(i));
			row->add_child(cpb);
		}

		// Delete button
		Button *del_btn = memnew(Button);
		del_btn->set_text("x");
		del_btn->set_flat(true);
		del_btn->set_custom_minimum_size(Size2(20, 0));
		del_btn->connect("pressed",
				callable_mp(this, &GVSVariablesPanel::_on_delete_variable).bind(i));
		row->add_child(del_btn);
	}
}

void GVSVariablesPanel::_save_to_resource() {
	emit_signal(SNAME("variable_changed"));
}

// Helpers

Color GVSVariablesPanel::_type_color(int p_type) {
	switch (p_type) {
		case GVSVariable::VAR_FLOAT: return Color(0.4f, 0.8f, 1.0f);
		case GVSVariable::VAR_INT: return Color(0.6f, 1.0f, 0.6f);
		case GVSVariable::VAR_BOOL: return Color(1.0f, 0.75f, 0.4f);
		case GVSVariable::VAR_COLOR: return Color(1.0f, 0.5f, 0.8f);
		// TODO: Vectores
	}
	return Color(1, 1, 1);
}

String GVSVariablesPanel::_type_name(int p_type) {
	switch (p_type) {
		case GVSVariable::VAR_FLOAT: return "float";
		case GVSVariable::VAR_INT:   return "int";
		case GVSVariable::VAR_BOOL:  return "bool";
		case GVSVariable::VAR_COLOR: return "color";
		// TODO: Vectores
	}
	return "float";
}

// Resource load

void GVSVariablesPanel::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
	_rebuild_variable_list();
}

GVSVariablesPanel::GVSVariablesPanel() {
	set_custom_minimum_size(Size2(240, 150));
	set_theme_type_variation("Tree");
}

}
