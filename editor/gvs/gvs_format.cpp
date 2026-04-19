#include "gvs_format.h"

#include "core/io/file_access.h"
#include "core/io/json.h"

namespace GodotVisualStream {

// --- GVSFormatLoader ---

// Convierte un diccionario JSON en un GVSEmitterNode, ignorando campos ausentes
Ref<GVSEmitterNode> GVSFormatLoader::_parse_node(const Dictionary &p_dict) {
	Ref<GVSEmitterNode> node;
	node.instantiate();

	if (p_dict.has("id")) {
		node->set_id(int(p_dict["id"]));
	}
	if (p_dict.has("title")) {
		node->set_title(p_dict["title"]);
	}
	if (p_dict.has("canvas_pos") && p_dict["canvas_pos"].get_type() == Variant::DICTIONARY) {
		Dictionary pos = p_dict["canvas_pos"];
		float x = pos.has("x") ? float(pos["x"]) : 0.0f;
		float y = pos.has("y") ? float(pos["y"]) : 0.0f;
		node->set_canvas_pos(Vector2(x, y));
	}
	if (p_dict.has("lifetime")) {
		node->set_lifetime(float(p_dict["lifetime"]));
	}
	if (p_dict.has("lifetime_var")) {
		node->set_lifetime_var(p_dict["lifetime_var"]);
	}

	return node;
}

// Lee el archivo .gvs, parsea el JSON y reconstruye el GVSResource con sus nodos
Ref<Resource> GVSFormatLoader::load(const String &p_path, const String &p_original_path, Error *r_error,
		bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	// Lectura del archivo
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

	// Parseo JSON — si falla devolvemos el recurso vacío en vez de un error fatal
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

	// Extracción de campos del JSON raíz
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
			nodes.push_back(_parse_node(nodes_json[i]));
		}
		resource->set_nodes(nodes);
	}

	if (d.has("variables") && d["variables"].get_type() == Variant::ARRAY) {
		Array vars_json = d["variables"];
		TypedArray<GVSVariable> variables;
		for (int i = 0; i < vars_json.size(); i++) {
			if (vars_json[i].get_type() != Variant::DICTIONARY) {
				continue;
			}
			variables.push_back(_parse_variable(vars_json[i]));
		}
		resource->set_variables(variables);
	}

	if (r_error) { *r_error = OK; }
	return resource;
}

Ref<GVSVariable> GVSFormatLoader::_parse_variable(const Dictionary &p_dict) {
	Ref<GVSVariable> var;
	var.instantiate();

	if (p_dict.has("name")) {
		var->set_var_name(p_dict["name"]);
	}
	int type = 0;
	if (p_dict.has("type")) {
		type = int(p_dict["type"]);
		var->set_type(type);
	}
	if (p_dict.has("default_value")) {
		Variant raw = p_dict["default_value"];
		// Color stored as dict: reconstruct from stored type
		if (type == GVSVariable::VAR_COLOR && raw.get_type() == Variant::DICTIONARY) {
			Dictionary cd = raw;
			float r = cd.has("r") ? float(cd["r"]) : 1.0f;
			float g = cd.has("g") ? float(cd["g"]) : 1.0f;
			float b = cd.has("b") ? float(cd["b"]) : 1.0f;
			float a = cd.has("a") ? float(cd["a"]) : 1.0f;
			var->set_default_value(Color(r, g, b, a));
		} else {
			var->set_default_value(raw);
		}
	}
	return var;
}

// Informa a Godot de que este loader gestiona la extensión .gvs
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

// --- GVSFormatSaver ---

// Convierte un GVSEmitterNode en un diccionario JSON serializable
Dictionary GVSFormatSaver::_serialize_node(const Ref<GVSEmitterNode> &p_node) {
	Dictionary pos;
	pos["x"] = p_node->get_canvas_pos().x;
	pos["y"] = p_node->get_canvas_pos().y;

	Dictionary nd;
	nd["id"]           = p_node->get_id();
	nd["title"]        = p_node->get_title();
	nd["canvas_pos"]   = pos;
	nd["lifetime"]     = p_node->get_lifetime();
	nd["lifetime_var"] = p_node->get_lifetime_var();

	return nd;
}

// Serializa el GVSResource a JSON con indentación y lo escribe en disco
Error GVSFormatSaver::save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	Ref<GVSResource> gvs = p_resource;
	ERR_FAIL_COND_V(gvs.is_null(), ERR_INVALID_PARAMETER);

	// Serialización de los nodos
	Array nodes_json;
	TypedArray<GVSEmitterNode> nodes = gvs->get_nodes();
	for (int i = 0; i < nodes.size(); i++) {
		Ref<GVSEmitterNode> node = nodes[i];
		if (!node.is_valid()) {
			continue;
		}
		nodes_json.push_back(_serialize_node(node));
	}

	Array vars_json;
	TypedArray<GVSVariable> variables = gvs->get_variables();
	for (int i = 0; i < variables.size(); i++) {
		Ref<GVSVariable> var = variables[i];
		if (var.is_valid()) {
			vars_json.push_back(_serialize_variable(var));
		}
	}

	// Construcción del JSON raíz y escritura en disco
	Dictionary root;
	root["version"]   = gvs->get_version();
	root["variables"] = vars_json;
	root["nodes"]     = nodes_json;

	String json_text = JSON::stringify(root, "\t", false);

	Error err;
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::WRITE, &err);
	ERR_FAIL_COND_V_MSG(err != OK, err, vformat("GVSFormatSaver: cannot open '%s' for writing.", p_path));

	f->store_string(json_text);
	f->close();

	return OK;
}

// Informa a Godot de que este saver gestiona la extensión .gvs
void GVSFormatSaver::get_recognized_extensions(const Ref<Resource> &p_resource, List<String> *p_extensions) const {
	if (recognize(p_resource)) {
		p_extensions->push_back("gvs");
	}
}

Dictionary GVSFormatSaver::_serialize_variable(const Ref<GVSVariable> &p_var) {
	Dictionary vd;
	vd["name"] = p_var->get_var_name();
	vd["type"] = p_var->get_type();

	Variant dv = p_var->get_default_value();
	if (p_var->get_type() == GVSVariable::VAR_COLOR) {
		Color c = dv;
		Dictionary cd;
		cd["r"] = c.r;
		cd["g"] = c.g;
		cd["b"] = c.b;
		cd["a"] = c.a;
		vd["default_value"] = cd;
	} else {
		vd["default_value"] = dv;
	}
	return vd;
}

bool GVSFormatSaver::recognize(const Ref<Resource> &p_resource) const {
	return p_resource.is_valid() && p_resource->is_class("GVSResource");
}

}
