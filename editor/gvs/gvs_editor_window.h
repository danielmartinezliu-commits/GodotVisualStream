#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/control.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/split_container.h"
#include "gvs_resource.h"
#include "nodes/gvs_particle_system.h"
#include "panels/gvs_particle_preview.h"
#include "panels/gvs_variables_panel.h"
#include "panels/gvs_emitters_panel.h"
#include "panels/gvs_timeline_panel.h"
#include "panels/gvs_inspector_panel.h"

namespace GodotVisualStream {

class GVSEditorWindow : public AcceptDialog {
	GDCLASS(GVSEditorWindow, AcceptDialog);

	Ref<GVSResource> current_resource;

	// Splits
	HSplitContainer *main_split         = nullptr;
	HSplitContainer *center_right_split = nullptr;
	VSplitContainer *left_split         = nullptr;
	VSplitContainer *center_split       = nullptr;

	// Paneles
	GVSParticlePreview *particle_preview = nullptr;
	GVSVariablesPanel  *variables_panel  = nullptr;
	GVSEmittersPanel   *emitters_panel   = nullptr;
	GVSTimelinePanel   *timeline_panel   = nullptr;
	GVSInspectorPanel  *inspector_panel  = nullptr;

	void _build_ui();
	void _on_graph_changed();
	void _on_add_to_scene_pressed();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void load_resource(const Ref<GVSResource> &p_resource);
	Ref<GVSResource> get_current_resource() const;

	GVSEditorWindow();
};

}
