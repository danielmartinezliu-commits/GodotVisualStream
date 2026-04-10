#pragma once

#include "core/object/ref_counted.h"
#include "core/math/vector2.h"
#include "core/string/ustring.h"

namespace GodotVisualStream {

class GVSEmitterNode : public RefCounted {
	GDCLASS(GVSEmitterNode, RefCounted);

	int     id         = 0;
	String  title      = "Emitter";
	Vector2 canvas_pos;
	bool    selected   = false;

protected:
	static void _bind_methods();

public:
	static constexpr float NODE_WIDTH    = 180.0f;
	static constexpr float NODE_HEIGHT   = 100.0f;
	static constexpr float HEADER_HEIGHT =  28.0f;

	static Ref<GVSEmitterNode> create(const String &p_title = "Emitter", Vector2 p_pos = Vector2());

	int     get_id() const                  { return id; }

	void    set_title(const String &p_title) { title = p_title; }
	String  get_title() const               { return title; }

	void    set_canvas_pos(Vector2 p_pos)   { canvas_pos = p_pos; }
	Vector2 get_canvas_pos() const          { return canvas_pos; }

	void    set_selected(bool p_selected)   { selected = p_selected; }
	bool    is_selected() const             { return selected; }

	Rect2   get_canvas_rect() const {
		return Rect2(canvas_pos, Vector2(NODE_WIDTH, NODE_HEIGHT));
	}
};

}
