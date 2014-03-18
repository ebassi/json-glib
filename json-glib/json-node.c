/* json-node.c - JSON object model node
 * 
 * This file is part of JSON-GLib
 * Copyright (C) 2007  OpenedHand Ltd.
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

#include "config.h"

#include <glib.h>

#include "json-types-private.h"

/**
 * SECTION:json-node
 * @short_description: Node in a JSON object model
 *
 * A #JsonNode is a generic container of elements inside a JSON stream.
 * It can contain fundamental types (integers, booleans, floating point
 * numbers, strings) and complex types (arrays and objects).
 *
 * When parsing a JSON data stream you extract the root node and walk
 * the node tree by retrieving the type of data contained inside the
 * node with the %JSON_NODE_TYPE macro. If the node contains a fundamental
 * type you can retrieve a copy of the #GValue holding it with the
 * json_node_get_value() function, and then use the #GValue API to extract
 * the data; if the node contains a complex type you can retrieve the
 * #JsonObject or the #JsonArray using json_node_get_object() or
 * json_node_get_array() respectively, and then retrieve the nodes
 * they contain.
 */

G_DEFINE_BOXED_TYPE (JsonNode, json_node, json_node_copy, json_node_free);

/**
 * json_node_get_value_type:
 * @node: a #JsonNode
 *
 * Returns the #GType of the payload of the node.
 *
 * Return value: a #GType for the payload.
 *
 * Since: 0.4
 */
GType
json_node_get_value_type (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, G_TYPE_INVALID);

  switch (node->type)
    {
    case JSON_NODE_OBJECT:
      return JSON_TYPE_OBJECT;

    case JSON_NODE_ARRAY:
      return JSON_TYPE_ARRAY;

    case JSON_NODE_NULL:
      return G_TYPE_INVALID;

    case JSON_NODE_VALUE:
      if (node->data.value)
        return JSON_VALUE_TYPE (node->data.value);
      else
        return G_TYPE_INVALID;

    default:
      g_assert_not_reached ();
      return G_TYPE_INVALID;
    }
}

/**
 * json_node_alloc: (constructor)
 *
 * Allocates a new #JsonNode. Use json_node_init() and its variants
 * to initialize the returned value.
 *
 * Return value: (transfer full): the newly allocated #JsonNode. Use
 *   json_node_free() to free the resources allocated by this function
 *
 * Since: 0.16
 */
JsonNode *
json_node_alloc (void)
{
  return g_slice_new0 (JsonNode);
}

static void
json_node_unset (JsonNode *node)
{
  g_assert (node != NULL);

  switch (node->type)
    {
    case JSON_NODE_OBJECT:
      if (node->data.object)
        json_object_unref (node->data.object);
      break;

    case JSON_NODE_ARRAY:
      if (node->data.array)
        json_array_unref (node->data.array);
      break;

    case JSON_NODE_VALUE:
      if (node->data.value)
        json_value_unref (node->data.value);
      break;

    case JSON_NODE_NULL:
      break;
    }
}

/**
 * json_node_init:
 * @node: the #JsonNode to initialize
 * @type: the type of JSON node to initialize @node to
 *
 * Initializes a @node to a specific @type.
 *
 * If the node has already been initialized once, it will be reset to
 * the given type, and any data contained will be cleared.
 *
 * Return value: (transfer none): the initialized #JsonNode
 *
 * Since: 0.16
 */
JsonNode *
json_node_init (JsonNode *node,
                JsonNodeType type)
{
  g_return_val_if_fail (type >= JSON_NODE_OBJECT &&
                        type <= JSON_NODE_NULL, NULL);

  json_node_unset (node);

  node->type = type;

  return node;
}

/**
 * json_node_init_object:
 * @node: the #JsonNode to initialize
 * @object: (allow-none): the #JsonObject to initialize @node with, or %NULL
 *
 * Initializes @node to %JSON_NODE_OBJECT and sets @object into it.
 *
 * This function will take a reference on @object.
 *
 * If the node has already been initialized once, it will be reset to
 * the given type, and any data contained will be cleared.
 *
 * Return value: (transfer none): the initialized #JsonNode
 *
 * Since: 0.16
 */
