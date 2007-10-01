/**
 * SECTION:json-node
 * @short_description: Base element in a JSON stream
 *
 * A #JsonNode is a generic container of elements inside a JSON stream.
 * It can contain fundamental types (integers, booleans, floating point
 * numbers, strings) and complex types (arrays and objects).
 *
 * When parsing a JSON data stream you extract the root node and walk
 * the node tree by retrieving the type of data contained inside the
 * node with the %JSON_NODE_TYPE macro. If the node contains a fundamental
 * type you can retrieve a copy of the GValue holding it with the
 * json_node_get_value() function, and then use the GValue API to extract
 * the data; if the node contains a complex type you can retrieve the
 * #JsonObject or the #JsonArray using json_node_get_object() or
 * json_node_get_array() respectively, and then retrieve the nodes
 * they contain.
 */

#include "config.h"

#include <glib.h>

#include "json-types.h"
#include "json-private.h"

JsonNode *
json_node_new (JsonNodeType type)
{
  JsonNode *data;

  data = g_slice_new (JsonNode);
  data->type = type;

  return data;
}

JsonNode *
json_node_copy (JsonNode *node)
{
  JsonNode *copy;

  g_return_val_if_fail (node != NULL, NULL);

  copy = g_slice_new (JsonNode);
  *copy = *node;

  switch (copy->type)
    {
    case JSON_NODE_OBJECT:
      copy->data.object = json_object_ref (node->data.object);
      break;
    case JSON_NODE_ARRAY:
      copy->data.array = json_array_ref (node->data.array);
      break;
    case JSON_NODE_VALUE:
      g_value_init (&(copy->data.value), G_VALUE_TYPE (&(node->data.value)));
      g_value_copy (&(node->data.value), &(copy->data.value));
      break;
    case JSON_NODE_NULL:
      break;
    default:
      g_assert_not_reached ();
    }

  return copy;
}

void
json_node_set_object (JsonNode   *node,
                      JsonObject *object)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_OBJECT);

  if (node->data.object)
    json_object_unref (node->data.object);

  if (object)
    node->data.object = json_object_ref (object);
  else
    node->data.object = NULL;
}

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
 * @data: a #JsonNode
 *
 * Retrieves the #JsonObject stored inside a #JsonNode
 *
 * Return value: the #JsonObject
 */
JsonObject *
json_node_get_object (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_OBJECT, NULL);

  return node->data.object;
}

JsonObject *
json_node_dup_object (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_OBJECT, NULL);

  if (node->data.object)
    return json_object_ref (node->data.object);
  
  return NULL;
}

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
 * Return value: the #JsonArray
 */
JsonArray *
json_node_get_array (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_ARRAY, NULL);

  return node->data.array;
}

JsonArray *
json_node_dup_array (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_ARRAY, NULL);

  if (node->data.array)
    return json_array_ref (node->data.array);

  return NULL;
}

void
json_node_get_value (JsonNode *node,
                     GValue   *value)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE);

  if (G_VALUE_TYPE (&(node->data.value)) != 0)
    {
      g_value_init (value, G_VALUE_TYPE (&(node->data.value)));
      g_value_copy (&(node->data.value), value);
    }
}

void
json_node_set_value (JsonNode     *node,
                     const GValue *value)
{
  g_return_if_fail (node != NULL);
  g_return_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE);

  if (G_VALUE_TYPE (&(node->data.value)) != 0)
    g_value_unset (&(node->data.value));

  g_value_init (&(node->data.value), G_VALUE_TYPE (value));
  g_value_copy (value, &(node->data.value));
}

void
json_node_free (JsonNode *node)
{
  if (G_LIKELY (node))
    {
      switch (node->type)
        {
        case JSON_NODE_OBJECT:
          json_object_unref (node->data.object);
          break;

        case JSON_NODE_ARRAY:
          json_array_unref (node->data.array);
          break;

        case JSON_NODE_VALUE:
          g_value_unset (&(node->data.value));
          break;

        case JSON_NODE_NULL:
          break;
        }

      g_slice_free (JsonNode, node);
    }
}

G_CONST_RETURN gchar *
json_node_type_name (JsonNode *node)
{
  g_return_val_if_fail (node != NULL, "(null)");

  switch (node->type)
    {
    case JSON_NODE_OBJECT:
      return "JsonObject";

    case JSON_NODE_ARRAY:
      return "JsonArray";

    case JSON_NODE_NULL:
      return "NULL";

    case JSON_NODE_VALUE:
      return g_type_name (G_VALUE_TYPE (&(node->data.value)));
    }

  return "unknown";
}
