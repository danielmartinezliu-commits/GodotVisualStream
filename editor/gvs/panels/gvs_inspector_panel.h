#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/tab_container.h"
#include "editor/gvs/gvs_resource.h"
#include "editor/gvs/nodes/gvs_emitter_node.h"

namespace GodotVisualStream {

	class GVSInspectorPanel : public PanelContainer {
		GDCLASS(GVSInspectorPanel, PanelContainer);

		Ref<GVSResource>    current_resource;
		Ref<GVSEmitterNode> inspected_node;

		VBoxContainer *props_container = nullptr;
		Label         *id_label        = nullptr;
		LineEdit      *name_edit       = nullptr;
		Label         *pos_label       = nullptr;
		Label         *empty_label     = nullptr;

		void _build_ui();
		void _on_name_changed(const String &p_text);

	protected:
		static void _bind_methods();
		void _notification(int p_what);

	public:
		void load_resource(const Ref<GVSResource> &p_resource);
		void inspect_node(Ref<GVSEmitterNode> p_node);

		GVSInspectorPanel();
	};

}