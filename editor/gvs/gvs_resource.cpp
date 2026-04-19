#include "gvs_resource.h"

#include "core/object/class_db.h"

namespace GodotVisualStream {

// Expone version y nodes al sistema de propiedades de Godot (inspector y serialización)
void GVSResource::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_version", "version"), &GVSResource::set_version);
	ClassDB::bind_method(D_METHOD("get_version"), &GVSResource::get_version);
	ClassDB::bind_method(D_METHOD("set_nodes", "nodes"), &GVSResource::set_nodes);
	ClassDB::bind_method(D_METHOD("get_nodes"), &GVSResource::get_nodes);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "version"), "set_version", "get_version");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "nodes", PROPERTY_HINT_ARRAY_TYPE, "GVSEmitterNode"),
			"set_nodes", "get_nodes");

	ClassDB::bind_method(D_METHOD("set_variables", "variables"), &GVSResource::set_variables);
	ClassDB::bind_method(D_METHOD("get_variables"), &GVSResource::get_variables);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "variables", PROPERTY_HINT_ARRAY_TYPE, "GVSVariable"),
			"set_variables", "get_variables");
}

void GVSResource::set_version(int p_version) {
	version = p_version;
}

int GVSResource::get_version() const {
	return version;
}

void GVSResource::set_nodes(const TypedArray<GVSEmitterNode> &p_nodes) {
	nodes = p_nodes;
}

TypedArray<GVSEmitterNode> GVSResource::get_nodes() const {
	return nodes;
}

void GVSResource::set_variables(const TypedArray<GVSVariable> &p_variables) {
	variables = p_variables;
}

TypedArray<GVSVariable> GVSResource::get_variables() const {
	return variables;
}

}
