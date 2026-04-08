#include "gvs_editor_window.h"

void GVSEditorWindow::_bind_methods() {
}

void GVSEditorWindow::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		_build_ui();
	}
}

void GVSEditorWindow::_build_ui() {
	if (toolbar) return;

	VBoxContainer *vb = memnew(VBoxContainer);
	add_child(vb);

	toolbar = memnew(HBoxContainer);
	vb->add_child(toolbar);

	label_filename = memnew(Label);
	label_filename->set_text("No hay recurso cargado");
	toolbar->add_child(label_filename);

	canvas = memnew(Control);
	canvas->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	canvas->set_custom_minimum_size(Size2(0, 400));
	vb->add_child(canvas);
}

void GVSEditorWindow::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
	if (label_filename && current_resource.is_valid()) {
		label_filename->set_text("Editando: " + current_resource->get_path());
		set_title("GVS Editor — " + current_resource->get_path().get_file());
	}
}

Ref<GVSResource> GVSEditorWindow::get_current_resource() const {
	return current_resource;
}

GVSEditorWindow::GVSEditorWindow() {
	set_title("GVS Editor");
	set_exclusive(false);
	set_ok_button_text("Cerrar");
}
