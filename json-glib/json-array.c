/* json-array.c - JSON array implementation
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

#include "json-types-private.h"

/**
 * SECTION:json-array
 * @short_description: a JSON array representation
 *
 * #JsonArray is the representation of the array type inside JSON. It contains
 * #JsonNode elements, which may contain fundamental types, other arrays or
 * objects.
 *
 * Since arrays can be expensive, they are reference counted. You can control
 * the lifetime of a #JsonArray using json_array_ref() and json_array_unref().
 *
 * To append an element, use json_array_add_element().
 * To extract an element at a given index, use json_array_get_element().
 * To retrieve the entire array in list form, use json_array_get_elements().
 * To retrieve the length of the array, use json_array_get_length().
 */

G_DEFINE_BOXED_TYPE (JsonArray, json_array, json_array_ref, json_array_unref);

/**
 * json_array_new: (constructor)
 *
 * Creates a new #JsonArray.
 *
 * Return value: (transfer full): the newly created #JsonArray
 */
JsonArray *
json_array_new (void)
{
  JsonArray *array;

  array = g_slice_new0 (JsonArray);

  array->ref_count = 1;
  array->elements = g_ptr_array_new ();

  return array;
}

/**
 * json_array_sized_new: (constructor)
 * @n_elements: number of slots to pre-allocate
 *
 * Creates a new #JsonArray with @n_elements slots already allocated.
 *
 * Return value: (transfer full): the newly created #JsonArray
 */
JsonArray *
json_array_sized_new (guint n_elements)
{
  JsonArray *array;

  array = g_slice_new0 (JsonArray);
  
  array->ref_count = 1;
  array->elements = g_ptr_array_sized_new (n_elements);

  return array;
}

/**
 * json_array_ref:
 * @array: a #JsonArray
 *
 * Increase by one the reference count of a #JsonArray.
 *
 * Return value: (transfer none): the passed #JsonArray, with the reference count
 *   increased by one.
 */
JsonArray *
json_array_ref (JsonArray *array)
{
  g_return_val_if_fail (array != NULL, NULL);
  g_return_val_if_fail (array->ref_count > 0, NULL);

  array->ref_count++;

  return array;
}

/**
 * json_array_unref:
 * @array: a #JsonArray
 *
 * Decreases by one the reference count of a #JsonArray. If the
 * reference count reaches zero, the array is destroyed and all
 * its allocated resources are freed.
 */
void
json_array_unref (JsonArray *array)
{
  g_return_if_fail (array != NULL);
  g_return_if_fail (array->ref_count > 0);

  if (--array->ref_count == 0)
    {
      guint i;

      for (i = 0; i < array->elements->len; i++)
        json_node_unref (g_ptr_array_index (array->elements, i));

      g_ptr_array_free (array->elements, TRUE);
      array->elements = NULL;

      g_slice_free (JsonArray, array);
    }
}

/**
 * json_array_seal:
 * @array: a #JsonArray
 *
 * Seals the #JsonArray, making it immutable to further changes. This will
 * recursively seal all elements in the array too.
 *
 * If the @array is already immutable, this is a no-op.
 *
 * Since: 1.2
 */
void
json_array_seal (JsonArray *array)
{
  guint i;

  g_return_if_fail (array != NULL);
  g_return_if_fail (array->ref_count > 0);

  if (array->immutable)
    return;

  /* Propagate to all members. */
  for (i = 0; i < array->elements->len; i++)
    json_node_seal (g_ptr_array_index (array->elements, i));

  array->immutable_hash = json_array_hash (array);
  array->immutable = TRUE;
}

/**
 * json_array_is_immutable:
 * @array: a #JsonArray
 *
 * Check whether the given @array has been marked as immutable by calling
 * json_array_seal() on it.
 *
 * Since: 1.2
 * Returns: %TRUE if the @array is immutable
 */
gboolean
json_array_is_immutable (JsonArray *array)
{
  g_return_val_if_fail (array != NULL, FALSE);
  g_return_val_if_fail (array->ref_count > 0, FALSE);

  return array->immutable;
}

