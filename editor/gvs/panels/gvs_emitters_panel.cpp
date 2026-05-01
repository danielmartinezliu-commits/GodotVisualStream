#include "gvs_emitters_panel.h"

#include "core/input/input_event.h"
#include "core/io/resource_saver.h"
#include "core/object/callable_mp.h"
#include "scene/scene_string_names.h"

namespace GodotVisualStream {

void GVSEmittersPanel::_bind_methods() {
	ADD_SIGNAL(MethodInfo("node_selected",
			PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "GVSEmitterNode")));
	ADD_SIGNAL(MethodInfo("node_add_module_pressed",
			PropertyInfo(Variant::OBJECT, "node", PROPERTY_HINT_RESOURCE_TYPE, "GVSEmitterNode")));
	ADD_SIGNAL(MethodInfo("node_changed"));
}

void GVSEmittersPanel::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_RESIZED: {
			// Ajusta la barra overlay para que ocupe todo el ancho al redimensionar
			if (overlay_bar) {
				const float overlay_h = overlay_bar->get_minimum_size().y;
				overlay_bar->set_position(Vector2(0, 0));
				overlay_bar->set_size(Vector2(get_size().x, overlay_h));
			}
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			// Recachea el color del borde al cambiar el tema del editor
			Ref<StyleBoxFlat> flat = get_theme_stylebox(SceneStringName(panel), SNAME("Tree"));
			if (flat.is_valid()) {
				_panel_border_color = flat->get_bg_color().lightened(0.35f);
			}
			queue_redraw();
		} break;
		case NOTIFICATION_DRAW: {
			// Orden de pintado: fondo+grid → nodos → borde exterior
			_draw_grid();
			_draw_nodes();
			draw_rect(Rect2(Vector2(), get_size()), _panel_border_color, false, 1.0f);
		} break;
	}
}

void GVSEmittersPanel::_draw_grid() {
	const Size2 size = get_size();

	// Fondo oscuro (toda la zona del canvas)
	draw_rect(Rect2(0, 0, size.x, size.y), Color(0.0902f, 0.0902f, 0.0902f)); // #171717

	const float step = GRID_STEP * zoom;
	if (step < 3.0f) {
		return;
	}

	// Offset de scroll con módulo para repetir los dots suavemente
	float ox = Math::fmod(scroll_offset.x, step);
	float oy = Math::fmod(scroll_offset.y, step);
	if (ox < 0.0f) { ox += step; }
	if (oy < 0.0f) { oy += step; }

	const int   major_every  = 5;
	const float major_step   = step * major_every;
	float major_ox = Math::fmod(scroll_offset.x, major_step);
	float major_oy = Math::fmod(scroll_offset.y, major_step);
	if (major_ox < 0.0f) { major_ox += major_step; }
	if (major_oy < 0.0f) { major_oy += major_step; }

	const Color dot_color(0.39f, 0.39f, 0.39f);
	const Color major_color(0.50f, 0.50f, 0.50f);

	for (float x = ox; x < size.x + step; x += step) {
		for (float y = oy; y < size.y + step; y += step) {
			const bool is_major_x = Math::fmod(x - ox + major_ox, major_step) < step * 0.5f;
			const bool is_major_y = Math::fmod(y - oy + major_oy, major_step) < step * 0.5f;

			if (is_major_x && is_major_y) {
				draw_rect(Rect2(x - 1.5f, y - 1.5f, 3.0f, 3.0f), major_color);
			} else {
				draw_rect(Rect2(x - 1.0f, y - 1.0f, 2.0f, 2.0f), dot_color);
			}
		}
	}
}

// Dibuja todos los nodos del canvas en orden (el último en el array queda encima)
void GVSEmittersPanel::_draw_nodes() {
	for (int i = 0; i < nodes.size(); i++) {
		_draw_single_node(nodes[i]);
	}
}

