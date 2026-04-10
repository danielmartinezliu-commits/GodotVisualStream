#pragma once

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/menu_button.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/scroll_container.h"
#include "scene/gui/tab_container.h"
#include "editor/gvs/gvs_resource.h"

namespace GodotVisualStream {

class GVSVariablesPanel : public PanelContainer {
	GDCLASS(GVSVariablesPanel, PanelContainer);

public:
	enum VariableType {
		VAR_FLOAT = 0,
		VAR_INT   = 1,
		VAR_BOOL  = 2,
		VAR_COLOR = 3,
	};

	struct VariableEntry {
		String name;
		VariableType type = VAR_FLOAT;
	};

private:
	Ref<GVSResource> current_resource;

	TabContainer  *tab_container  = nullptr;
	VBoxContainer *variable_list  = nullptr;

	Vector<VariableEntry> variable_entries;

	void _build_ui();
	void _on_add_variable(int p_type);
	void _on_delete_variable(int p_index);
	void _on_variable_name_changed(const String &p_name, int p_index);
	void _rebuild_variable_list();

	static String _type_name(VariableType p_type);

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	void load_resource(const Ref<GVSResource> &p_resource);

	GVSVariablesPanel();
};

}
