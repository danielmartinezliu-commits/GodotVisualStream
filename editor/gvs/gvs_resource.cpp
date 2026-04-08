#include "gvs_resource.h"

#include "core/object/class_db.h"

namespace GodotVisualStream {

	void GVSResource::_bind_methods() {
		ClassDB::bind_method(D_METHOD("set_version", "version"), &GVSResource::set_version);
		ClassDB::bind_method(D_METHOD("get_version"), &GVSResource::get_version);

		ClassDB::bind_method(D_METHOD("set_graph_data", "data"), &GVSResource::set_graph_data);
		ClassDB::bind_method(D_METHOD("get_graph_data"), &GVSResource::get_graph_data);

		ADD_PROPERTY(PropertyInfo(Variant::INT, "version"), "set_version", "get_version");
		ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "graph_data"), "set_graph_data", "get_graph_data");
	}

	void GVSResource::set_version(int p_version) {
		version = p_version;
	}

	int GVSResource::get_version() const {
		return version;
	}

	void GVSResource::set_graph_data(const Dictionary &p_data) {
		graph_data = p_data;
	}

	Dictionary GVSResource::get_graph_data() const {
		return graph_data;
	}

}
