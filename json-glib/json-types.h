/* json-types.h - JSON data types
 * 
 * This file is part of JSON-GLib
 * Copyright (C) 2007  OpenedHand Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * Author:
 *   Emmanuele Bassi  <ebassi@openedhand.com>
 */

#ifndef __JSON_TYPES_H__
#define __JSON_TYPES_H__

#include <glib-object.h>

G_BEGIN_DECLS

/**
 * JSON_NODE_TYPE:
 * @node: a #JsonNode
 *
 * Evaluates to the #JsonNodeType contained by @node
 */
#define JSON_NODE_TYPE(node)    (((JsonNode *) (node))->type)
#define JSON_TYPE_OBJECT        (json_object_get_type ())
#define JSON_TYPE_ARRAY         (json_array_get_type ())

/**
 * JsonObject:
 *
 * A JSON object type. The contents of the #JsonObject structure are private
 * and should only be accessed by the provided API
 */
typedef struct _JsonObject      JsonObject;

/**
 * JsonArray:
 *
 * A JSON array type. The contents of the #JsonArray structure are private
 * and should only be accessed by the provided API
 */
typedef struct _JsonArray       JsonArray;

typedef struct _JsonNode        JsonNode;

/**
 * JsonNodeType:
 * @JSON_NODE_OBJECT: The node contains a #JsonObject
 * @JSON_NODE_ARRAY: The node contains a #JsonArray
 * @JSON_NODE_VALUE: The node contains a #GValue
 * @JSON_NODE_NULL: Special type, for nodes containing %NULL
 *
 * Indicates the content of a #JsonNode.
 */
typedef enum {
  JSON_NODE_OBJECT,
  JSON_NODE_ARRAY,
  JSON_NODE_VALUE,
  JSON_NODE_NULL
} JsonNodeType;

/**
 * JsonNode:
 *
 * A generic container of JSON data types. The contents of the #JsonNode
 * structure are private and should only be accessed via the provided
 * functions and never directly.
 */
struct _JsonNode
{
  /*< private >*/
  JsonNodeType type;

  union {
    JsonObject *object;
    JsonArray *array;
    GValue value;
  } data;

  JsonNode *parent;
};

JsonObject *          json_node_get_object    (JsonNode    *node);
JsonArray *           json_node_get_array     (JsonNode    *node);
void                  json_node_get_value     (JsonNode    *node,
                                               GValue      *value);
JsonNode *            json_node_get_parent    (JsonNode    *node);
G_CONST_RETURN gchar *json_node_type_name     (JsonNode    *node);

JsonObject *          json_object_ref         (JsonObject  *object);
void                  json_object_unref       (JsonObject  *object);
GList *               json_object_get_members (JsonObject  *object);
JsonNode *            json_object_get_member  (JsonObject  *object,
                                               const gchar *member_name);
gboolean              json_object_has_member  (JsonObject  *object,
                                               const gchar *member_name);
guint                 json_object_get_size    (JsonObject  *object);

JsonArray *           json_array_ref          (JsonArray   *array);
void                  json_array_unref        (JsonArray   *array);
GList *               json_array_get_elements (JsonArray   *array);
JsonNode *            json_array_get_element  (JsonArray   *array,
                                               guint        index_);
guint                 json_array_get_length   (JsonArray   *array);

G_END_DECLS

#endif /* __JSON_TYPES_H__ */
