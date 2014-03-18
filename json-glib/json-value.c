/* json-value.c - JSON value container
 * 
 * This file is part of JSON-GLib
 * Copyright (C) 2012  Emmanuele Bassi <ebassi@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 *   Emmanuele Bassi  <ebassi@linux.intel.com>
 */

#include "config.h"

#include <glib.h>

#include "json-types-private.h"

const gchar *
json_value_type_get_name (JsonValueType value_type)
{
  switch (value_type)
    {
    case JSON_VALUE_INVALID:
      return "Unset";

    case JSON_VALUE_INT:
      return "Integer";

    case JSON_VALUE_DOUBLE:
      return "Floating Point";

    case JSON_VALUE_BOOLEAN:
      return "Boolean";

    case JSON_VALUE_STRING:
      return "String";

    case JSON_VALUE_NULL:
      return "Null";
    }

  return "Undefined";
}

GType
json_value_type (const JsonValue *value)
{
  switch (value->type)
    {
    case JSON_VALUE_INVALID:
      return G_TYPE_INVALID;

    case JSON_VALUE_INT:
      return G_TYPE_INT64;

    case JSON_VALUE_DOUBLE:
      return G_TYPE_DOUBLE;

    case JSON_VALUE_BOOLEAN:
      return G_TYPE_BOOLEAN;

    case JSON_VALUE_STRING:
      return G_TYPE_STRING;

    case JSON_VALUE_NULL:
      return G_TYPE_INVALID;
    }

  return G_TYPE_INVALID;
}

JsonValue *
json_value_alloc (void)
{
  JsonValue *res = g_slice_new0 (JsonValue);

  res->ref_count = 1;

  return res;
}

JsonValue *
json_value_init (JsonValue     *value,
                 JsonValueType  value_type)
{
  g_return_val_if_fail (value != NULL, NULL);

  if (value->type != JSON_VALUE_INVALID)
    json_value_unset (value);

  value->type = value_type;

  return value;
}

JsonValue *
json_value_ref (JsonValue *value)
{
  g_return_val_if_fail (value != NULL, NULL);

  g_atomic_int_add (&value->ref_count, 1);

  return value;
}

void
json_value_unref (JsonValue *value)
{
  g_return_if_fail (value != NULL);

  if (g_atomic_int_dec_and_test (&value->ref_count))
    json_value_free (value);
}

void
json_value_unset (JsonValue *value)
{
  g_return_if_fail (value != NULL);

  switch (value->type)
    {
    case JSON_VALUE_INVALID:
      break;

    case JSON_VALUE_INT:
      value->data.v_int = 0;
      break;

    case JSON_VALUE_DOUBLE:
      value->data.v_double = 0.0;
      break;

    case JSON_VALUE_BOOLEAN:
      value->data.v_bool = FALSE;
      break;

    case JSON_VALUE_STRING:
      g_free (value->data.v_str);
      value->data.v_str = NULL;
      break;

    case JSON_VALUE_NULL:
      break;
    }
}

void
json_value_free (JsonValue *value)
{
  if (G_LIKELY (value != NULL))
    {
      json_value_unset (value);
      g_slice_free (JsonValue, value);
    }
}

#define _JSON_VALUE_DEFINE_SET(Type,EType,CType,VField) \
void \
json_value_set_##Type (JsonValue *value, CType VField) \
{ \
  g_return_if_fail (JSON_VALUE_IS_VALID (value)); \
  g_return_if_fail (JSON_VALUE_HOLDS (value, JSON_VALUE_##EType)); \
\
  value->data.VField = VField; \
\
}

#define _JSON_VALUE_DEFINE_GET(Type,EType,CType,VField) \
CType \
json_value_get_##Type (const JsonValue *value) \
{ \
  g_return_val_if_fail (JSON_VALUE_IS_VALID (value), 0); \
  g_return_val_if_fail (JSON_VALUE_HOLDS (value, JSON_VALUE_##EType), 0); \
\
  return value->data.VField; \
}

#define _JSON_VALUE_DEFINE_SET_GET(Type,EType,CType,VField) \
_JSON_VALUE_DEFINE_SET(Type,EType,CType,VField) \
_JSON_VALUE_DEFINE_GET(Type,EType,CType,VField)

_JSON_VALUE_DEFINE_SET_GET(int, INT, gint64, v_int)

_JSON_VALUE_DEFINE_SET_GET(double, DOUBLE, gdouble, v_double)

_JSON_VALUE_DEFINE_SET_GET(boolean, BOOLEAN, gboolean, v_bool)

void
json_value_set_string (JsonValue *value,
                       const gchar *v_str)
{
  g_return_if_fail (JSON_VALUE_IS_VALID (value));
  g_return_if_fail (JSON_VALUE_HOLDS_STRING (value));

  g_free (value->data.v_str);
  value->data.v_str = g_strdup (v_str);
}

_JSON_VALUE_DEFINE_GET(string, STRING, const gchar *, v_str)

#undef _JSON_VALUE_DEFINE_SET_GET
#undef _JSON_VALUE_DEFINE_GET
#undef _JSON_VALUE_DEFINE_SET