JsonNode *
json_node_init_object (JsonNode   *node,
                       JsonObject *object)
{
  g_return_val_if_fail (node != NULL, NULL);
  
  json_node_init (node, JSON_NODE_OBJECT);
  json_node_set_object (node, object);

  return node;
}

/**
 * json_node_init_array:
 * @node: the #JsonNode to initialize
 * @array: (allow-none): the #JsonArray to initialize @node with, or %NULL
 *
 * Initializes @node to %JSON_NODE_ARRAY and sets @array into it.
 *
 * This function will take a reference on @array.
 *
 * If the node has already been initialized once, it will be reset to
 * the given type, and any data contained will be cleared.
 *
 * Return value: (transfer none): the initialized #JsonNode
 *
 * Since: 0.16
 */
JsonNode *
json_node_init_array (JsonNode  *node,
                      JsonArray *array)
{
  g_return_val_if_fail (node != NULL, NULL);

  json_node_init (node, JSON_NODE_ARRAY);
  json_node_set_array (node, array);

  return node;
}

/**
 * json_node_init_int:
 * @node: the #JsonNode to initialize
 * @value: an integer
 *
 * Initializes @node to %JSON_NODE_VALUE and sets @value into it.
 *
 * If the node has already been initialized once, it will be reset to
 * the given type, and any data contained will be cleared.
 *
 * Return value: (transfer none): the initialized #JsonNode
 *
 * Since: 0.16
 */
JsonNode *
json_node_init_int (JsonNode *node,
                    gint64    value)
{
  g_return_val_if_fail (node != NULL, NULL);

  json_node_init (node, JSON_NODE_VALUE);
  json_node_set_int (node, value);

  return node;
}

/**
 * json_node_init_double:
 * @node: the #JsonNode to initialize
 * @value: a floating point value
 *
 * Initializes @node to %JSON_NODE_VALUE and sets @value into it.
 *
 * If the node has already been initialized once, it will be reset to
 * the given type, and any data contained will be cleared.
 *
 * Return value: (transfer none): the initialized #JsonNode
 *
 * Since: 0.16
 */
JsonNode *
json_node_init_double (JsonNode *node,
                       gdouble   value)
{
  g_return_val_if_fail (node != NULL, NULL);

  json_node_init (node, JSON_NODE_VALUE);
  json_node_set_double (node, value);

  return node;
}

/**
 * json_node_init_boolean:
 * @node: the #JsonNode to initialize
 * @value: a boolean value
 *
 * Initializes @node to %JSON_NODE_VALUE and sets @value into it.
 *
 * If the node has already been initialized once, it will be reset to
 * the given type, and any data contained will be cleared.
 *
 * Return value: (transfer none): the initialized #JsonNode
 *
 * Since: 0.16
 */
JsonNode *
json_node_init_boolean (JsonNode *node,
                        gboolean  value)
{
  g_return_val_if_fail (node != NULL, NULL);

  json_node_init (node, JSON_NODE_VALUE);
  json_node_set_boolean (node, value);

  return node;
}

/**
 * json_node_init_string:
 * @node: the #JsonNode to initialize
 * @value: (allow-none): a string value
 *
 * Initializes @node to %JSON_NODE_VALUE and sets @value into it.
 *
 * If the node has already been initialized once, it will be reset to
 * the given type, and any data contained will be cleared.
 *
 * Return value: (transfer none): the initialized #JsonNode
 *
 * Since: 0.16
 */
JsonNode *
json_node_init_string (JsonNode   *node,
                       const char *value)
{
  g_return_val_if_fail (node != NULL, NULL);

  json_node_init (node, JSON_NODE_VALUE);
  json_node_set_string (node, value);

  return node;
}

/**
 * json_node_init_null:
 * @node: the #JsonNode to initialize
 *
 * Initializes @node to %JSON_NODE_NULL.
 *
 * If the node has already been initialized once, it will be reset to
 * the given type, and any data contained will be cleared.
 *
 * Return value: (transfer none): the initialized #JsonNode
 *
 * Since: 0.16
 */
