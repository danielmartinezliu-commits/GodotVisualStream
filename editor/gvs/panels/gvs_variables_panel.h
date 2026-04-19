#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/color_picker.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/tab_container.h"
#include "editor/gvs/gvs_resource.h"
#include "editor/gvs/nodes/gvs_variable.h"

namespace GodotVisualStream {

// Row control that supports drag-and-drop of its variable onto inspector properties.
class GVSVariableRow : public HBoxContainer {
	GDCLASS(GVSVariableRow, HBoxContainer);

	Ref<GVSVariable> variable;

protected:
	static void _bind_methods() {}

public:
	void setup(const Ref<GVSVariable> &p_var) { variable = p_var; }
	virtual Variant get_drag_data(const Point2 &p_at_position) override;
};

class GVSVariablesPanel : public PanelContainer {
	GDCLASS(GVSVariablesPanel, PanelContainer);

	Ref<GVSResource> current_resource;

	TabContainer *tab_container  = nullptr;
	VBoxContainer *variable_list = nullptr;

	void _build_ui();
	void _build_variables_tab(TabContainer *p_tabs);
	void _build_config_tab(TabContainer *p_tabs);

	void _on_add_variable(int p_type);
	void _on_delete_variable(int p_index);
	void _on_variable_name_changed(const String &p_name, int p_index);
	void _on_value_float_changed(double p_val, int p_index);
	void _on_value_int_changed(double p_val, int p_index);
	void _on_value_bool_changed(bool p_val, int p_index);
	void _on_value_color_changed(const Color &p_color, int p_index);

	void _rebuild_variable_list();
	void _save_to_resource();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	static Color _type_color(int p_type);
	static String _type_name(int p_type);

	void load_resource(const Ref<GVSResource> &p_resource);

	GVSVariablesPanel();
};

}
