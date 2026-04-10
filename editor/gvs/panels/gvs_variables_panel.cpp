#include "gvs_variables_panel.h"

#include "core/object/callable_mp.h"
#include "scene/gui/popup_menu.h"
#include "scene/resources/style_box_flat.h"

namespace GodotVisualStream {

void GVSVariablesPanel::_bind_methods() {
}

void GVSVariablesPanel::_notification(int p_what)
{
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

void GVSVariablesPanel::_build_ui() {
	if (tab_container) {
		return;
	}

	tab_container = memnew(TabContainer);
	tab_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tab_container->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(tab_container);

	VBoxContainer *variables_tab = memnew(VBoxContainer);
	variables_tab->set_name("Variables");
	tab_container->add_child(variables_tab);

	MarginContainer *toolbar_margin = memnew(MarginContainer);
	toolbar_margin->add_theme_constant_override("margin_left", 4);
	toolbar_margin->add_theme_constant_override("margin_right", 4);
	toolbar_margin->add_theme_constant_override("margin_top", 4);
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
	popup->add_item("Float", VAR_FLOAT);
	popup->add_item("Int", VAR_INT);
	popup->add_item("Bool", VAR_BOOL);
	popup->add_item("Color", VAR_COLOR);
	popup->connect("id_pressed", callable_mp(this, &GVSVariablesPanel::_on_add_variable));

	ScrollContainer *scroll = memnew(ScrollContainer);
	scroll->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->set_horizontal_scroll_mode(ScrollContainer::SCROLL_MODE_DISABLED);
	variables_tab->add_child(scroll);

	variable_list = memnew(VBoxContainer);
	variable_list->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	scroll->add_child(variable_list);

	VBoxContainer *config_tab = memnew(VBoxContainer);
	config_tab->set_name("Configuration");
	tab_container->add_child(config_tab);

	Label *config_placeholder = memnew(Label);
	config_placeholder->set_text("No config enabled.");
	config_placeholder->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	config_placeholder->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	config_placeholder->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	config_tab->add_child(config_placeholder);
}

void GVSVariablesPanel::_on_add_variable(int p_type) {
	VariableEntry entry;
	entry.name = "var_" + itos(variable_entries.size());
	entry.type = (VariableType)p_type;
	variable_entries.push_back(entry);
	_rebuild_variable_list();
}

void GVSVariablesPanel::_on_delete_variable(int p_index) {
	if (p_index >= 0 && p_index < variable_entries.size()) {
		variable_entries.remove_at(p_index);
		_rebuild_variable_list();
	}
}

void GVSVariablesPanel::_on_variable_name_changed(const String &p_name, int p_index) {
	if (p_index >= 0 && p_index < variable_entries.size()) {
		variable_entries.write[p_index].name = p_name;
	}
}

void GVSVariablesPanel::_rebuild_variable_list() {
	if (!variable_list) {
		return;
	}

	for (int i = variable_list->get_child_count() - 1; i >= 0; i--) {
		Node *child = variable_list->get_child(i);
		variable_list->remove_child(child);
		child->queue_free();
	}

	for (int i = 0; i < variable_entries.size(); i++) {
		const VariableEntry &entry = variable_entries[i];

		MarginContainer *row_margin = memnew(MarginContainer);
		row_margin->add_theme_constant_override("margin_left", 6);
		row_margin->add_theme_constant_override("margin_right", 4);
		row_margin->add_theme_constant_override("margin_top", 2);
		row_margin->add_theme_constant_override("margin_bottom", 2);
		variable_list->add_child(row_margin);

		HBoxContainer *row = memnew(HBoxContainer);
		row_margin->add_child(row);

		// Tipo coloreado
		Label *type_label = memnew(Label);
		type_label->set_text("[" + _type_name(entry.type) + "]");
		type_label->set_custom_minimum_size(Size2(52, 0));
		switch (entry.type) {
			case VAR_FLOAT: type_label->add_theme_color_override("font_color", Color(0.4f, 0.8f, 1.0f)); break;
			case VAR_INT:   type_label->add_theme_color_override("font_color", Color(0.6f, 1.0f, 0.6f)); break;
			case VAR_BOOL:  type_label->add_theme_color_override("font_color", Color(1.0f, 0.75f, 0.4f)); break;
			case VAR_COLOR: type_label->add_theme_color_override("font_color", Color(1.0f, 0.5f, 0.8f)); break;
		}
		row->add_child(type_label);

		LineEdit *name_edit = memnew(LineEdit);
		name_edit->set_text(entry.name);
		name_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		name_edit->connect("text_changed", callable_mp(this, &GVSVariablesPanel::_on_variable_name_changed).bind(i));
		row->add_child(name_edit);

		Button *del_btn = memnew(Button);
		del_btn->set_text("x");
		del_btn->set_flat(true);
		del_btn->set_custom_minimum_size(Size2(20, 0));
		del_btn->connect("pressed", callable_mp(this, &GVSVariablesPanel::_on_delete_variable).bind(i));
		row->add_child(del_btn);
	}
}

String GVSVariablesPanel::_type_name(VariableType p_type) {
	switch (p_type) {
		case VAR_FLOAT: return "float";
		case VAR_INT:   return "int";
		case VAR_BOOL:  return "bool";
		case VAR_COLOR: return "color";
	}
	return "float";
}

void GVSVariablesPanel::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
}

GVSVariablesPanel::GVSVariablesPanel() {
	set_custom_minimum_size(Size2(240, 150));
	set_theme_type_variation("Tree");
}

}
