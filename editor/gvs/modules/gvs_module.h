#pragma once

#include "core/io/resource.h"
#include "core/math/color.h"
#include "core/string/ustring.h"
#include "core/variant/dictionary.h"

class GPUParticles3D;
class ParticleProcessMaterial;

namespace GodotVisualStream {

class GVSModule : public Resource {
	GDCLASS(GVSModule, Resource);

	String type_id;
	bool enabled = true;

protected:
	static void _bind_methods();

public:
	static constexpr float BOX_HEIGHT = 20.0f;

	void set_type_id(const String &p_type) { type_id = p_type; }
	String get_type_id() const { return type_id; }

	void set_enabled(bool p_enabled) { enabled = p_enabled; }
	bool get_enabled() const { return enabled; }

	virtual String get_display_name() const { return type_id; }
	virtual Color  get_category_color() const { return Color(0.55f, 0.55f, 0.55f); }

	// Subclases sobreescriben estos para guardar/cargar sus propiedades extra
	virtual void serialize_to(Dictionary &p_dict) const {}
	virtual void deserialize_from(const Dictionary &p_dict) {}

	// --- Inspector integration ---
	// Describes a single editable property shown in the inspector panel.
	struct InspectorProp {
		String label;
		String prop_name;
		// Numeric (float / int)
		bool   is_int    = false;
		double value     = 0.0;
		String bound_var;
		double min_val   = 0.0;
		double max_val   = 1e6;
		double step      = 0.01;
		// Color
		bool  is_color   = false;
		Color color_value;
		// Bool
		bool  is_bool    = false;
		bool  bool_value = false;
		// Resource (EditorResourcePicker)
		bool         is_resource   = false;
		String       resource_type;          // e.g. "Material"
		Ref<Resource> resource_value;
	};
	virtual Vector<InspectorProp> get_inspector_props() const { return {}; }
	virtual void set_prop_value(const String &p_name, double p_val) {}
	virtual void set_prop_binding(const String &p_name, const String &p_var) {}
	virtual void set_prop_color(const String &p_name, const Color &p_color) {}
	virtual void set_prop_bool(const String &p_name, bool p_val) {}
	virtual void set_prop_resource(const String &p_name, const Ref<Resource> &p_res) {}

	// Called from GVSEmitterNode::create_gpu_particles() to apply module data to the runtime node.
	virtual void apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const {}

	static Ref<GVSModule> create(const String &p_type_id);
};

}
