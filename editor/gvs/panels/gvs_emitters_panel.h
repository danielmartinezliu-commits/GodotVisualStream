#pragma once

#include "scene/gui/control.h"
#include "scene/gui/button.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/box_container.h"
#include "scene/resources/style_box_flat.h"
#include "scene/scene_string_names.h"
#include "core/input/input_event.h"
#include "core/templates/vector.h"
#include "editor/gvs/gvs_resource.h"
#include "editor/gvs/nodes/gvs_emitter_node.h"

namespace GodotVisualStream {

class GVSEmittersPanel : public Control {
	GDCLASS(GVSEmittersPanel, Control);

	Ref<GVSResource> current_resource;

	// Barra superpuesta dentro del canvas (no ocupa espacio encima)
	MarginContainer *overlay_bar = nullptr;

	Vector<Ref<GVSEmitterNode>> nodes;

	int     dragging_node    = -1;
	Vector2 drag_node_offset;

	float   zoom          = 1.0f;
	Vector2 scroll_offset;
	bool    panning          = false;
	Vector2 pan_start_mouse;
	Vector2 pan_start_offset;

	Color _panel_border_color = Color(0.1f, 0.1f, 0.1f);

	static constexpr float ZOOM_MIN  = 0.15f;
	static constexpr float ZOOM_MAX  = 4.0f;
	static constexpr float ZOOM_STEP = 0.12f;
	static constexpr float GRID_STEP = 25.0f;

	void    _draw_grid();
	void    _draw_nodes();
	void    _draw_single_node(const Ref<GVSEmitterNode> &p_node);

	Vector2 _screen_to_canvas(Vector2 p_screen) const;
	Vector2 _canvas_to_screen(Vector2 p_canvas) const;
	int     _node_at_screen(Vector2 p_screen) const;

	void    _on_add_node_pressed();
	void    _save();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	virtual void gui_input(const Ref<InputEvent> &p_event) override;

public:
	void load_resource(const Ref<GVSResource> &p_resource);

	GVSEmittersPanel();
};

}