#pragma once

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "gvs_resource.h"
#include "nodes/gvs_variable.h"
#include "modules/gvs_module.h"

namespace GodotVisualStream {

	class GVSFormatLoader : public ResourceFormatLoader {
		GDCLASS(GVSFormatLoader, ResourceFormatLoader);

		static Ref<GVSEmitterNode> _parse_node(const Dictionary &p_dict);
		static Ref<GVSVariable>    _parse_variable(const Dictionary &p_dict);
		static Ref<GVSModule>      _parse_module(const Dictionary &p_dict);

	public:
		virtual Ref<Resource> load(
				const String &p_path,
				const String &p_original_path,
				Error *r_error,
				bool p_use_sub_threads,
				float *r_progress,
				CacheMode p_cache_mode) override;

		virtual void get_recognized_extensions(List<String> *p_extensions) const override;
		virtual bool handles_type(const String &p_type) const override;
		virtual String get_resource_type(const String &p_path) const override;
	};

	class GVSFormatSaver : public ResourceFormatSaver {
		GDCLASS(GVSFormatSaver, ResourceFormatSaver);

		static Dictionary _serialize_node(const Ref<GVSEmitterNode> &p_node);
		static Dictionary _serialize_variable(const Ref<GVSVariable> &p_var);
		static Dictionary _serialize_module(const Ref<GVSModule> &p_mod);

	public:
		virtual Error save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) override;
		virtual void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const override;
		virtual bool recognize(const Ref<Resource> &p_resource) const override;
	};

}
