#pragma once

#include "gvs_module.h"

namespace GodotVisualStream {

class GVSModuleInitialVelocity : public GVSModule {
	GDCLASS(GVSModuleInitialVelocity, GVSModule);

	float velocity_min = 0.0f;
	String velocity_min_var;
	float velocity_max = 5.0f;
	String velocity_max_var;

protected:
	static void _bind_methods();

public:
	static const char *TYPE_ID;

	void set_velocity_min(float p_v) { velocity_min = p_v; }
	float get_velocity_min() const { return velocity_min; }

	void set_velocity_min_var(const String &p_var) { velocity_min_var = p_var; }
	String get_velocity_min_var() const { return velocity_min_var; }

	void set_velocity_max(float p_v) { velocity_max = p_v; }
	float get_velocity_max() const { return velocity_max; }

	void set_velocity_max_var(const String &p_var) { velocity_max_var = p_var; }
	String get_velocity_max_var() const { return velocity_max_var; }

	String get_display_name() const override { return "Initial Velocity"; }
	Color  get_category_color() const override { return Color(0.3f, 0.7f, 1.0f); }

	void serialize_to(Dictionary &p_dict) const override;
	void deserialize_from(const Dictionary &p_dict) override;

	Vector<InspectorProp> get_inspector_props() const override;
	void set_prop_value(const String &p_name, double p_val) override;
	void set_prop_binding(const String &p_name, const String &p_var) override;

	void apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const override;
};

}