JsonNode *
json_node_init_null (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);

  return json_node_init (node, JSON_NODE_NULL);
}

/**
 * json_node_new: (constructor)
 * @type: a #JsonNodeType
 *
 * Creates a new #JsonNode of @type.
 *
 * This is a convenience function for json_node_alloc() and json_node_init(),
 * and it's the equivalent of:
 *
 * |[<!-- language="C" -->
     json_node_init (json_node_alloc (), type);
 * ]|
 *
 * Return value: (transfer full): the newly created #JsonNode
 */
JsonNode *
json_node_new (JsonNodeType type)
{
  g_return_val_if_fail (type >= JSON_NODE_OBJECT &&
                        type <= JSON_NODE_NULL, NULL);

  return json_node_init (json_node_alloc (), type);
}

/**
 * json_node_copy:
 * @node: a #JsonNode
 *
 * Copies @node. If the node contains complex data types then the reference
 * count of the objects is increased.
 *
 * Return value: (transfer full): the copied #JsonNode
 */
JsonNode *
json_node_copy (JsonNode *node)
{
  JsonNode *copy;

  g_return_val_if_fail (node != NULL, NULL);

  copy = g_slice_new0 (JsonNode);
  copy->type = node->type;

  switch (copy->type)
    {
    case JSON_NODE_OBJECT:
      copy->data.object = json_node_dup_object (node);
      break;

    case JSON_NODE_ARRAY:
      copy->data.array = json_node_dup_array (node);
      break;

    case JSON_NODE_VALUE:
      if (node->data.value)
        copy->data.value = json_value_ref (node->data.value);
      break;

    case JSON_NODE_NULL:
      break;

    default:
      g_assert_not_reached ();
    }

  return copy;
}

/**
 * json_node_set_object:
 * @node: a #JsonNode initialized to %JSON_NODE_OBJECT
 * @object: a #JsonObject
 *
 * Sets @objects inside @node. The reference count of @object is increased.
 */
void
json_node_set_object (JsonNode   *node,
                      JsonObject *object)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_OBJECT);

  if (node->data.object != NULL)
    json_object_unref (node->data.object);

  if (object)
    node->data.object = json_object_ref (object);
  else
    node->data.object = NULL;
}

/**
 * json_node_take_object:
 * @node: a #JsonNode initialized to %JSON_NODE_OBJECT
 * @object: (transfer full): a #JsonObject
 *
 * Sets @object inside @node. The reference count of @object is not increased.
 */
void
json_node_take_object (JsonNode   *node,
                       JsonObject *object)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_OBJECT);

  if (node->data.object)
    {
      json_object_unref (node->data.object);
      node->data.object = NULL;
    }

  if (object)
    node->data.object = object;
}

/**
 * json_node_get_object:
 * @node: a #JsonNode
 *
 * Retrieves the #JsonObject stored inside a #JsonNode
 *
 * Return value: (transfer none): the #JsonObject
 */
JsonObject *
json_node_get_object (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_OBJECT, NULL);

  return node->data.object;
}

/**
 * json_node_dup_object:
 * @node: a #JsonNode
 *
 * Retrieves the #JsonObject inside @node. The reference count of
 * the returned object is increased.
 *
 * Return value: (transfer full): the #JsonObject
 */
JsonObject *
json_node_dup_object (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_OBJECT, NULL);

  if (node->data.object)
    return json_object_ref (node->data.object);
  
  return NULL;
}

/**
 * json_node_set_array:
 * @node: a #JsonNode initialized to %JSON_NODE_ARRAY
 * @array: a #JsonArray
 *
 * Sets @array inside @node and increases the #JsonArray reference count
 */
void
json_node_set_array (JsonNode  *node,
                     JsonArray *array)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_ARRAY);

  if (node->data.array)
    json_array_unref (node->data.array);

  if (array)
    node->data.array = json_array_ref (array);
  else
    node->data.array = NULL;
}

/**
 * json_node_take_array:
 * @node: a #JsonNode initialized to %JSON_NODE_ARRAY
 * @array: (transfer full): a #JsonArray
 *
 * Sets @array into @node without increasing the #JsonArray reference count.
 */