/**
 * json_array_get_elements:
 * @array: a #JsonArray
 *
 * Gets the elements of a #JsonArray as a list of #JsonNode instances.
 *
 * Return value: (element-type JsonNode) (transfer container): a #GList
 *   containing the elements of the array. The contents of the list are
 *   owned by the array and should never be modified or freed. Use
 *   g_list_free() on the returned list when done using it
 */
GList *
json_array_get_elements (JsonArray *array)
{
  GList *retval;
  guint i;

  g_return_val_if_fail (array != NULL, NULL);

  retval = NULL;
  for (i = 0; i < array->elements->len; i++)
    retval = g_list_prepend (retval,
                             g_ptr_array_index (array->elements, i));

  return g_list_reverse (retval);
}

/**
 * json_array_dup_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 *
 * Retrieves a copy of the #JsonNode containing the value of the
 * element at @index_ inside a #JsonArray
 *
 * Return value: (transfer full): a copy of the #JsonNode at the requested
 *   index. Use json_node_unref() when done.
 *
 * Since: 0.6
 */
JsonNode *
json_array_dup_element (JsonArray *array,
                        guint      index_)
{
  JsonNode *retval;

  g_return_val_if_fail (array != NULL, NULL);
  g_return_val_if_fail (index_ < array->elements->len, NULL);

  retval = json_array_get_element (array, index_);
  if (!retval)
    return NULL;

  return json_node_copy (retval);
}

/**
 * json_array_get_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 * 
 * Retrieves the #JsonNode containing the value of the element at @index_
 * inside a #JsonArray.
 *
 * Return value: (transfer none): a pointer to the #JsonNode at the requested index
 */
JsonNode *
json_array_get_element (JsonArray *array,
                        guint      index_)
{
  g_return_val_if_fail (array != NULL, NULL);
  g_return_val_if_fail (index_ < array->elements->len, NULL);

  return g_ptr_array_index (array->elements, index_);
}

/**
 * json_array_get_int_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 *
 * Conveniently retrieves the integer value of the element at @index_
 * inside @array
 *
 * See also: json_array_get_element(), json_node_get_int()
 *
 * Return value: the integer value
 *
 * Since: 0.8
 */
gint64
json_array_get_int_element (JsonArray *array,
                            guint      index_)
{
  JsonNode *node;

  g_return_val_if_fail (array != NULL, 0);
  g_return_val_if_fail (index_ < array->elements->len, 0);

  node = g_ptr_array_index (array->elements, index_);
  g_return_val_if_fail (node != NULL, 0);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE, 0);

  return json_node_get_int (node);
}

/**
 * json_array_get_double_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 *
 * Conveniently retrieves the floating point value of the element at
 * @index_ inside @array
 *
 * See also: json_array_get_element(), json_node_get_double()
 *
 * Return value: the floating point value
 *
 * Since: 0.8
 */
gdouble
json_array_get_double_element (JsonArray *array,
                               guint      index_)
{
  JsonNode *node;

  g_return_val_if_fail (array != NULL, 0.0);
  g_return_val_if_fail (index_ < array->elements->len, 0.0);

  node = g_ptr_array_index (array->elements, index_);
  g_return_val_if_fail (node != NULL, 0.0);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE, 0.0);

  return json_node_get_double (node);
}

/**
 * json_array_get_boolean_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 *
 * Conveniently retrieves the boolean value of the element at @index_
 * inside @array
 *
 * See also: json_array_get_element(), json_node_get_boolean()
 *
 * Return value: the integer value
 *
 * Since: 0.8
 */
gboolean
json_array_get_boolean_element (JsonArray *array,
                                guint      index_)
{
  JsonNode *node;

  g_return_val_if_fail (array != NULL, FALSE);
  g_return_val_if_fail (index_ < array->elements->len, FALSE);

  node = g_ptr_array_index (array->elements, index_);
  g_return_val_if_fail (node != NULL, FALSE);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE, FALSE);

  return json_node_get_boolean (node);
}

