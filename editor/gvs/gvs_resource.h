#pragma once

#include "core/io/resource.h"

namespace GodotVisualStream {
	class GVSResource : public Resource {
		GDCLASS(GVSResource, Resource);

	protected:
		static void _bind_methods();

	public:
		int version = 1;

		Dictionary graph_data;

		void set_version(int p_version);
		int get_version() const;

		void set_graph_data(const Dictionary &p_data);
		Dictionary get_graph_data() const;

		GVSResource() = default;
	};

}
