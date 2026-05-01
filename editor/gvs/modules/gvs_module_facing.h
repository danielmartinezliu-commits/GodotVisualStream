#pragma once

#include "gvs_module.h"

namespace GodotVisualStream {

// Controls the billboard / facing mode of the particle draw mesh.
// 0 = Camera Plane   → BILLBOARD_ENABLED
// 1 = Camera Position → BILLBOARD_ENABLED  (same shader path in Godot 4)
// 2 = Velocity       → BILLBOARD_PARTICLES
class GVSModuleFacing : public GVSModule {
	GDCLASS(GVSModuleFacing, GVSModule);

	int facing_mode = 0;

protected:
	static void _bind_methods();

public:
	static const char *TYPE_ID;

	void set_facing_mode(int p_mode) { facing_mode = CLAMP(p_mode, 0, 2); }
	int  get_facing_mode() const { return facing_mode; }

	String get_display_name()   const override { return "Facing"; }
	Color  get_category_color() const override { return Color(0.30f, 0.75f, 0.80f); }

	void serialize_to(Dictionary &p_dict) const override;
	void deserialize_from(const Dictionary &p_dict) override;

	Vector<InspectorProp> get_inspector_props() const override;
	void set_prop_enum(const String &p_name, int p_idx) override;

	void apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const override;
};

}