void GVSEmittersPanel::_draw_single_node(const Ref<GVSEmitterNode> &p_node) {
	const Vector2 pos = _canvas_to_screen(p_node->get_canvas_pos());
	const float w  = GVSEmitterNode::NODE_WIDTH    * zoom;
	const float h  = p_node->get_node_height()     * zoom;
	const float hh = GVSEmitterNode::HEADER_HEIGHT * zoom;

	draw_rect(Rect2(pos, Vector2(w, h)), Color(0.20f, 0.20f, 0.20f));
	draw_rect(Rect2(pos, Vector2(w, hh)), Color(0.18f, 0.45f, 0.78f));
	const Color border = p_node->is_selected() ? Color(0.9f, 0.6f, 0.1f) : Color(0.12f, 0.12f, 0.12f);
	draw_rect(Rect2(pos, Vector2(w, h)), border, false, p_node->is_selected() ? 2.0f : 1.0f);

	Ref<Font> font = get_theme_font("font", "Label");
	if (!font.is_valid()) { return; }

	const int  font_size = MAX(10, (int)(13 * zoom));
	const int  prop_size = MAX(8,  (int)(10 * zoom));

	// Título en cabecera
	const float title_y = pos.y + (hh + font->get_ascent(font_size)) * 0.5f;
	draw_string(font, Vector2(pos.x + 8.0f * zoom, title_y),
			p_node->get_title(), HORIZONTAL_ALIGNMENT_LEFT,
			w - 16.0f * zoom, font_size, Color(1, 1, 1));

	draw_line(Vector2(pos.x, pos.y + hh), Vector2(pos.x + w, pos.y + hh),
			Color(0.0f, 0.0f, 0.0f, 0.5f), 1.0f);

	// Tres secciones apiladas bajo la cabecera
	float section_y = pos.y + hh;
	for (int s = 0; s < 3; s++) {
		section_y = _draw_section(p_node, s, section_y, font, prop_size, zoom);
	}
}