/**
 * json_array_get_string_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 *
 * Conveniently retrieves the string value of the element at @index_
 * inside @array
 *
 * See also: json_array_get_element(), json_node_get_string()
 *
 * Return value: the string value; the returned string is owned by
 *   the #JsonArray and should not be modified or freed
 *
 * Since: 0.8
 */
const gchar *
json_array_get_string_element (JsonArray *array,
                               guint      index_)
{
  JsonNode *node;

  g_return_val_if_fail (array != NULL, NULL);
  g_return_val_if_fail (index_ < array->elements->len, NULL);

  node = g_ptr_array_index (array->elements, index_);
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_HOLDS_VALUE (node) || JSON_NODE_HOLDS_NULL (node), NULL);

  if (JSON_NODE_HOLDS_NULL (node))
    return NULL;

  return json_node_get_string (node);
}

/**
 * json_array_get_null_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 *
 * Conveniently retrieves whether the element at @index_ is set to null
 *
 * See also: json_array_get_element(), JSON_NODE_TYPE(), %JSON_NODE_NULL
 *
 * Return value: %TRUE if the element is null
 *
 * Since: 0.8
 */
gboolean
json_array_get_null_element (JsonArray *array,
                             guint      index_)
{
  JsonNode *node;

  g_return_val_if_fail (array != NULL, FALSE);
  g_return_val_if_fail (index_ < array->elements->len, FALSE);

  node = g_ptr_array_index (array->elements, index_);
  g_return_val_if_fail (node != NULL, FALSE);

  if (JSON_NODE_HOLDS_NULL (node))
    return TRUE;

  if (JSON_NODE_HOLDS_ARRAY (node))
    return json_node_get_array (node) == NULL;

  if (JSON_NODE_HOLDS_OBJECT (node))
    return json_node_get_object (node) == NULL;

  return FALSE;
}

/**
 * json_array_get_array_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 *
 * Conveniently retrieves the array from the element at @index_
 * inside @array
 *
 * See also: json_array_get_element(), json_node_get_array()
 *
 * Return value: (transfer none): the array
 *
 * Since: 0.8
 */
JsonArray *
json_array_get_array_element (JsonArray *array,
                              guint      index_)
{
  JsonNode *node;

  g_return_val_if_fail (array != NULL, NULL);
  g_return_val_if_fail (index_ < array->elements->len, NULL);

  node = g_ptr_array_index (array->elements, index_);
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_HOLDS_ARRAY (node) || JSON_NODE_HOLDS_NULL (node), NULL);

  if (JSON_NODE_HOLDS_NULL (node))
    return NULL;

  return json_node_get_array (node);
}

/**
 * json_array_get_object_element:
 * @array: a #JsonArray
 * @index_: the index of the element to retrieve
 *
 * Conveniently retrieves the object from the element at @index_
 * inside @array
 *
 * See also: json_array_get_element(), json_node_get_object()
 *
 * Return value: (transfer none): the object
 *
 * Since: 0.8
 */
JsonObject *
json_array_get_object_element (JsonArray *array,
                               guint      index_)
{
  JsonNode *node;

  g_return_val_if_fail (array != NULL, NULL);
  g_return_val_if_fail (index_ < array->elements->len, NULL);

  node = g_ptr_array_index (array->elements, index_);
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_HOLDS_OBJECT (node) || JSON_NODE_HOLDS_NULL (node), NULL);

  if (JSON_NODE_HOLDS_NULL (node))
    return NULL;

  return json_node_get_object (node);
}

/**
 * json_array_get_length:
 * @array: a #JsonArray
 *
 * Retrieves the length of a #JsonArray
 *
 * Return value: the length of the array
 */
guint
json_array_get_length (JsonArray *array)
{
  g_return_val_if_fail (array != NULL, 0);

  return array->elements->len;
}

