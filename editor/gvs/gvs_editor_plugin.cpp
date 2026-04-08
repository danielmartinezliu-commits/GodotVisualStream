#include "gvs_editor_plugin.h"

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "editor/editor_node.h"
#include "editor/gui/editor_file_dialog.h"

void GVSEditorPlugin::_bind_methods() {
}

bool GVSEditorPlugin::handles(Object *p_object) const {
	return p_object->is_class("GVSResource");
}

void GVSEditorPlugin::edit(Object *p_object) {
	GVSResource *res = Object::cast_to<GVSResource>(p_object);
	if (res) {
		editor_window->load_resource(Ref<GVSResource>(res));
		editor_window->popup_centered_ratio(0.7f);
	}
}

GVSEditorPlugin::GVSEditorPlugin() {

	editor_window = memnew(GVSEditorWindow);
	EditorNode::get_singleton()->get_gui_base()->add_child(editor_window);

	create_dialog = memnew(EditorFileDialog);
	create_dialog->set_file_mode(EditorFileDialog::FILE_MODE_SAVE_FILE);
	create_dialog->set_title("Crear nuevo archivo GVS");
	create_dialog->add_filter("*.gvs", "GVS File");
	EditorNode::get_singleton()->get_gui_base()->add_child(create_dialog);

	context_menu_plugin.instantiate();
	context_menu_plugin->setup(create_dialog);
	add_context_menu_plugin(EditorContextMenuPlugin::CONTEXT_SLOT_FILESYSTEM_CREATE, context_menu_plugin);

	format_loader.instantiate();
	ResourceLoader::add_resource_format_loader(format_loader);

	format_saver.instantiate();
	ResourceSaver::add_resource_format_saver(format_saver);
}

GVSEditorPlugin::~GVSEditorPlugin() {
	remove_context_menu_plugin(context_menu_plugin);

	ResourceLoader::remove_resource_format_loader(format_loader);
	format_loader.unref();

	ResourceSaver::remove_resource_format_saver(format_saver);
	format_saver.unref();
}