// Dibuja una sección (label + módulos + botón "+") y devuelve la Y donde termina
float GVSEmittersPanel::_draw_section(const Ref<GVSEmitterNode> &p_node, int p_section,
		float p_start_y, const Ref<Font> &p_font, int p_prop_size, float p_zoom) {

	const float w         = GVSEmitterNode::NODE_WIDTH     * p_zoom;
	const float label_h   = GVSEmitterNode::SECTION_LABEL_H   * p_zoom;
	const float add_btn_h = GVSEmitterNode::SECTION_ADD_BTN_H * p_zoom;
	const float mod_h     = GVSModule::BOX_HEIGHT * p_zoom;
	const float stripe_w  = 3.0f * p_zoom;
	const Vector2 pos     = _canvas_to_screen(p_node->get_canvas_pos());

	// Colores de sección
	struct SectionDef { const char *label; Color bg; Color text_col; };
	static const SectionDef defs[3] = {
		{ "PARTICLE SPAWN",  Color(0.14f, 0.19f, 0.14f), Color(0.45f, 0.85f, 0.50f) },
		{ "PARTICLE UPDATE", Color(0.13f, 0.16f, 0.22f), Color(0.45f, 0.65f, 1.00f) },
		{ "RENDER",          Color(0.19f, 0.14f, 0.20f), Color(0.80f, 0.50f, 0.85f) },
	};

	const TypedArray<GVSModule> &mods = (p_section == 0)
			? p_node->get_spawn_modules()
			: (p_section == 1 ? p_node->get_update_modules() : p_node->get_render_modules());

	// Cabecera de sección
	draw_rect(Rect2(pos.x, p_start_y, w, label_h), defs[p_section].bg);
	const float label_text_y = p_start_y + (label_h + p_font->get_ascent(p_prop_size)) * 0.5f;
	draw_string(p_font, Vector2(pos.x + 6.0f * p_zoom, label_text_y),
			defs[p_section].label, HORIZONTAL_ALIGNMENT_LEFT,
			w - 8.0f * p_zoom, p_prop_size, defs[p_section].text_col);

	float cur_y = p_start_y + label_h;

	// Módulos de la sección
	for (int m = 0; m < mods.size(); m++) {
		Ref<GVSModule> mod = mods[m];
		if (!mod.is_valid()) { continue; }

		const Color cat  = mod->get_category_color();
		const float alpha = mod->get_enabled() ? 1.0f : 0.4f;

		draw_rect(Rect2(pos.x, cur_y, w, mod_h), Color(0.19f, 0.19f, 0.19f));
		draw_rect(Rect2(pos.x, cur_y, stripe_w, mod_h), Color(cat.r, cat.g, cat.b, alpha));
		draw_line(Vector2(pos.x, cur_y + mod_h - 1), Vector2(pos.x + w, cur_y + mod_h - 1),
				Color(0.0f, 0.0f, 0.0f, 0.2f), 1.0f);

		const Color name_col = mod->get_enabled()
				? Color(cat.r * 0.7f + 0.3f, cat.g * 0.7f + 0.3f, cat.b * 0.7f + 0.3f)
				: Color(0.45f, 0.45f, 0.45f);
		const float text_y = cur_y + (mod_h + p_font->get_ascent(p_prop_size)) * 0.5f;
		draw_string(p_font, Vector2(pos.x + stripe_w + 5.0f * p_zoom, text_y),
				mod->get_display_name(), HORIZONTAL_ALIGNMENT_LEFT,
				w - stripe_w - 8.0f * p_zoom, p_prop_size, name_col);

		cur_y += mod_h;
	}

	// Botón "+" de la sección
	draw_rect(Rect2(pos.x, cur_y, w, add_btn_h), Color(0.16f, 0.16f, 0.16f));
	draw_line(Vector2(pos.x, cur_y), Vector2(pos.x + w, cur_y),
			Color(0.0f, 0.0f, 0.0f, 0.3f), 1.0f);

	const int  plus_size = MAX(8, (int)(10 * p_zoom));
	const float plus_y   = cur_y + (add_btn_h + p_font->get_ascent(plus_size)) * 0.5f;
	draw_string(p_font, Vector2(pos.x, plus_y),
			"+ Add Module", HORIZONTAL_ALIGNMENT_CENTER,
			w, plus_size, Color(0.60f, 0.60f, 0.60f));

	draw_line(Vector2(pos.x, cur_y + add_btn_h), Vector2(pos.x + w, cur_y + add_btn_h),
			Color(0.0f, 0.0f, 0.0f, 0.4f), 1.0f);

	return cur_y + add_btn_h;
}

// Devuelve true si p_screen está sobre el botón "+" de alguna sección; rellena r_node y r_section
bool GVSEmittersPanel::_section_add_btn_at_screen(Vector2 p_screen, int &r_node, int &r_section) const {
	for (int i = nodes.size() - 1; i >= 0; i--) {
		const Vector2 pos = _canvas_to_screen(nodes[i]->get_canvas_pos());
		const float w = GVSEmitterNode::NODE_WIDTH * zoom;

		if (p_screen.x < pos.x || p_screen.x > pos.x + w) {
			continue;
		}

		float cur_y = pos.y + GVSEmitterNode::HEADER_HEIGHT * zoom;
		for (int s = 0; s < 3; s++) {
			const float sec_h     = nodes[i]->get_section_height(s) * zoom;
			const float add_btn_h = GVSEmitterNode::SECTION_ADD_BTN_H * zoom;
			const float btn_top   = cur_y + sec_h - add_btn_h;

			if (p_screen.y >= btn_top && p_screen.y < cur_y + sec_h) {
				r_node    = i;
				r_section = s;
				return true;
			}
			cur_y += sec_h;
		}
	}
	return false;
}

// Convierte coordenadas de pantalla a espacio canvas (tiene en cuenta scroll y zoom)
Vector2 GVSEmittersPanel::_screen_to_canvas(Vector2 p_screen) const {
	return (p_screen - scroll_offset) / zoom;
}

Vector2 GVSEmittersPanel::_canvas_to_screen(Vector2 p_canvas) const {
	return p_canvas * zoom + scroll_offset;
}

