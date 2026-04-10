#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/control.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/split_container.h"
#include "gvs_resource.h"
#include "panels/gvs_particle_preview.h"
#include "panels/gvs_variables_panel.h"
#include "panels/gvs_emitters_panel.h"
#include "panels/gvs_timeline_panel.h"
#include "panels/gvs_inspector_panel.h"

namespace GodotVisualStream {

// GVSEditorWindow — Ventana flotante del editor .gvs.
// Se muestra como popup al hacer doble clic en un .gvs en el FileSystem.
// Hereda de AcceptDialog para obtener barra de título, botón cerrar y
// el mismo look que el resto de ventanas del editor de Godot.
//
// Layout:
//   ┌─────────────────┬──────────────────────────┬───────────────┐
//   │ particle_preview│     emitters_panel        │               │
//   │                 │     (zona más amplia)     │ inspector     │
//   ├─────────────────┼──────────────────────────┤ _panel        │
//   │ variables_panel │     timeline_panel        │               │
//   └─────────────────┴──────────────────────────┴───────────────┘
class GVSEditorWindow : public AcceptDialog {
	GDCLASS(GVSEditorWindow, AcceptDialog);

	Ref<GVSResource> current_resource;

	// Splits
	HSplitContainer *main_split         = nullptr; // izquierda | centro+derecha
	HSplitContainer *center_right_split = nullptr; // centro | derecha
	VSplitContainer *left_split         = nullptr; // preview | variables
	VSplitContainer *center_split       = nullptr; // emitters | timeline

	// Paneles
	GVSParticlePreview *particle_preview = nullptr;
	GVSVariablesPanel  *variables_panel  = nullptr;
	GVSEmittersPanel   *emitters_panel   = nullptr;
	GVSTimelinePanel   *timeline_panel   = nullptr;
	GVSInspectorPanel  *inspector_panel  = nullptr;

	void _build_ui();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void load_resource(const Ref<GVSResource> &p_resource);
	Ref<GVSResource> get_current_resource() const;

	GVSEditorWindow();
};

}
