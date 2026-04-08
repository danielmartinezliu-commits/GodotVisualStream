#include "gvs_inspector_panel.h"

namespace GodotVisualStream {

	void GVSInspectorPanel::_bind_methods() {
	}

	void GVSInspectorPanel::_notification(int p_what) {
		if (p_what == NOTIFICATION_READY) {
			_build_ui();
		}
	}

	void GVSInspectorPanel::_build_ui() {
		if (placeholder_label) {
			return;
		}

		placeholder_label = memnew(Label);
		placeholder_label->set_text("Inspector");
		placeholder_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
		placeholder_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
		placeholder_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		placeholder_label->set_v_size_flags(Control::SIZE_EXPAND_FILL);
		add_child(placeholder_label);
	}

	void GVSInspectorPanel::load_resource(const Ref<GVSResource> &p_resource) {
		current_resource = p_resource;
	}

	GVSInspectorPanel::GVSInspectorPanel() {
		set_custom_minimum_size(Size2(220, 0));
	}

}
