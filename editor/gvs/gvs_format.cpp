#include "gvs_format.h"

#include "core/io/file_access.h"
#include "core/io/json.h"

namespace GodotVisualStream {

Ref<Resource> GVSFormatLoader::load(const String &p_path, const String &p_original_path, Error *r_error,
		bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Error err;
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ, &err);
	if (err != OK) {
		if (r_error) { *r_error = err; }
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
	if (result.get_type() != Variant::DICTIONARY) {
		if (r_error) { *r_error = OK; }
		return resource;
	}

	Dictionary d = result;

	if (d.has("version")) {
		resource->set_version(int(d["version"]));
	}

	if (d.has("nodes") && d["nodes"].get_type() == Variant::ARRAY) {
		Array nodes_json = d["nodes"];
		TypedArray<GVSEmitterNode> nodes;
		for (int i = 0; i < nodes_json.size(); i++) {
			if (nodes_json[i].get_type() != Variant::DICTIONARY) {
				continue;
			}
			Dictionary nd = nodes_json[i];
			Ref<GVSEmitterNode> node;
			node.instantiate();
			if (nd.has("id")) {
				node->set_id(int(nd["id"]));
			}
			if (nd.has("title")) {
				node->set_title(nd["title"]);
			}
			if (nd.has("canvas_pos") && nd["canvas_pos"].get_type() == Variant::DICTIONARY) {
				Dictionary pos = nd["canvas_pos"];
				float x = pos.has("x") ? float(pos["x"]) : 0.0f;
				float y = pos.has("y") ? float(pos["y"]) : 0.0f;
				node->set_canvas_pos(Vector2(x, y));
			}
			nodes.push_back(node);
		}
		resource->set_nodes(nodes);
	}

	if (r_error) { *r_error = OK; }
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

	Array nodes_json;
	TypedArray<GVSEmitterNode> nodes = gvs->get_nodes();
	for (int i = 0; i < nodes.size(); i++) {
		Ref<GVSEmitterNode> node = nodes[i];
		if (!node.is_valid()) {
			continue;
		}
		Dictionary pos;
		pos["x"] = node->get_canvas_pos().x;
		pos["y"] = node->get_canvas_pos().y;

		Dictionary nd;
		nd["id"]         = node->get_id();
		nd["title"]      = node->get_title();
		nd["canvas_pos"] = pos;
		nodes_json.push_back(nd);
	}

	Dictionary root;
	root["version"] = gvs->get_version();
	root["nodes"]   = nodes_json;

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

}