// Devuelve el índice del nodo bajo el cursor, o -1 si no hay ninguno (recorre al revés para priorizar el de encima)
int GVSEmittersPanel::_node_at_screen(Vector2 p_screen) const {
	for (int i = nodes.size() - 1; i >= 0; i--) {
		const Rect2 r(
				_canvas_to_screen(nodes[i]->get_canvas_pos()),
				Vector2(GVSEmitterNode::NODE_WIDTH * zoom, nodes[i]->get_node_height() * zoom));
		if (r.has_point(p_screen)) {
			return i;
		}
	}
	return -1;
}

void GVSEmittersPanel::gui_input(const Ref<InputEvent> &p_event) {
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid()) {
		if (mb->is_pressed()) {
			if (mb->get_button_index() == MouseButton::WHEEL_UP) {
				const float old_zoom = zoom;
				zoom = MIN(zoom * (1.0f + ZOOM_STEP), ZOOM_MAX);
				const Vector2 mouse = mb->get_position();
				scroll_offset = mouse - (mouse - scroll_offset) * (zoom / old_zoom);
				queue_redraw();
				accept_event();
			} else if (mb->get_button_index() == MouseButton::WHEEL_DOWN) {
				const float old_zoom = zoom;
				zoom = MAX(zoom * (1.0f - ZOOM_STEP), ZOOM_MIN);
				const Vector2 mouse = mb->get_position();
				scroll_offset = mouse - (mouse - scroll_offset) * (zoom / old_zoom);
				queue_redraw();
				accept_event();
			} else if (mb->get_button_index() == MouseButton::LEFT) {
						// Primero comprueba si el clic cae sobre el "+" de alguna sección
				int add_node = -1, add_section = -1;
				if (_section_add_btn_at_screen(mb->get_position(), add_node, add_section)) {
					module_menu_node_idx = add_node;
					module_menu_section  = add_section;

					module_menu->clear();
					// Collect type IDs already on this node to disable duplicates
					const Ref<GVSEmitterNode> &cur_node = nodes[add_node];
					auto node_has = [&](const char *tid) -> bool {
						auto check = [&](const TypedArray<GVSModule> &list) -> bool {
							for (int ci = 0; ci < list.size(); ci++) {
								Ref<GVSModule> cm = list[ci];
								if (cm.is_valid() && cm->get_type_id() == String(tid)) { return true; }
							}
							return false;
						};
						return check(cur_node->get_spawn_modules())
							|| check(cur_node->get_update_modules())
							|| check(cur_node->get_render_modules());
					};

					module_menu->clear();
					if (add_section == 0) {
						module_menu->add_item("Position",         5);
						module_menu->add_item("Lifetime",         2);
						module_menu->add_item("Spawn Rate",       0);
						module_menu->add_item("Initial Velocity", 1);
						// Grey out already-present modules
						if (node_has(GVSModulePosition::TYPE_ID))        { module_menu->set_item_disabled(0, true); }
						if (node_has(GVSModuleLifetime::TYPE_ID))         { module_menu->set_item_disabled(1, true); }
						if (node_has(GVSModuleSpawnRate::TYPE_ID))        { module_menu->set_item_disabled(2, true); }
						if (node_has(GVSModuleInitialVelocity::TYPE_ID))  { module_menu->set_item_disabled(3, true); }
					} else if (add_section == 1) {
						module_menu->add_item("Initial Velocity", 1);
						if (node_has(GVSModuleInitialVelocity::TYPE_ID))  { module_menu->set_item_disabled(0, true); }
					} else {
						module_menu->add_item("Render Material", 3);
						module_menu->add_item("Facing",          4);
						if (node_has(GVSModuleRenderMaterial::TYPE_ID))   { module_menu->set_item_disabled(0, true); }
						if (node_has(GVSModuleFacing::TYPE_ID))           { module_menu->set_item_disabled(1, true); }
					}

					module_menu->set_position(get_screen_position() + mb->get_position());
					module_menu->popup();
					accept_event();
					return;
				}

				const int idx = _node_at_screen(mb->get_position());
				for (int i = 0; i < nodes.size(); i++) {
					nodes[i]->set_selected(false);
				}
				if (idx >= 0) {
					nodes[idx]->set_selected(true);
					dragging_node    = idx;
					drag_node_offset = mb->get_position() - _canvas_to_screen(nodes[idx]->get_canvas_pos());
					emit_signal(SNAME("node_selected"), nodes[idx]);
					accept_event();
				} else {
					emit_signal(SNAME("node_selected"), Ref<GVSEmitterNode>());
				}
				queue_redraw();
			} else if (mb->get_button_index() == MouseButton::RIGHT) {
				const int idx = _node_at_screen(mb->get_position());
				if (idx >= 0) {
					context_menu_node_idx = idx;
					context_menu->set_position(get_screen_position() + mb->get_position());
					context_menu->popup();
					accept_event();
				}
			} else if (mb->get_button_index() == MouseButton::MIDDLE) {
				panning          = true;
				pan_start_mouse  = mb->get_position();
				pan_start_offset = scroll_offset;
				accept_event();
			}
		} else {
			if (mb->get_button_index() == MouseButton::LEFT) {
				if (dragging_node >= 0) {
					_save();
				}
				dragging_node = -1;
				accept_event();
			} else if (mb->get_button_index() == MouseButton::MIDDLE) {
				panning = false;
				accept_event();
			}
		}
	}

	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_valid()) {
		if (dragging_node >= 0) {
			const Vector2 new_screen = mm->get_position() - drag_node_offset;
			nodes[dragging_node]->set_canvas_pos(_screen_to_canvas(new_screen));
			queue_redraw();
			accept_event();
		} else if (panning) {
			scroll_offset = pan_start_offset + (mm->get_position() - pan_start_mouse);
			queue_redraw();
			accept_event();
		}
	}

	Ref<InputEventKey> key = p_event;
	if (key.is_valid() && key->is_pressed() && !key->is_echo()) {
		if (key->get_keycode() == Key::KEY_DELETE || key->get_keycode() == Key::BACKSPACE) {
			for (int i = 0; i < nodes.size(); i++) {
				if (nodes[i]->is_selected()) {
					_delete_node(i);
					accept_event();
					break;
				}
			}
		}
	}
}

