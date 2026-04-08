#include "gvs_context_menu_plugin.h"

#include "core/io/resource_saver.h"
#include "core/object/callable_mp.h"
#include "editor/file_system/editor_file_system.h"
#include "editor/gui/editor_file_dialog.h"

namespace GodotVisualStream {

	void GVSContextMenuPlugin::_bind_methods() {
	}

	void GVSContextMenuPlugin::setup(EditorFileDialog *p_dialog) {
		save_dialog = p_dialog;
		save_dialog->connect("file_selected", callable_mp(this, &GVSContextMenuPlugin::_on_file_selected));
	}

	void GVSContextMenuPlugin::get_options(const Vector<String> &p_paths) {
		add_context_menu_item(
				"GVS File...",
				callable_mp(this, &GVSContextMenuPlugin::_show_create_dialog),
				Ref<Texture2D>()
		);
	}

	void GVSContextMenuPlugin::_show_create_dialog(Array p_paths) {
		ERR_FAIL_NULL_MSG(save_dialog, "GVSContextMenuPlugin: setup() no fue llamado.");
		ERR_FAIL_COND_MSG(p_paths.is_empty(), "GVSContextMenuPlugin: el callback no recibió ninguna carpeta.");

		String folder = p_paths[0];
		save_dialog->set_current_dir(folder);
		save_dialog->set_current_file("new_file.gvs");
		save_dialog->popup_file_dialog();
	}

	void GVSContextMenuPlugin::_on_file_selected(const String &p_path) {
		Ref<GVSResource> resource;
		resource.instantiate();

		Error err = ResourceSaver::save(resource, p_path);
		ERR_FAIL_COND_MSG(err != OK,
				vformat("GVSContextMenuPlugin: no se pudo guardar '%s' (error %d).", p_path, err));

		EditorFileSystem::get_singleton()->scan_changes();
	}

}
