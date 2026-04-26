#pragma once

#include "gvs_module.h"
#include "scene/resources/material.h"

namespace GodotVisualStream {

// Sets the process material on the GPUParticles3D node, restricted to
// ParticleProcessMaterial and ShaderMaterial — exactly what GPUParticles3D.process_material accepts.
class GVSModuleRenderMaterial : public GVSModule {
	GDCLASS(GVSModuleRenderMaterial, GVSModule);

	Ref<Material> process_material;

protected:
	static void _bind_methods();

public:
	static const char *TYPE_ID;

	void set_process_material(const Ref<Material> &p_mat) { process_material = p_mat; }
	Ref<Material> get_process_material() const { return process_material; }

	String get_display_name()   const override { return "Render Material"; }
	Color  get_category_color() const override { return Color(0.75f, 0.45f, 0.85f); }

	void serialize_to(Dictionary &p_dict) const override;
	void deserialize_from(const Dictionary &p_dict) override;

	Vector<InspectorProp> get_inspector_props() const override;
	void set_prop_resource(const String &p_name, const Ref<Resource> &p_res) override;

	// Calls gpu->set_process_material(), replacing the default ParticleProcessMaterial.
	void apply_to_particles(GPUParticles3D *p_gpu, ParticleProcessMaterial *p_mat) const override;
};

}