/**
 * json_array_add_element:
 * @array: a #JsonArray
 * @node: (transfer full): a #JsonNode
 *
 * Appends @node inside @array. The array will take ownership of the
 * #JsonNode.
 */
void
json_array_add_element (JsonArray *array,
                        JsonNode  *node)
{
  g_return_if_fail (array != NULL);
  g_return_if_fail (node != NULL);

  g_ptr_array_add (array->elements, node);
}

/**
 * json_array_add_int_element:
 * @array: a #JsonArray
 * @value: an integer value
 *
 * Conveniently adds an integer @value into @array
 *
 * See also: json_array_add_element(), json_node_set_int()
 *
 * Since: 0.8
 */
void
json_array_add_int_element (JsonArray *array,
                            gint64     value)
{
  g_return_if_fail (array != NULL);

  json_array_add_element (array, json_node_init_int (json_node_alloc (), value));
}

/**
 * json_array_add_double_element:
 * @array: a #JsonArray
 * @value: a floating point value
 *
 * Conveniently adds a floating point @value into @array
 *
 * See also: json_array_add_element(), json_node_set_double()
 *
 * Since: 0.8
 */
void
json_array_add_double_element (JsonArray *array,
                               gdouble    value)
{
  g_return_if_fail (array != NULL);

  json_array_add_element (array, json_node_init_double (json_node_alloc (), value));
}

/**
 * json_array_add_boolean_element:
 * @array: a #JsonArray
 * @value: a boolean value
 *
 * Conveniently adds a boolean @value into @array
 *
 * See also: json_array_add_element(), json_node_set_boolean()
 *
 * Since: 0.8
 */
void
json_array_add_boolean_element (JsonArray *array,
                                gboolean   value)
{
  g_return_if_fail (array != NULL);

  json_array_add_element (array, json_node_init_boolean (json_node_alloc (), value));
}

/**
 * json_array_add_string_element:
 * @array: a #JsonArray
 * @value: a string value
 *
 * Conveniently adds a string @value into @array
 *
 * See also: json_array_add_element(), json_node_set_string()
 *
 * Since: 0.8
 */
void
json_array_add_string_element (JsonArray   *array,
                               const gchar *value)
{
  JsonNode *node;

  g_return_if_fail (array != NULL);

  node = json_node_alloc ();

  if (value != NULL)
    json_node_init_string (node, value);
  else
    json_node_init_null (node);

  json_array_add_element (array, node);
}

/**
 * json_array_add_null_element:
 * @array: a #JsonArray
 *
 * Conveniently adds a null element into @array
 *
 * See also: json_array_add_element(), %JSON_NODE_NULL
 *
 * Since: 0.8
 */
void
json_array_add_null_element (JsonArray *array)
{
  g_return_if_fail (array != NULL);

  json_array_add_element (array, json_node_init_null (json_node_alloc ()));
}

/**
 * json_array_add_array_element:
 * @array: a #JsonArray
 * @value: (allow-none) (transfer full): a #JsonArray
 *
 * Conveniently adds an array into @array. The @array takes ownership
 * of the newly added #JsonArray
 *
 * See also: json_array_add_element(), json_node_take_array()
 *
 * Since: 0.8
 */
void
json_array_add_array_element (JsonArray *array,
                              JsonArray *value)
{
  JsonNode *node;

  g_return_if_fail (array != NULL);

  node = json_node_alloc ();

  if (value != NULL)
    {
      json_node_init_array (node, value);
      json_array_unref (value);
    }
  else
    json_node_init_null (node);

  json_array_add_element (array, node);
}

/**
 * json_array_add_object_element:
 * @array: a #JsonArray
 * @value: (transfer full): a #JsonObject
 *
 * Conveniently adds an object into @array. The @array takes ownership
 * of the newly added #JsonObject
 *
 * See also: json_array_add_element(), json_node_take_object()
 *
 * Since: 0.8
 */
