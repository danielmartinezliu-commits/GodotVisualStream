#include "gvs_variable.h"

#include "core/object/class_db.h"

namespace GodotVisualStream {

void GVSVariable::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_var_name", "name"), &GVSVariable::set_var_name);
	ClassDB::bind_method(D_METHOD("get_var_name"), &GVSVariable::get_var_name);
	ClassDB::bind_method(D_METHOD("set_type", "type"), &GVSVariable::set_type);
	ClassDB::bind_method(D_METHOD("get_type"), &GVSVariable::get_type);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "var_name"), "set_var_name", "get_var_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "type"), "set_type", "get_type");
}

void GVSVariable::set_var_name(const String &p_name) {
	var_name = p_name;
}

String GVSVariable::get_var_name() const {
	return var_name;
}

void GVSVariable::set_type(int p_type) {
	type = p_type;
	default_value = default_for_type(p_type);
}

int GVSVariable::get_type() const {
	return type;
}

void GVSVariable::set_default_value(const Variant &p_val) {
	default_value = p_val;
}

Variant GVSVariable::get_default_value() const {
	return default_value;
}

Ref<GVSVariable> GVSVariable::create(const String &p_name, int p_type) {
	Ref<GVSVariable> var;
	var.instantiate();
	var->var_name = p_name;
	var->type = p_type;
	var->default_value = default_for_type(p_type);
	return var;
}

Variant GVSVariable::default_for_type(int p_type) {
	switch (p_type) {
		case VAR_FLOAT: return 0.0f;
		case VAR_INT: return 0;
		case VAR_BOOL: return false;
		case VAR_COLOR: return Color(1.0f, 1.0f, 1.0f, 1.0f);
		// VAR_VECTOR not yet implemented
	}
	return 0.0f;
}

}
