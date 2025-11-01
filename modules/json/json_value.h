#pragma once

#include <stdbool.h>

#include "string_array.h"

typedef struct _JsonValue JsonValue;

typedef enum {
  JSON_VALUE_TYPE_OBJECT,
  JSON_VALUE_TYPE_ARRAY,
  JSON_VALUE_TYPE_STRING,
  JSON_VALUE_TYPE_NUMBER,
  JSON_VALUE_TYPE_TRUE,
  JSON_VALUE_TYPE_FALSE,
  JSON_VALUE_TYPE_NULL
} JsonValueType;

JsonValue *json_value_new_object();

JsonValue *json_value_new_array();

JsonValue *json_value_new_string(const char *string);

JsonValue *json_value_new_string_take(char *string);

JsonValue *json_value_new_number(double number);

JsonValue *json_value_new_true();

JsonValue *json_value_new_false();

JsonValue *json_value_new_null();

JsonValueType json_value_get_type(JsonValue *self);

/// Gets an member with [name] from this object.
/// Returns `NULL` if this member is not present.
/// Value must be of type `JSON_VALUE_TYPE_OBJECT`.
JsonValue *json_value_get_member(JsonValue *self, const char *name);

/// Gets the name of the member at this [index] in the object.
/// Value must be of type `JSON_VALUE_TYPE_OBJECT`.
const char *json_value_get_member_name(JsonValue *self, size_t index);

/// Gets the value of the member at this [index] in the object.
/// Value must be of type `JSON_VALUE_TYPE_OBJECT`.
JsonValue *json_value_get_member_value(JsonValue *self, size_t index);

/// Gets an object member with [name] from this object.
/// Returns `NULL` if this member is not present or is not an object.
/// Value must be of type `JSON_VALUE_TYPE_OBJECT`.
JsonValue *json_value_get_object_member(JsonValue *self, const char *name);

/// Gets an array member with [name] from this object.
/// Returns `NULL` if this member is not present or is not an array.
/// Value must be of type `JSON_VALUE_TYPE_OBJECT`.
JsonValue *json_value_get_array_member(JsonValue *self, const char *name);

/// Gets an string array member with [name] from this object.
/// Returns `NULL` if this member is not present or is not a string array.
/// Value must be of type `JSON_VALUE_TYPE_OBJECT`.
StringArray *json_value_get_string_array_member(JsonValue *self,
                                                const char *name);

/// Gets an string member with [name] from this object.
/// Returns [default_value] if the member is not present.
/// Returns `NULL` if this member is not a string.
const char *json_value_get_string_member(JsonValue *self, const char *name,
                                         const char *default_value);

/// Gets an number member with [name] from this object.
/// Returns [default_value] if the member is not present.
/// Returns `0.0` if this member is not a number.
double json_value_get_number_member(JsonValue *self, const char *name,
                                    double default_value);

/// Gets an boolean member with [name] from this object.
/// Returns [default_value] if the member is not present.
/// Returns `false` if this member is not `true` or `false`.
bool json_value_get_boolean_member(JsonValue *self, const char *name,
                                   bool default_value);

/// Returns the number of elements in this value.
/// Value must be of type `JSON_VALUE_TYPE_OBJECT` or `JSON_VALUE_TYPE_ARRAY`.
size_t json_value_get_length(JsonValue *self);

/// Value must be of type `JSON_VALUE_TYPE_ARRAY`.
JsonValue *json_value_get_element(JsonValue *self, size_t index);

/// Returns the string this value represents.
/// Value must be of type `JSON_VALUE_TYPE_STRING`.
const char *json_value_get_string(JsonValue *self);

/// Returns the number this value represents.
/// Value must be of type `JSON_VALUE_TYPE_NUMBER`.
double json_value_get_number(JsonValue *self);

/// Set [name]/[value] pair in an object.
/// Value must be of type `JSON_VALUE_TYPE_OBJECT`.
void json_value_set_member(JsonValue *self, const char *name, JsonValue *value);

/// Add [element] to the end of an array.
/// Value must be of type `JSON_VALUE_TYPE_ARRAY`.
void json_value_add_element(JsonValue *self, JsonValue *element);

char *json_value_to_string(JsonValue *self);

JsonValue *json_value_ref(JsonValue *self);

void json_value_unref(JsonValue *self);