void
json_array_add_object_element (JsonArray  *array,
                               JsonObject *value)
{
  JsonNode *node;

  g_return_if_fail (array != NULL);

  node = json_node_alloc ();

  if (value != NULL)
    {
      json_node_init_object (node, value);
      json_object_unref (value);
    }
  else
    json_node_init_null (node);

  json_array_add_element (array, node);
}

/**
 * json_array_remove_element:
 * @array: a #JsonArray
 * @index_: the position of the element to be removed
 *
 * Removes the #JsonNode inside @array at @index_ freeing its allocated
 * resources.
 */
void
json_array_remove_element (JsonArray *array,
                           guint      index_)
{
  g_return_if_fail (array != NULL);
  g_return_if_fail (index_ < array->elements->len);

  json_node_unref (g_ptr_array_remove_index (array->elements, index_));
}

/**
 * json_array_foreach_element:
 * @array: a #JsonArray
 * @func: (scope call): the function to be called on each element
 * @data: (closure): data to be passed to the function
 *
 * Iterates over all elements of @array and calls @func on
 * each one of them.
 *
 * It is safe to change the value of a #JsonNode of the @array
 * from within the iterator @func, but it is not safe to add or
 * remove elements from the @array.
 *
 * Since: 0.8
 */
void
json_array_foreach_element (JsonArray        *array,
                            JsonArrayForeach  func,
                            gpointer          data)
{
  gint i;

  g_return_if_fail (array != NULL);
  g_return_if_fail (func != NULL);

  for (i = 0; i < array->elements->len; i++)
    {
      JsonNode *element_node;

      element_node = g_ptr_array_index (array->elements, i);

      (* func) (array, i, element_node, data);
    }
}

/**
 * json_array_hash:
 * @key: (type JsonArray): a JSON array to hash
 *
 * Calculate a hash value for the given @key (a #JsonArray).
 *
 * The hash is calculated over the array and all its elements, recursively. If
 * the array is immutable, this is a fast operation; otherwise, it scales
 * proportionally with the length of the array.
 *
 * Returns: hash value for @key
 * Since: 1.2
 */
guint
json_array_hash (gconstpointer key)
{
  JsonArray *array;  /* unowned */
  guint hash = 0;
  guint i;

  g_return_val_if_fail (key != NULL, 0);

  array = (JsonArray *) key;

  /* If the array is immutable, we can use the calculated hash. */
  if (array->immutable)
    return array->immutable_hash;

  /* Otherwise, calculate the hash. */
  for (i = 0; i < array->elements->len; i++)
    {
      JsonNode *node = g_ptr_array_index (array->elements, i);
      hash ^= (i ^ json_node_hash (node));
    }

  return hash;
}

/**
 * json_array_equal:
 * @a: (type JsonArray): a JSON array
 * @b: (type JsonArray): another JSON array
 *
 * Check whether @a and @b are equal #JsonArrays, meaning they have the same
 * number of elements, and the values of elements in corresponding positions
 * are equal.
 *
 * Returns: %TRUE if @a and @b are equal; %FALSE otherwise
 * Since: 1.2
 */
gboolean
json_array_equal (gconstpointer a,
                  gconstpointer b)
{
  JsonArray *array_a, *array_b;  /* unowned */
  guint length_a, length_b, i;

  g_return_val_if_fail (a != NULL, FALSE);
  g_return_val_if_fail (b != NULL, FALSE);

  array_a = (JsonArray *) a;
  array_b = (JsonArray *) b;

  /* Identity comparison. */
  if (array_a == array_b)
    return TRUE;

  /* Check lengths. */
  length_a = json_array_get_length (array_a);
  length_b = json_array_get_length (array_b);

  if (length_a != length_b)
    return FALSE;

  /* Check elements. */
  for (i = 0; i < length_a; i++)
    {
      JsonNode *child_a, *child_b;  /* unowned */

      child_a = json_array_get_element (array_a, i);
      child_b = json_array_get_element (array_b, i);

      if (!json_node_equal (child_a, child_b))
        return FALSE;
    }

  return TRUE;
}
