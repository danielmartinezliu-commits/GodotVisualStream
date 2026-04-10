#include "gvs_timeline_panel.h"

#include "scene/gui/box_container.h"
#include "scene/resources/style_box_flat.h"

class VBoxContainer;

namespace GodotVisualStream {

void GVSTimelinePanel::_bind_methods() {
}

void GVSTimelinePanel::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		_build_ui();
		Ref<StyleBoxFlat> base = get_theme_stylebox(SNAME("panel"), SNAME("Tree"));
		if (base.is_valid()) {
			Ref<StyleBoxFlat> bordered = base->duplicate();
			bordered->set_border_width_all(1);
			bordered->set_border_color(base->get_bg_color().lightened(0.35f));
			add_theme_style_override(SNAME("panel"), bordered);
		}
	}
}

void GVSTimelinePanel::_build_ui() {
	if (placeholder_label) {
		return;
	}

	TabContainer *tabs = memnew(TabContainer);
	tabs->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tabs->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	add_child(tabs);

	VBoxContainer *tab_root = memnew(VBoxContainer);
	tab_root->set_name("Timeline");
	tab_root->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	tab_root->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	tabs->add_child(tab_root);

	placeholder_label = memnew(Label);
	placeholder_label->set_text("No timeline data");
	placeholder_label->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	placeholder_label->set_vertical_alignment(VERTICAL_ALIGNMENT_CENTER);
	placeholder_label->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	placeholder_label->add_theme_color_override("font_color", Color(0.55f, 0.55f, 0.55f));
	tab_root->add_child(placeholder_label);
}

void GVSTimelinePanel::load_resource(const Ref<GVSResource> &p_resource) {
	current_resource = p_resource;
}

GVSTimelinePanel::GVSTimelinePanel() {
	set_custom_minimum_size(Size2(300, 120));
	set_theme_type_variation("Tree");
}

}
