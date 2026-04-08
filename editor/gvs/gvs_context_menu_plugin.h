#pragma once

#include "editor/inspector/editor_context_menu_plugin.h"
#include "gvs_resource.h"

class EditorFileDialog;

class GVSContextMenuPlugin : public EditorContextMenuPlugin {
	GDCLASS(GVSContextMenuPlugin, EditorContextMenuPlugin);

	EditorFileDialog *save_dialog = nullptr;

	void _show_create_dialog(Array p_paths);
	void _on_file_selected(const String &p_path);

protected:
	static void _bind_methods();

public:
	void setup(EditorFileDialog *p_dialog);

	virtual void get_options(const Vector<String> &p_paths) override;
};
