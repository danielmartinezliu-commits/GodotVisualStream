#include "gvs_timeline_panel.h"

namespace GodotVisualStream {

	void GVSTimelinePanel::_bind_methods() {
	}

	void GVSTimelinePanel::_notification(int p_what) {
		if (p_what == NOTIFICATION_READY) {
			_build_ui();
		}
	}

	void GVSTimelinePanel::_build_ui() {
		if (placeholder_label) {
			return;
		}

		placeholder_label = memnew(Label);
		placeholder_label->set_text("Timeline");
		placeholder_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
		placeholder_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
		placeholder_label->set_h_size_flags(Control::SIZE_EXPAND_FILL);
		placeholder_label->set_v_size_flags(Control::SIZE_EXPAND_FILL);
		add_child(placeholder_label);
	}

	void GVSTimelinePanel::load_resource(const Ref<GVSResource> &p_resource) {
		current_resource = p_resource;
	}

	GVSTimelinePanel::GVSTimelinePanel() {
		set_custom_minimum_size(Size2(300, 120));
	}

}