// Crea un nuevo nodo emisor centrado en la vista actual y lo añade al recurso
void GVSEmittersPanel::_on_add_node_pressed() {
	Ref<GVSEmitterNode> node = GVSEmitterNode::create("Emitter");
	const Vector2 center_screen(get_size().x * 0.5f, get_size().y * 0.5f);
	const Vector2 canvas_pos = _screen_to_canvas(center_screen)
			- Vector2(GVSEmitterNode::NODE_WIDTH * 0.5f, node->get_node_height() * 0.5f);
	node->set_canvas_pos(canvas_pos);
	nodes.push_back(node);

	if (current_resource.is_valid()) {
		TypedArray<GVSEmitterNode> res_nodes = current_resource->get_nodes();
		res_nodes.push_back(node);
		current_resource->set_nodes(res_nodes);
		_save();
	}

	queue_redraw();
}

void GVSEmittersPanel::_delete_node(int p_index) {
	if (p_index < 0 || p_index >= nodes.size()) {
		return;
	}
	nodes.remove_at(p_index);

	if (current_resource.is_valid()) {
		TypedArray<GVSEmitterNode> res_nodes = current_resource->get_nodes();
		res_nodes.remove_at(p_index);
		current_resource->set_nodes(res_nodes);
		_save();
	}

	emit_signal(SNAME("node_selected"), Ref<GVSEmitterNode>());
	queue_redraw();
}

void GVSEmittersPanel::_on_context_menu_id_pressed(int p_id) {
	if (p_id == 0) {
		_delete_node(context_menu_node_idx);
	}
	context_menu_node_idx = -1;
}

