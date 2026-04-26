#pragma once

#include "core/io/resource.h"
#include "core/math/vector2.h"
#include "core/string/ustring.h"
#include "core/variant/typed_array.h"
#include "editor/gvs/modules/gvs_module.h"

class GPUParticles3D;

namespace GodotVisualStream {

class GVSEmitterNode : public Resource {
	GDCLASS(GVSEmitterNode, Resource);

	int id = 0;
	String title = "Emitter";
	Vector2 canvas_pos;
	bool selected = false;

	TypedArray<GVSModule> spawn_modules;
	TypedArray<GVSModule> update_modules;
	TypedArray<GVSModule> render_modules;

protected:
	static void _bind_methods();

public:
	static constexpr float NODE_WIDTH        = 180.0f;
	static constexpr float HEADER_HEIGHT     = 28.0f;
	static constexpr float SECTION_LABEL_H   = 18.0f;
	static constexpr float SECTION_ADD_BTN_H = 18.0f;

	float get_section_height(int p_section) const {
		int n = 0;
		if (p_section == 0)      { n = spawn_modules.size(); }
		else if (p_section == 1) { n = update_modules.size(); }
		else                     { n = render_modules.size(); }
		return SECTION_LABEL_H + n * GVSModule::BOX_HEIGHT + SECTION_ADD_BTN_H;
	}

	float get_node_height() const {
		return HEADER_HEIGHT
			+ get_section_height(0)
			+ get_section_height(1)
			+ get_section_height(2);
	}

	static Ref<GVSEmitterNode> create(const String &p_title = "Emitter", Vector2 p_pos = Vector2());
	static void ensure_id_counter(int p_id);

	void set_id(int p_id) { id = p_id; }
	int get_id() const { return id; }

	void set_title(const String &p_title) { title = p_title; }
	String get_title() const { return title; }

	void set_canvas_pos(Vector2 p_pos) { canvas_pos = p_pos; }
	Vector2 get_canvas_pos() const { return canvas_pos; }

	void set_selected(bool p_selected) { selected = p_selected; }
	bool is_selected() const { return selected; }

	void set_spawn_modules(const TypedArray<GVSModule> &m)  { spawn_modules  = m; }
	TypedArray<GVSModule> get_spawn_modules()  const { return spawn_modules; }
	void add_spawn_module(const Ref<GVSModule> &m)  { spawn_modules.push_back(m); }

	void set_update_modules(const TypedArray<GVSModule> &m) { update_modules = m; }
	TypedArray<GVSModule> get_update_modules() const { return update_modules; }
	void add_update_module(const Ref<GVSModule> &m) { update_modules.push_back(m); }

	void set_render_modules(const TypedArray<GVSModule> &m)  { render_modules  = m; }
	TypedArray<GVSModule> get_render_modules()  const { return render_modules; }
	void add_render_module(const Ref<GVSModule> &m)  { render_modules.push_back(m); }

	Rect2 get_canvas_rect() const { return Rect2(canvas_pos, Vector2(NODE_WIDTH, get_node_height())); }

	GPUParticles3D *create_gpu_particles() const;
};

}
