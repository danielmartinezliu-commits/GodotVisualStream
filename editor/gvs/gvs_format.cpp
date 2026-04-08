#include "gvs_format.h"

#include "core/io/file_access.h"
#include "core/io/json.h"


Ref<Resource> GVSFormatLoader::load(
		const String &p_path,
		const String &p_original_path,
		Error *r_error,
		bool p_use_sub_threads,
		float *r_progress,
		CacheMode p_cache_mode) {

	Error err;
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ, &err);
	if (err != OK) {
		if (r_error) {
			*r_error = err;
		}
		return Ref<Resource>();
	}

	String text = f->get_as_text();
	f->close();

	Ref<GVSResource> resource;
	resource.instantiate();

	JSON json;
	err = json.parse(text);

	if (err != OK) {
		WARN_PRINT(vformat("GVSFormatLoader: error parsing '%s' at line %d: %s",
			p_path, json.get_error_line(), json.get_error_message()));
		if (r_error) { *r_error = OK; }
		return resource;
	}

	Variant result = json.get_data();

	if (err != OK) {
		if (r_error) {
			*r_error = OK;
		}
		return resource;
	}

	if (result.get_type() == Variant::DICTIONARY) {
		Dictionary d = result;

		if (d.has("version")) {
			resource->set_version(int(d["version"]));
		}
		if (d.has("graph_data") && d["graph_data"].get_type() == Variant::DICTIONARY) {
			resource->set_graph_data(d["graph_data"]);
		}
	}

	if (r_error) {
		*r_error = OK;
	}
	return resource;
}

void GVSFormatLoader::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("gvs");
}

bool GVSFormatLoader::handles_type(const String &p_type) const {
	return p_type == "GVSResource";
}

String GVSFormatLoader::get_resource_type(const String &p_path) const {
	if (p_path.get_extension().to_lower() == "gvs") {
		return "GVSResource";
	}
	return "";
}

Error GVSFormatSaver::save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	Ref<GVSResource> gvs = p_resource;
	ERR_FAIL_COND_V(gvs.is_null(), ERR_INVALID_PARAMETER);

	Dictionary root;
	root["version"] = gvs->get_version();
	root["graph_data"] = gvs->get_graph_data();

	String json_text = JSON::stringify(root, "\t", false);

	Error err;
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::WRITE, &err);
	ERR_FAIL_COND_V_MSG(err != OK, err, vformat("GVSFormatSaver: cannot open '%s' for writing.", p_path));

	f->store_string(json_text);
	f->close();

	return OK;
}

void GVSFormatSaver::get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const {
	if (recognize(p_resource)) {
		p_extensions->push_back("gvs");
	}
}

bool GVSFormatSaver::recognize(const Ref<Resource> &p_resource) const {
	return p_resource.is_valid() && p_resource->is_class("GVSResource");
}
