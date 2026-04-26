#pragma once

#include "gvs_module.h"

namespace GodotVisualStream {

class GVSModuleLifetime : public GVSModule {
	GDCLASS(GVSModuleLifetime, GVSModule);

	float lifetime = 5.0f;
	String lifetime_var;

protected:
	static void _bind_methods();

public:
	static const char *TYPE_ID;

	void set_lifetime(float p_val) { lifetime = p_val; }
	float get_lifetime() const { return lifetime; }

	void set_lifetime_var(const String &p_var) { lifetime_var = p_var; }
	String get_lifetime_var() const { return lifetime_var; }

	String get_display_name() const override { return "Lifetime"; }
	Color  get_category_color() const override { return Color(0.95f, 0.65f, 0.2f); }

	void serialize_to(Dictionary &p_dict) const override;
	void deserialize_from(const Dictionary &p_dict) override;

	Vector<InspectorProp> get_inspector_props() const override;
	void set_prop_value(const String &p_name, double p_val) override;
	void set_prop_binding(const String &p_name, const String &p_var) override;

	void apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const override;
};

}