void
json_node_take_array (JsonNode  *node,
                      JsonArray *array)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_ARRAY);

  if (node->data.array)
    {
      json_array_unref (node->data.array);
      node->data.array = NULL;
    }

  if (array)
    node->data.array = array;
}

/**
 * json_node_get_array:
 * @node: a #JsonNode
 *
 * Retrieves the #JsonArray stored inside a #JsonNode
 *
 * Return value: (transfer none): the #JsonArray
 */
JsonArray *
json_node_get_array (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_ARRAY, NULL);

  return node->data.array;
}

/**
 * json_node_dup_array:
 * @node: a #JsonNode
 *
 * Retrieves the #JsonArray stored inside a #JsonNode and returns it
 * with its reference count increased by one.
 *
 * Return value: (transfer full): the #JsonArray with its reference
 *   count increased.
 */
JsonArray *
json_node_dup_array (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_ARRAY, NULL);

  if (node->data.array)
    return json_array_ref (node->data.array);

  return NULL;
}

/**
 * json_node_get_value:
 * @node: a #JsonNode
 * @value: (out caller-allocates): return location for an uninitialized value
 *
 * Retrieves a value from a #JsonNode and copies into @value. When done
 * using it, call g_value_unset() on the #GValue.
 */
void
json_node_get_value (JsonNode *node,
                     GValue   *value)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE);

  if (node->data.value)
    {
      g_value_init (value, JSON_VALUE_TYPE (node->data.value));
      switch (JSON_VALUE_TYPE (node->data.value))
        {
        case G_TYPE_INT64:
          g_value_set_int64 (value, json_value_get_int (node->data.value));
          break;

        case G_TYPE_DOUBLE:
          g_value_set_double (value, json_value_get_double (node->data.value));
          break;

        case G_TYPE_BOOLEAN:
          g_value_set_boolean (value, json_value_get_boolean (node->data.value));
          break;

        case G_TYPE_STRING:
          g_value_set_string (value, json_value_get_string (node->data.value));
          break;

        default:
          break;
        }
    }
}

/**
 * json_node_set_value:
 * @node: a #JsonNode initialized to %JSON_NODE_VALUE
 * @value: the #GValue to set
 *
 * Sets @value inside @node. The passed #GValue is copied into the #JsonNode
 */
void
json_node_set_value (JsonNode     *node,
                     const GValue *value)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE);
  g_return_if_fail (G_VALUE_TYPE (value) != G_TYPE_INVALID);

  if (node->data.value == NULL)
    node->data.value = json_value_alloc ();

  switch (G_VALUE_TYPE (value))
    {
    /* auto-promote machine integers to 64 bit integers */
    case G_TYPE_INT64:
    case G_TYPE_INT:
      json_value_init (node->data.value, JSON_VALUE_INT);
      if (G_VALUE_TYPE (value) == G_TYPE_INT64)
        json_value_set_int (node->data.value, g_value_get_int64 (value));
      else
        json_value_set_int (node->data.value, g_value_get_int (value));
      break;

    case G_TYPE_BOOLEAN:
      json_value_init (node->data.value, JSON_VALUE_BOOLEAN);
      json_value_set_boolean (node->data.value, g_value_get_boolean (value));
      break;

    /* auto-promote single-precision floats to double precision floats */
    case G_TYPE_DOUBLE:
    case G_TYPE_FLOAT:
      json_value_init (node->data.value, JSON_VALUE_DOUBLE);
      if (G_VALUE_TYPE (value) == G_TYPE_DOUBLE)
        json_value_set_double (node->data.value, g_value_get_double (value));
      else
        json_value_set_double (node->data.value, g_value_get_float (value));
      break;

    case G_TYPE_STRING:
      json_value_init (node->data.value, JSON_VALUE_STRING);
      json_value_set_string (node->data.value, g_value_get_string (value));
      break;

    default:
      g_warning ("Invalid value of type '%s'",
                 g_type_name (G_VALUE_TYPE (value)));
      return;
    }

}

/**
 * json_node_free:
 * @node: a #JsonNode
 *
 * Frees the resources allocated by @node.
 */
