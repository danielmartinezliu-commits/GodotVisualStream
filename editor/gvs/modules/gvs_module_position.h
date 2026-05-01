#pragma once

#include "gvs_module.h"

namespace GodotVisualStream {

class GVSModulePosition : public GVSModule {
	GDCLASS(GVSModulePosition, GVSModule);

	Vector3 offset;
	String  offset_binding;

protected:
	static void _bind_methods();

public:
	static const char *TYPE_ID;

	void    set_offset(const Vector3 &v) { offset = v; }
	Vector3 get_offset() const { return offset; }

	String get_display_name()   const override { return "Position"; }
	Color  get_category_color() const override { return Color(0.90f, 0.75f, 0.30f); }

	void serialize_to(Dictionary &p_dict) const override;
	void deserialize_from(const Dictionary &p_dict) override;

	Vector<InspectorProp> get_inspector_props() const override;
	void    set_prop_vector3(const String &p_name, const Vector3 &p_val) override;
	Vector3 get_prop_vector3(const String &p_name) const override;
	void    set_prop_binding(const String &p_name, const String &p_var) override;

	void apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const override;
};

}
