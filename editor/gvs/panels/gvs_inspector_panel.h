#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/spin_box.h"
#include "scene/gui/tab_container.h"
#include "editor/gvs/gvs_resource.h"
#include "editor/gvs/nodes/gvs_emitter_node.h"
#include "editor/gvs/nodes/gvs_variable.h"

namespace GodotVisualStream {

// Drop zone for a single numeric property. Shows a SpinBox when unbound,
// or a variable badge + unbind button when a variable is assigned.
class GVSPropertyDropZone : public HBoxContainer {
	GDCLASS(GVSPropertyDropZone, HBoxContainer);

	float value = 0.0f;
	String bound_var;
	int accepted_var_type = GVSVariable::VAR_FLOAT;

	SpinBox *spin_box = nullptr;
	Label *var_badge = nullptr;
	Button *unbind_btn = nullptr;

	void _rebuild_display();
	void _on_spin_changed(double p_val);
	void _on_unbind_pressed();

	// Forwarding callables used by set_drag_forwarding on child controls.
	bool _can_drop_fwd(const Point2 &p_pos, const Variant &p_data);
	void _drop_fwd(const Point2 &p_pos, const Variant &p_data);

protected:
	static void _bind_methods();

public:
	void setup(float p_value, const String &p_bound_var, int p_accepted_var_type);

	float  get_value()     const { return value; }
	String get_bound_var() const { return bound_var; }

	virtual bool can_drop_data(const Point2 &p_point, const Variant &p_data) const override;
	virtual void drop_data(const Point2 &p_point, const Variant &p_data) override;
};

class GVSInspectorPanel : public PanelContainer {
	GDCLASS(GVSInspectorPanel, PanelContainer);

	Ref<GVSResource> current_resource;
	Ref<GVSEmitterNode> inspected_node;

	VBoxContainer *props_container = nullptr;
	Label *id_label = nullptr;
	LineEdit  *name_edit = nullptr;
	Label *pos_label = nullptr;
	Label *empty_label = nullptr;
	GVSPropertyDropZone *lifetime_drop = nullptr;

	void _build_ui();
	void _build_props_grid(VBoxContainer *p_container);
	void _on_name_changed(const String &p_text);
	void _on_lifetime_value_changed(float p_val);
	void _on_lifetime_binding_changed(const String &p_var);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void load_resource(const Ref<GVSResource> &p_resource);
	void inspect_node(Ref<GVSEmitterNode> p_node);

	GVSInspectorPanel();
};

}
