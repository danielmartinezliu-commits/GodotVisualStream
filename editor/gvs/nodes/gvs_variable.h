#pragma once

#include "core/io/resource.h"

namespace GodotVisualStream {

class GVSVariable : public Resource {
	GDCLASS(GVSVariable, Resource);

	String var_name = "variable";
	int type = 0;
	Variant default_value;

protected:
	static void _bind_methods();

public:
	enum Type {
		VAR_FLOAT   = 0,
		VAR_INT     = 1,
		VAR_BOOL    = 2,
		VAR_COLOR   = 3,
		VAR_VECTOR3 = 4,
	};

	void set_var_name(const String &p_name);
	String get_var_name() const;

	void set_type(int p_type);
	int get_type() const;

	void set_default_value(const Variant &p_val);
	Variant get_default_value() const;

	static Ref<GVSVariable> create(const String &p_name, int p_type);
	static Variant default_for_type(int p_type);
};

}
