#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/control.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/label.h"
#include "gvs_resource.h"

// GVSEditorWindow — Ventana flotante del editor .gvs.
// Se muestra como popup al hacer doble clic en un .gvs en el FileSystem.
// Hereda de AcceptDialog para obtener barra de título, botón cerrar y
// el mismo look que el resto de ventanas del editor de Godot.
class GVSEditorWindow : public AcceptDialog {
	GDCLASS(GVSEditorWindow, AcceptDialog);

	Ref<GVSResource> current_resource;

	HBoxContainer *toolbar = nullptr;
	Label *label_filename = nullptr;
	Control *canvas = nullptr;

	void _build_ui();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void load_resource(const Ref<GVSResource> &p_resource);
	Ref<GVSResource> get_current_resource() const;

	GVSEditorWindow();
};
