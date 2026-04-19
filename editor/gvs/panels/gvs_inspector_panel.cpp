#include "gvs_inspector_panel.h"

#include "core/object/callable_mp.h"
#include "core/object/class_db.h"
#include "scene/resources/style_box_flat.h"

namespace GodotVisualStream {

// =============================================================================
// GVSPropertyDropZone
//
// Widget reutilizable para una propiedad numérica que puede recibir una variable
// del panel de Variables mediante drag & drop.
//
// Tiene dos estados visuales:
//   - SIN variable vinculada → muestra un SpinBox editable con el valor actual.
//   - CON variable vinculada → muestra un "chip" azul con el nombre de la variable
//                              y un botón "x" para desvincularla.
//
// Señales que emite:
//   "value_changed"   (float value)   → el usuario cambió el valor en el SpinBox.
//   "binding_changed" (String var)    → se vinculó o desvinculó una variable
//                                       (string vacía = desvinculada).
// =============================================================================

void GVSPropertyDropZone::_bind_methods() {
	ADD_SIGNAL(MethodInfo("value_changed",   PropertyInfo(Variant::FLOAT,  "value")));
	ADD_SIGNAL(MethodInfo("binding_changed", PropertyInfo(Variant::STRING, "var_name")));

	// Estos métodos se pasan como Callable a set_drag_forwarding() en el SpinBox,
	// permitiendo que los eventos de drop lleguen al drop zone aunque el SpinBox
	// sea el control que está bajo el cursor.
	ClassDB::bind_method(D_METHOD("_can_drop_fwd", "pos", "data"), &GVSPropertyDropZone::_can_drop_fwd);
	ClassDB::bind_method(D_METHOD("_drop_fwd",     "pos", "data"), &GVSPropertyDropZone::_drop_fwd);
}

// Configura el estado inicial del widget. Llamado cada vez que el inspector
// carga un nodo diferente o cambia la propiedad.
void GVSPropertyDropZone::setup(float p_value, const String &p_bound_var, int p_accepted_var_type) {
	value             = p_value;
	bound_var         = p_bound_var;
	accepted_var_type = p_accepted_var_type;
	_rebuild_display();
}

// Destruye todos los hijos actuales y reconstruye la UI según el estado:
// sin variable → SpinBox; con variable → chip + botón de desvinculación.
void GVSPropertyDropZone::_rebuild_display() {
	for (int i = get_child_count() - 1; i >= 0; i--) {
		Node *child = get_child(i);
		remove_child(child);
		child->queue_free();
	}
	spin_box   = nullptr;
	var_badge  = nullptr;
	unbind_btn = nullptr;

	// Container defaul a MOUSE_FILTER_PASS; necesitamos STOP para que el sistema
	// de drag & drop de Godot (_gui_drop) llegue a este control al recorrer padres.
	set_mouse_filter(Control::MOUSE_FILTER_STOP);

	if (bound_var.is_empty()) {
		// Estado: sin variable → SpinBox editable.
		spin_box = memnew(SpinBox);
		spin_box->set_min(-1e6);
		spin_box->set_max(1e6);
		spin_box->set_step(0.01);
		spin_box->set_value(value);
		spin_box->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		spin_box->connect("value_changed",
				callable_mp(this, &GVSPropertyDropZone::_on_spin_changed));
		// El SpinBox tiene MOUSE_FILTER_STOP y absorbería el drop antes de que
		// llegue al drop zone. set_drag_forwarding lo redirige explícitamente.
		spin_box->set_drag_forwarding(
				Callable(),
				callable_mp(this, &GVSPropertyDropZone::_can_drop_fwd),
				callable_mp(this, &GVSPropertyDropZone::_drop_fwd));
		add_child(spin_box);
	} else {
		// Estado: con variable → chip con nombre + botón de desvinculación.
		var_badge = memnew(Label);
		var_badge->set_text(bound_var);
		var_badge->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		var_badge->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
		var_badge->add_theme_color_override("font_color", Color(0.4f, 0.8f, 1.0f));

		Ref<StyleBoxFlat> chip_style;
		chip_style.instantiate();
		chip_style->set_bg_color(Color(0.4f, 0.8f, 1.0f, 0.12f));
		chip_style->set_border_color(Color(0.4f, 0.8f, 1.0f, 0.5f));
		chip_style->set_border_width_all(1);
		chip_style->set_content_margin_all(3);
		var_badge->add_theme_style_override("normal", chip_style);
		add_child(var_badge);

		unbind_btn = memnew(Button);
		unbind_btn->set_text("x");
		unbind_btn->set_flat(true);
		unbind_btn->set_custom_minimum_size(Size2(20, 0));
		unbind_btn->connect("pressed",
				callable_mp(this, &GVSPropertyDropZone::_on_unbind_pressed));
		add_child(unbind_btn);
	}
}

// El usuario movió el SpinBox → guarda el nuevo valor y notifica al inspector.
void GVSPropertyDropZone::_on_spin_changed(double p_val) {
	value = float(p_val);
	emit_signal(SNAME("value_changed"), value);
}

// El usuario pulsó "x" → quita la vinculación y vuelve al estado SpinBox.
void GVSPropertyDropZone::_on_unbind_pressed() {
	bound_var = "";
	_rebuild_display();
	emit_signal(SNAME("binding_changed"), String(""));
}

// Puentes que el SpinBox usa como forwarding callables para delegar el drop aquí.
bool GVSPropertyDropZone::_can_drop_fwd(const Point2 &p_pos, const Variant &p_data) {
	return can_drop_data(p_pos, p_data);
}
void GVSPropertyDropZone::_drop_fwd(const Point2 &p_pos, const Variant &p_data) {
	drop_data(p_pos, p_data);
}

// Devuelve true solo si el dato arrastrado es una variable GVS del tipo aceptado.
// Godot llama a este método al recorrer la jerarquía de controles bajo el cursor.
bool GVSPropertyDropZone::can_drop_data(const Point2 &p_point, const Variant &p_data) const {
	if (p_data.get_type() != Variant::DICTIONARY) {
		return false;
	}
	Dictionary d = p_data;
	if (!d.has("type") || String(d["type"]) != "gvs_variable") {
		return false;
	}
	// Solo acepta el tipo de variable para el que fue configurado (p.ej. solo floats).
	if (!d.has("var_type") || int(d["var_type"]) != accepted_var_type) {
		return false;
	}
	return true;
}

// El usuario soltó una variable válida aquí → vincula la variable y reconstruye la UI.
void GVSPropertyDropZone::drop_data(const Point2 &p_point, const Variant &p_data) {
	Dictionary d = p_data;
	bound_var = String(d["name"]);
	_rebuild_display();
	emit_signal(SNAME("binding_changed"), bound_var);
}

// =============================================================================
// GVSInspectorPanel
//
// Panel derecho del editor GVS. Muestra las propiedades del GVSEmitterNode
// seleccionado en el canvas de emitters.
//
// Layout (dentro de una pestaña "Inspector"):
//   - Si no hay nodo seleccionado → Label "No node selected".
//   - Si hay nodo → grid de 2 columnas (clave | valor):
//       Name     → LineEdit editable (cambia el título del nodo en el canvas)
//       ID       → Label read-only
//       Position → Label read-only (posición en el canvas)
//       Lifetime → GVSPropertyDropZone (SpinBox o chip de variable)
//
// Señales que emite:
//   "node_changed" → cualquier propiedad cambió; el editor window lo escucha
//                    para hacer auto-save y forzar redibujado del canvas.
// =============================================================================

void GVSInspectorPanel::_bind_methods() {
	ADD_SIGNAL(MethodInfo("node_changed"));
}

void GVSInspectorPanel::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		_build_ui();
		// Aplica un borde sutil igual al de los otros paneles del editor.
		Ref<StyleBoxFlat> base = get_theme_stylebox(SNAME("panel"), SNAME("Tree"));
		if (base.is_valid()) {
			Ref<StyleBoxFlat> bordered = base->duplicate();
			bordered->set_border_width_all(1);
			bordered->set_border_color(base->get_bg_color().lightened(0.35f));
			add_theme_style_override(SNAME("panel"), bordered);
		}
	}
}

