#pragma once

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "gvs_resource.h"

// ─── Loader ──────────────────────────────────────────────────────────────────
// Lee archivos .gvs del disco y devuelve un GVSResource.
class GVSFormatLoader : public ResourceFormatLoader {
	GDCLASS(GVSFormatLoader, ResourceFormatLoader);

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

// ─── Saver ───────────────────────────────────────────────────────────────────
// Escribe un GVSResource al disco en formato .gvs (JSON legible).
class GVSFormatSaver : public ResourceFormatSaver {
	GDCLASS(GVSFormatSaver, ResourceFormatSaver);

public:
	virtual Error save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) override;
	virtual void get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const override;
	virtual bool recognize(const Ref<Resource> &p_resource) const override;
};
