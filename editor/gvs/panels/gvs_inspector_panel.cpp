#include "gvs_inspector_panel.h"

#include "core/object/callable_mp.h"
#include "core/object/class_db.h"
#include "scene/resources/style_box_flat.h"

namespace GodotVisualStream {

void GVSInspectorPanel::_bind_methods() {
	ADD_SIGNAL(MethodInfo("node_changed"));
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

void GVSInspectorPanel::_build_ui() {
	if (props_container) {
		return;
	}

	TabContainer *tabs = memnew(TabContainer);
	tabs->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tabs->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(tabs);

	VBoxContainer *tab_root = memnew(VBoxContainer);
	tab_root->set_name("Inspector");
	tab_root->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tab_root->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tabs->add_child(tab_root);

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

	MarginContainer *grid_margin = memnew(MarginContainer);
	grid_margin->add_theme_constant_override("margin_left", 12);
	grid_margin->add_theme_constant_override("margin_right", 6);
	grid_margin->add_theme_constant_override("margin_top", 4);
	grid_margin->add_theme_constant_override("margin_bottom", 4);
	props_container->add_child(grid_margin);

	GridContainer *grid = memnew(GridContainer);
	grid->set_columns(2);
	grid->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	grid->add_theme_constant_override("h_separation", 8);
	grid->add_theme_constant_override("v_separation", 6);
	grid_margin->add_child(grid);

	auto make_key = [](const String &p_text) -> Label * {
		Label *lbl = memnew(Label);
		lbl->set_text(p_text);
		lbl->set_custom_minimum_size(Size2(70, 0));
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

	props_container->hide();
}

void GVSInspectorPanel::_on_name_changed(const String &p_text) {
	if (!inspected_node.is_valid()) {
		return;
	}
	inspected_node->set_title(p_text);
	emit_signal(SNAME("node_changed"));
}

void GVSInspectorPanel::inspect_node(Ref<GVSEmitterNode> p_node) {
	if (!props_container) {
		return;
	}

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
