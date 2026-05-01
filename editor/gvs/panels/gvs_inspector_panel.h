#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/check_box.h"
#include "scene/gui/color_picker.h"
#include "scene/gui/color_rect.h"
#include "editor/inspector/editor_resource_picker.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/separator.h"
#include "scene/gui/option_button.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/tab_container.h"
#include "editor/gvs/gvs_resource.h"
#include "editor/gvs/nodes/gvs_emitter_node.h"
#include "editor/gvs/nodes/gvs_variable.h"
#include "editor/gvs/modules/gvs_module.h"

namespace GodotVisualStream {

// Drop zone for a single numeric property. Shows a SpinBox when unbound,
// or a variable badge + unbind button when a variable is assigned.
class GVSPropertyDropZone : public HBoxContainer {
	GDCLASS(GVSPropertyDropZone, HBoxContainer);

	float value = 0.0f;
	String bound_var;
	int accepted_var_type = GVSVariable::VAR_FLOAT;

	double spin_min  = -1e6;
	double spin_max  =  1e6;
	double spin_step = 0.01;

	SpinBox *spin_box = nullptr;
	Label *var_badge = nullptr;
	Button *unbind_btn = nullptr;

	void _rebuild_display();
	void _on_spin_changed(double p_val);
	void _on_unbind_pressed();

	bool _can_drop_fwd(const Point2 &p_pos, const Variant &p_data);
	void _drop_fwd(const Point2 &p_pos, const Variant &p_data);

protected:
	static void _bind_methods();

public:
	void setup(float p_value, const String &p_bound_var, int p_accepted_var_type,
			double p_min = -1e6, double p_max = 1e6, double p_step = 0.01);

	float  get_value()     const { return value; }
	String get_bound_var() const { return bound_var; }

	virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
	virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;
};

class GVSInspectorPanel : public PanelContainer {
	GDCLASS(GVSInspectorPanel, PanelContainer);

	Ref<GVSResource> current_resource;
	Ref<GVSEmitterNode> inspected_node;

	VBoxContainer *props_container   = nullptr;
	VBoxContainer *modules_container = nullptr;
	Label    *id_label    = nullptr;
	LineEdit *name_edit   = nullptr;
	Label    *pos_label   = nullptr;
	Label    *empty_label = nullptr;

	void _build_ui();
	void _build_emitter_grid(VBoxContainer *p_container);
	void _rebuild_modules();

	void _on_name_changed(const String &p_text);

	void _on_module_enabled_toggled(bool p_enabled, Ref<GVSModule> p_mod);
	void _on_module_value_changed(float p_val, Ref<GVSModule> p_mod, String p_prop);
	void _on_module_binding_changed(const String &p_var, Ref<GVSModule> p_mod, String p_prop);
	void _on_module_color_changed(const Color &p_color, Ref<GVSModule> p_mod, String p_prop);
	void _on_module_bool_changed(bool p_val, Ref<GVSModule> p_mod, String p_prop);
	void _on_module_resource_changed(const Ref<Resource> &p_res, Ref<GVSModule> p_mod, String p_prop);
	void _on_module_enum_changed(int p_idx, Ref<GVSModule> p_mod, String p_prop);
	void _on_module_vector3_component_changed(double p_val, Ref<GVSModule> p_mod, String p_prop, int p_component);
	void _on_module_delete_pressed(int p_section, int p_mod_idx);

	bool _can_drop_v3(const Point2 &p_pos, const Variant &p_data);
	void _drop_v3_data(const Point2 &p_pos, const Variant &p_data, Ref<GVSModule> p_mod, String p_prop);
	void _on_v3_unbind_pressed(Ref<GVSModule> p_mod, String p_prop);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void load_resource(const Ref<GVSResource> &p_resource);
	void inspect_node(Ref<GVSEmitterNode> p_node);
	void refresh() { if (inspected_node.is_valid()) { inspect_node(inspected_node); } }

	GVSInspectorPanel();
};

}
