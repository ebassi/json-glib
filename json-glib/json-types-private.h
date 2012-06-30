/* json-types-private.h - JSON data types private header
 * 
 * This file is part of JSON-GLib
 * Copyright (C) 2007  OpenedHand Ltd
 * Copyright (C) 2009  Intel Corp.
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

#ifndef __JSON_TYPES_PRIVATE_H__
#define __JSON_TYPES_PRIVATE_H__

#include "json-types.h"

G_BEGIN_DECLS

typedef struct _JsonValue JsonValue;

typedef enum {
  JSON_VALUE_INVALID = 0,
  JSON_VALUE_INT,
  JSON_VALUE_DOUBLE,
  JSON_VALUE_BOOLEAN,
  JSON_VALUE_STRING,
  JSON_VALUE_NULL
} JsonValueType;

struct _JsonNode
{
  /*< private >*/
  JsonNodeType type;

  union {
    JsonObject *object;
    JsonArray *array;
    JsonValue *value;
  } data;

  JsonNode *parent;
};

#define JSON_VALUE_INIT                 { JSON_VALUE_INVALID, 1, { 0 } }
#define JSON_VALUE_INIT_TYPE(t)         { (t), 1, { 0 } }
#define JSON_VALUE_IS_VALID(v)          ((v) != NULL && (v)->type != JSON_VALUE_INVALID)
#define JSON_VALUE_HOLDS(v,t)           ((v) != NULL && (v)->type == (t))
#define JSON_VALUE_HOLDS_INT(v)         (JSON_VALUE_HOLDS((v), JSON_VALUE_INT))
#define JSON_VALUE_HOLDS_DOUBLE(v)      (JSON_VALUE_HOLDS((v), JSON_VALUE_DOUBLE))
#define JSON_VALUE_HOLDS_BOOLEAN(v)     (JSON_VALUE_HOLDS((v), JSON_VALUE_BOOLEAN))
#define JSON_VALUE_HOLDS_STRING(v)      (JSON_VALUE_HOLDS((v), JSON_VALUE_STRING))
#define JSON_VALUE_HOLDS_NULL(v)        (JSON_VALUE_HOLDS((v), JSON_VALUE_NULL))
#define JSON_VALUE_TYPE(v)              (json_value_type((v)))

struct _JsonValue
{
  JsonValueType type;

  volatile gint ref_count;

  union {
    gint64 v_int;
    gdouble v_double;
    gboolean v_bool;
    gchar *v_str;
  } data;
};

struct _JsonArray
{
  GPtrArray *elements;

  volatile gint ref_count;
};

struct _JsonObject
{
  GHashTable *members;

  /* the members of the object, ordered in reverse */
  GList *members_ordered;

  volatile gint ref_count;
};

G_GNUC_INTERNAL
const gchar *   json_node_type_get_name         (JsonNodeType     node_type);
G_GNUC_INTERNAL
const gchar *   json_value_type_get_name        (JsonValueType    value_type);

G_GNUC_INTERNAL
GType           json_value_type                 (const JsonValue *value);

G_GNUC_INTERNAL
JsonValue *     json_value_alloc                (void);
G_GNUC_INTERNAL
JsonValue *     json_value_init                 (JsonValue       *value,
                                                 JsonValueType    value_type);
G_GNUC_INTERNAL
JsonValue *     json_value_ref                  (JsonValue       *value);
G_GNUC_INTERNAL
void            json_value_unref                (JsonValue       *value);
G_GNUC_INTERNAL
void            json_value_unset                (JsonValue       *value);
G_GNUC_INTERNAL
void            json_value_free                 (JsonValue       *value);
G_GNUC_INTERNAL
void            json_value_set_int              (JsonValue       *value,
                                                 gint64           v_int);
G_GNUC_INTERNAL
gint64          json_value_get_int              (const JsonValue *value);
G_GNUC_INTERNAL
void            json_value_set_double           (JsonValue       *value,
                                                 gdouble          v_double);
G_GNUC_INTERNAL
gdouble         json_value_get_double           (const JsonValue *value);
G_GNUC_INTERNAL
void            json_value_set_boolean          (JsonValue       *value,
                                                 gboolean         v_bool);
G_GNUC_INTERNAL
gboolean        json_value_get_boolean          (const JsonValue *value);
G_GNUC_INTERNAL
void            json_value_set_string           (JsonValue       *value,
                                                 const gchar     *v_str);
G_GNUC_INTERNAL
const gchar *   json_value_get_string           (const JsonValue *value);

G_END_DECLS

#endif /* __JSON_TYPES_PRIVATE_H__ */