// Construye el grid de propiedades dentro del contenedor dado.
// Cada fila tiene un Label (clave, gris) a la izquierda y un widget (valor) a la derecha.
void GVSInspectorPanel::_build_props_grid(VBoxContainer *p_container) {
	MarginContainer *grid_margin = memnew(MarginContainer);
	grid_margin->add_theme_constant_override("margin_left",   12);
	grid_margin->add_theme_constant_override("margin_right",   6);
	grid_margin->add_theme_constant_override("margin_top",     4);
	grid_margin->add_theme_constant_override("margin_bottom",  4);
	p_container->add_child(grid_margin);

	GridContainer *grid = memnew(GridContainer);
	grid->set_columns(2);
	grid->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	grid->add_theme_constant_override("h_separation", 8);
	grid->add_theme_constant_override("v_separation", 6);
	grid_margin->add_child(grid);

	// Lambda auxiliar para crear los labels de clave uniformes.
	auto make_key = [](const String &p_text) -> Label * {
		Label *lbl = memnew(Label);
		lbl->set_text(p_text);
		lbl->set_custom_minimum_size(Size2(70, 0));
		lbl->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
		lbl->add_theme_color_override("font_color", Color(0.65f, 0.65f, 0.65f));
		return lbl;
	};

	// Name: editable; cambiar el nombre actualiza el título del nodo en el canvas.
	grid->add_child(make_key("Name"));
	name_edit = memnew(LineEdit);
	name_edit->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	name_edit->connect("text_changed", callable_mp(this, &GVSInspectorPanel::_on_name_changed));
	grid->add_child(name_edit);

	// ID: solo lectura; identificador numérico único del nodo.
	grid->add_child(make_key("ID"));
	id_label = memnew(Label);
	id_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	id_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	grid->add_child(id_label);

	// Position: solo lectura; coordenadas del nodo en el canvas 2D del editor.
	grid->add_child(make_key("Position"));
	pos_label = memnew(Label);
	pos_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	pos_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	grid->add_child(pos_label);

	// Lifetime: tiempo de vida del emisor. Acepta un valor directo (SpinBox)
	// o una variable float arrastrada desde el panel de Variables (chip azul).
	grid->add_child(make_key("Lifetime"));
	lifetime_drop = memnew(GVSPropertyDropZone);
	lifetime_drop->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	lifetime_drop->connect("value_changed",
			callable_mp(this, &GVSInspectorPanel::_on_lifetime_value_changed));
	lifetime_drop->connect("binding_changed",
			callable_mp(this, &GVSInspectorPanel::_on_lifetime_binding_changed));
	grid->add_child(lifetime_drop);
}

