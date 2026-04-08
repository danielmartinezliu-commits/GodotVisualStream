#pragma once

#include "scene/gui/panel_container.h"
#include "scene/gui/label.h"
#include "editor/gvs/gvs_resource.h"

namespace GodotVisualStream {

	class GVSVariablesPanel : public PanelContainer {
		GDCLASS(GVSVariablesPanel, PanelContainer);

		Ref<GVSResource> current_resource;

		Label *placeholder_label = nullptr;

		void _build_ui();

	protected:
		static void _bind_methods();
		void _notification(int p_what);

	public:
		void load_resource(const Ref<GVSResource> &p_resource);

		GVSVariablesPanel();
	};

}
