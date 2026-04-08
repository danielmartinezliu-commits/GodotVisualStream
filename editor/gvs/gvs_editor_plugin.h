#pragma once

#include "editor/plugins/editor_plugin.h"
#include "gvs_context_menu_plugin.h"
#include "gvs_editor_window.h"
#include "gvs_format.h"
#include "gvs_resource.h"

class EditorFileDialog;

class GVSEditorPlugin : public EditorPlugin {
	GDCLASS(GVSEditorPlugin, EditorPlugin);

	GVSEditorWindow *editor_window = nullptr;
	EditorFileDialog *create_dialog = nullptr;

	Ref<GVSContextMenuPlugin> context_menu_plugin;

	Ref<GVSFormatLoader> format_loader;
	Ref<GVSFormatSaver> format_saver;

protected:
	static void _bind_methods();

public:
	virtual String get_plugin_name() const override { return "GVS"; }

	virtual bool handles(Object *p_object) const override;
	virtual void edit(Object *p_object) override;

	GVSEditorPlugin();
	~GVSEditorPlugin();
};