// Construye el árbol completo de UI la primera vez que el panel entra en escena.
void GVSInspectorPanel::_build_ui() {
	if (props_container) {
		return; // ya construido
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

	// Placeholder visible cuando ningún nodo está seleccionado.
	empty_label = memnew(Label);
	empty_label->set_text("No node selected");
	empty_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	empty_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	empty_label->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	empty_label->add_theme_color_override("font_color", Color(0.55f, 0.55f, 0.55f));
	tab_root->add_child(empty_label);

	// Contenedor de propiedades: oculto hasta que se selecciona un nodo.
	props_container = memnew(VBoxContainer);
	props_container->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tab_root->add_child(props_container);

	_build_props_grid(props_container);

	props_container->hide();
}

// El usuario editó el nombre → actualiza el nodo y dispara auto-save + redibujado.
void GVSInspectorPanel::_on_name_changed(const String &p_text) {
	if (!inspected_node.is_valid()) {
		return;
	}
	inspected_node->set_title(p_text);
	emit_signal(SNAME("node_changed"));
}

// El usuario cambió el valor del SpinBox de Lifetime.
void GVSInspectorPanel::_on_lifetime_value_changed(float p_val) {
	if (!inspected_node.is_valid()) {
		return;
	}
	inspected_node->set_lifetime(p_val);
	emit_signal(SNAME("node_changed"));
}

// El usuario vinculó o desvinculó una variable en el drop zone de Lifetime.
// p_var = nombre de la variable (vacío si se desvinculó).
void GVSInspectorPanel::_on_lifetime_binding_changed(const String &p_var) {
	if (!inspected_node.is_valid()) {
		return;
	}
	inspected_node->set_lifetime_var(p_var);
	emit_signal(SNAME("node_changed"));
}

// Carga un nodo en el inspector: rellena todos los widgets con sus datos actuales.
// Llamado desde GVSEditorWindow cuando el usuario selecciona un nodo en el canvas.
void GVSInspectorPanel::inspect_node(Ref<GVSEmitterNode> p_node) {
	if (!props_container) {
		return;
	}

	inspected_node = p_node;

	if (!p_node.is_valid()) {
		// Deselección: oculta propiedades y muestra el placeholder.
		props_container->hide();
		empty_label->show();
		return;
	}

	id_label->set_text(itos(p_node->get_id()));

	// Desconecta la señal antes de set_text para no disparar _on_name_changed
	// con el texto del nodo anterior mientras se actualiza el widget.
	name_edit->disconnect("text_changed", callable_mp(this, &GVSInspectorPanel::_on_name_changed));
	name_edit->set_text(p_node->get_title());
	name_edit->connect("text_changed", callable_mp(this, &GVSInspectorPanel::_on_name_changed));

	pos_label->set_text(vformat("%.1f,  %.1f",
			p_node->get_canvas_pos().x, p_node->get_canvas_pos().y));

	// Inicializa el drop zone con el valor actual y la variable vinculada (si hay).
	lifetime_drop->setup(p_node->get_lifetime(), p_node->get_lifetime_var(), GVSVariable::VAR_FLOAT);

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