void
json_node_free (JsonNode *node)
{
  if (G_LIKELY (node))
    {
      json_node_unset (node);
      g_slice_free (JsonNode, node);
    }
}

/**
 * json_node_type_name:
 * @node: a #JsonNode
 *
 * Retrieves the user readable name of the data type contained by @node.
 *
 * Return value: a string containing the name of the type. The returned string
 *   is owned by the node and should never be modified or freed
 */
const gchar *
json_node_type_name (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, "(null)");

  switch (node->type)
    {
    case JSON_NODE_OBJECT:
    case JSON_NODE_ARRAY:
    case JSON_NODE_NULL:
      return json_node_type_get_name (node->type);

    case JSON_NODE_VALUE:
      if (node->data.value)
        return json_value_type_get_name (node->data.value->type);
    }

  return "unknown";
}

const gchar *
json_node_type_get_name (JsonNodeType node_type)
{
  switch (node_type)
    {
    case JSON_NODE_OBJECT:
      return "JsonObject";

    case JSON_NODE_ARRAY:
      return "JsonArray";

    case JSON_NODE_NULL:
      return "NULL";

    case JSON_NODE_VALUE:
      return "Value";

    default:
      g_assert_not_reached ();
      break;
    }

  return "unknown";
}

/**
 * json_node_set_parent:
 * @node: a #JsonNode
 * @parent: (transfer none): the parent #JsonNode of @node
 *
 * Sets the parent #JsonNode of @node
 *
 * Since: 0.8
 */
void
json_node_set_parent (JsonNode *node,
                      JsonNode *parent)
{
  g_return_if_fail (node != NULL);

  node->parent = parent;
}

/**
 * json_node_get_parent:
 * @node: a #JsonNode
 *
 * Retrieves the parent #JsonNode of @node.
 *
 * Return value: (transfer none): the parent node, or %NULL if @node is
 *   the root node
 */
JsonNode *
json_node_get_parent (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);

  return node->parent;
}

/**
 * json_node_set_string:
 * @node: a #JsonNode initialized to %JSON_NODE_VALUE
 * @value: a string value
 *
 * Sets @value as the string content of the @node, replacing any existing
 * content.
 */
void
json_node_set_string (JsonNode    *node,
                      const gchar *value)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE);

  if (node->data.value == NULL)
    node->data.value = json_value_init (json_value_alloc (), JSON_VALUE_STRING);
  else
    json_value_init (node->data.value, JSON_VALUE_STRING);

  json_value_set_string (node->data.value, value);
}

/**
 * json_node_get_string:
 * @node: a #JsonNode of type %JSON_NODE_VALUE
 *
 * Gets the string value stored inside a #JsonNode
 *
 * Return value: a string value.
 */
const gchar *
json_node_get_string (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);

  if (JSON_NODE_TYPE (node) == JSON_NODE_NULL)
    return NULL;

  if (JSON_VALUE_HOLDS_STRING (node->data.value))
    return json_value_get_string (node->data.value);

  return NULL;
}

/**
 * json_node_dup_string:
 * @node: a #JsonNode of type %JSON_NODE_VALUE
 *
 * Gets a copy of the string value stored inside a #JsonNode
 *
 * Return value: (transfer full): a newly allocated string containing a copy
 *   of the #JsonNode contents. Use g_free() to free the allocated resources
 */
gchar *
json_node_dup_string (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);

  return g_strdup (json_node_get_string (node));
}

/**
 * json_node_set_int:
 * @node: a #JsonNode of type %JSON_NODE_VALUE
 * @value: an integer value
 *
 * Sets @value as the integer content of the @node, replacing any existing
 * content.
 */
void
json_node_set_int (JsonNode *node,
                   gint64    value)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE);

  if (node->data.value == NULL)
    node->data.value = json_value_init (json_value_alloc (), JSON_VALUE_INT);
  else
    json_value_init (node->data.value, JSON_VALUE_INT);

  json_value_set_int (node->data.value, value);
}

/**
 * json_node_get_int:
 * @node: a #JsonNode of type %JSON_NODE_VALUE
 *
 * Gets the integer value stored inside a #JsonNode
 *
 * Return value: an integer value.
 */
