#pragma once

#include "gvs_module.h"

namespace GodotVisualStream {

class GVSModuleSpawnRate : public GVSModule {
	GDCLASS(GVSModuleSpawnRate, GVSModule);

	float rate = 10.0f;
	String rate_var;

protected:
	static void _bind_methods();

public:
	static const char *TYPE_ID;

	void set_rate(float p_rate) { rate = p_rate; }
	float get_rate() const { return rate; }

	void set_rate_var(const String &p_var) { rate_var = p_var; }
	String get_rate_var() const { return rate_var; }

	String get_display_name() const override { return "Spawn Rate"; }
	Color  get_category_color() const override { return Color(0.3f, 0.85f, 0.45f); }

	void serialize_to(Dictionary &p_dict) const override;
	void deserialize_from(const Dictionary &p_dict) override;

	Vector<InspectorProp> get_inspector_props() const override;
	void set_prop_value(const String &p_name, double p_val) override;
	void set_prop_binding(const String &p_name, const String &p_var) override;

	void apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const override;
};

}