void GVSEmittersPanel::_on_module_menu_id_pressed(int p_type_id) {
	if (module_menu_node_idx < 0 || module_menu_node_idx >= nodes.size()) {
		return;
	}
	String type_id;
	switch (p_type_id) {
		case 0: type_id = GVSModuleSpawnRate::TYPE_ID;      break;
		case 1: type_id = GVSModuleInitialVelocity::TYPE_ID; break;
		case 2: type_id = GVSModuleLifetime::TYPE_ID;         break;
		case 3: type_id = GVSModuleRenderMaterial::TYPE_ID; break;
		case 4: type_id = GVSModuleFacing::TYPE_ID;    break;
		case 5: type_id = GVSModulePosition::TYPE_ID; break;
		default: module_menu_node_idx = -1; module_menu_section = -1; return;
	}

	Ref<GVSModule> mod = GVSModule::create(type_id);
	Ref<GVSEmitterNode> node = nodes[module_menu_node_idx];

	switch (module_menu_section) {
		case 0: node->add_spawn_module(mod);  break;
		case 1: node->add_update_module(mod); break;
		case 2: node->add_render_module(mod); break;
		default: break;
	}

	if (current_resource.is_valid()) {
		_save();
	}
	queue_redraw();
	emit_signal(SNAME("node_selected"), node);
	emit_signal(SNAME("node_changed"));
	module_menu_node_idx = -1;
	module_menu_section  = -1;
}

// Guarda el recurso en disco si tiene ruta asignada
void GVSEmittersPanel::_save() {
	if (current_resource.is_valid() && !current_resource->get_path().is_empty()) {
		ResourceSaver::save(current_resource, current_resource->get_path());
	}
}

// Sincroniza la lista de nodos del canvas con los del recurso cargado
void GVSEmittersPanel::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
	nodes.clear();

	if (current_resource.is_valid()) {
		TypedArray<GVSEmitterNode> saved = current_resource->get_nodes();
		for (int i = 0; i < saved.size(); i++) {
			Ref<GVSEmitterNode> node = saved[i];
			if (node.is_valid()) {
				GVSEmitterNode::ensure_id_counter(node->get_id());
				nodes.push_back(node);
			}
		}
	}

	queue_redraw();
}

GVSEmittersPanel::GVSEmittersPanel() {
	set_clip_contents(true);
	set_mouse_filter(MOUSE_FILTER_STOP);
	set_focus_mode(FOCUS_CLICK);

	// Barra de herramientas superpuesta dentro del canvas
	overlay_bar = memnew(MarginContainer);
	overlay_bar->add_theme_constant_override("margin_left", 8);
	overlay_bar->add_theme_constant_override("margin_right", 8);
	overlay_bar->add_theme_constant_override("margin_top", 6);
	overlay_bar->add_theme_constant_override("margin_bottom", 6);
	// MOUSE_FILTER_PASS para que el canvas reciba eventos en zonas sin botón
	overlay_bar->set_mouse_filter(MOUSE_FILTER_PASS);
	// Ligera transparencia para que se note que está dentro del canvas
	overlay_bar->set_modulate(Color(1.0f, 1.0f, 1.0f, 0.82f));
	add_child(overlay_bar);

	HBoxContainer *btn_row = memnew(HBoxContainer);
	overlay_bar->add_child(btn_row);

	Button *btn_add = memnew(Button);
	btn_add->set_text("Add Node");
	btn_add->set_theme_type_variation(SceneStringName(FlatButton));
	btn_add->connect("pressed", callable_mp(this, &GVSEmittersPanel::_on_add_node_pressed));
	btn_row->add_child(btn_add);

	context_menu = memnew(PopupMenu);
	context_menu->add_item("Delete Node", 0);
	context_menu->connect("id_pressed", callable_mp(this, &GVSEmittersPanel::_on_context_menu_id_pressed));
	add_child(context_menu);

	module_menu = memnew(PopupMenu);
	module_menu->set_name("ModuleMenu");
	module_menu->connect("id_pressed", callable_mp(this, &GVSEmittersPanel::_on_module_menu_id_pressed));
	add_child(module_menu);
}

}