gint64
json_node_get_int (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, 0);

  if (JSON_NODE_TYPE (node) == JSON_NODE_NULL)
    return 0;

  if (JSON_VALUE_HOLDS_INT (node->data.value))
    return json_value_get_int (node->data.value);

  if (JSON_VALUE_HOLDS_DOUBLE (node->data.value))
    return json_value_get_double (node->data.value);

  if (JSON_VALUE_HOLDS_BOOLEAN (node->data.value))
    return json_value_get_boolean (node->data.value);

  return 0;
}

/**
 * json_node_set_double:
 * @node: a #JsonNode of type %JSON_NODE_VALUE
 * @value: a double value
 *
 * Sets @value as the double content of the @node, replacing any existing
 * content.
 */
void
json_node_set_double (JsonNode *node,
                      gdouble   value)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE);

  if (node->data.value == NULL)
    node->data.value = json_value_init (json_value_alloc (), JSON_VALUE_DOUBLE);
  else
    json_value_init (node->data.value, JSON_VALUE_DOUBLE);

  json_value_set_double (node->data.value, value);
}

/**
 * json_node_get_double:
 * @node: a #JsonNode of type %JSON_NODE_VALUE
 *
 * Gets the double value stored inside a #JsonNode
 *
 * Return value: a double value.
 */
gdouble
json_node_get_double (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, 0.0);

  if (JSON_NODE_TYPE (node) == JSON_NODE_NULL)
    return 0;

  if (JSON_VALUE_HOLDS_DOUBLE (node->data.value))
    return json_value_get_double (node->data.value);

  if (JSON_VALUE_HOLDS_INT (node->data.value))
    return (gdouble) json_value_get_int (node->data.value);

  if (JSON_VALUE_HOLDS_BOOLEAN (node->data.value))
    return (gdouble) json_value_get_boolean (node->data.value);

  return 0.0;
}

/**
 * json_node_set_boolean:
 * @node: a #JsonNode of type %JSON_NODE_VALUE
 * @value: a boolean value
 *
 * Sets @value as the boolean content of the @node, replacing any existing
 * content.
 */
void
json_node_set_boolean (JsonNode *node,
                       gboolean  value)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE);

  if (node->data.value == NULL)
    node->data.value = json_value_init (json_value_alloc (), JSON_VALUE_BOOLEAN);
  else
    json_value_init (node->data.value, JSON_VALUE_BOOLEAN);

  json_value_set_boolean (node->data.value, value);
}

/**
 * json_node_get_boolean:
 * @node: a #JsonNode of type %JSON_NODE_VALUE
 *
 * Gets the boolean value stored inside a #JsonNode
 *
 * Return value: a boolean value.
 */
gboolean
json_node_get_boolean (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, FALSE);

  if (JSON_NODE_TYPE (node) == JSON_NODE_NULL)
    return FALSE;

  if (JSON_VALUE_HOLDS_BOOLEAN (node->data.value))
    return json_value_get_boolean (node->data.value);

  if (JSON_VALUE_HOLDS_INT (node->data.value))
    return json_value_get_int (node->data.value) != 0;

  if (JSON_VALUE_HOLDS_DOUBLE (node->data.value))
    return json_value_get_double (node->data.value) != 0.0;

  return FALSE;
}

/**
 * json_node_get_node_type:
 * @node: a #JsonNode
 *
 * Retrieves the #JsonNodeType of @node
 *
 * Return value: the type of the node
 *
 * Since: 0.8
 */
JsonNodeType
json_node_get_node_type (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, JSON_NODE_NULL);

  return node->type;
}

/**
 * json_node_is_null:
 * @node: a #JsonNode
 *
 * Checks whether @node is a %JSON_NODE_NULL.
 *
 * A %JSON_NODE_NULL node is not the same as a %NULL #JsonNode; a
 * %JSON_NODE_NULL represents a 'null' value in the JSON tree.
 *
 * Return value: %TRUE if the node is null
 *
 * Since: 0.8
 */
gboolean
json_node_is_null (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, TRUE);

  return node->type == JSON_NODE_NULL;
}
