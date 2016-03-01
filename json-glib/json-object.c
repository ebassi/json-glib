/* json-object.c - JSON object implementation
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

#include <string.h>
#include <glib.h>

#include "json-types-private.h"

/**
 * SECTION:json-object
 * @short_description: a JSON object representation
 *
 * #JsonObject is the representation of the object type inside JSON. It
 * contains #JsonNodes, which may contain fundamental types, arrays or other
 * objects; each node inside an object, or "member", is accessed using a
 * unique string, or "name".
 *
 * Since objects can be expensive, they are reference counted. You can control
 * the lifetime of a #JsonObject using json_object_ref() and json_object_unref().
 *
 * To add or overwrite a member with a given name, use json_object_set_member().
 * To extract a member with a given name, use json_object_get_member().
 * To retrieve the list of members, use json_object_get_members().
 * To retrieve the size of the object (that is, the number of members it has),
 * use json_object_get_size().
 */

G_DEFINE_BOXED_TYPE (JsonObject, json_object, json_object_ref, json_object_unref);

/**
 * json_object_new: (constructor)
 * 
 * Creates a new #JsonObject, an JSON object type representation.
 *
 * Return value: (transfer full): the newly created #JsonObject
 */
JsonObject *
json_object_new (void)
{
  JsonObject *object;

  object = g_slice_new0 (JsonObject);
  
  object->ref_count = 1;
  object->members = g_hash_table_new_full (g_str_hash, g_str_equal,
                                           g_free,
                                           (GDestroyNotify) json_node_unref);
  object->members_ordered = NULL;

  return object;
}

/**
 * json_object_ref:
 * @object: a #JsonObject
 *
 * Increase by one the reference count of a #JsonObject.
 *
 * Return value: (transfer none): the passed #JsonObject, with the reference count
 *   increased by one.
 */
JsonObject *
json_object_ref (JsonObject *object)
{
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (object->ref_count > 0, NULL);

  object->ref_count++;

  return object;
}

/**
 * json_object_unref:
 * @object: a #JsonObject
 *
 * Decreases by one the reference count of a #JsonObject. If the
 * reference count reaches zero, the object is destroyed and all
 * its allocated resources are freed.
 */
void
json_object_unref (JsonObject *object)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->ref_count > 0);

  if (--object->ref_count == 0)
    {
      g_list_free (object->members_ordered);
      g_hash_table_destroy (object->members);
      object->members_ordered = NULL;
      object->members = NULL;

      g_slice_free (JsonObject, object);
    }
}

/**
 * json_object_seal:
 * @object: a #JsonObject
 *
 * Seals the #JsonObject, making it immutable to further changes. This will
 * recursively seal all members of the object too.
 *
 * If the @object is already immutable, this is a no-op.
 *
 * Since: 1.2
 */
void
json_object_seal (JsonObject *object)
{
  JsonObjectIter iter;
  JsonNode *node;

  g_return_if_fail (object != NULL);
  g_return_if_fail (object->ref_count > 0);

  if (object->immutable)
    return;

  /* Propagate to all members. */
  json_object_iter_init (&iter, object);

  while (json_object_iter_next (&iter, NULL, &node))
    json_node_seal (node);

  object->immutable_hash = json_object_hash (object);
  object->immutable = TRUE;
}

/**
 * json_object_is_immutable:
 * @object: a #JsonObject
 *
 * Check whether the given @object has been marked as immutable by calling
 * json_object_seal() on it.
 *
 * Since: 1.2
 * Returns: %TRUE if the @object is immutable
 */
gboolean
json_object_is_immutable (JsonObject *object)
{
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (object->ref_count > 0, FALSE);

  return object->immutable;
}

static inline void
object_set_member_internal (JsonObject  *object,
                            const gchar *member_name,
                            JsonNode    *node)
{
  gchar *name = g_strdup (member_name);

  if (g_hash_table_lookup (object->members, name) == NULL)
    object->members_ordered = g_list_prepend (object->members_ordered, name);
  else
    {
      GList *l;

      /* if the member already exists then we need to replace the
       * pointer to its name, to avoid keeping invalid pointers
       * once we replace the key in the hash table
       */
      l = g_list_find_custom (object->members_ordered, name, (GCompareFunc) strcmp);
      if (l != NULL)
        l->data = name;
    }

  g_hash_table_replace (object->members, name, node);
}

/**
 * json_object_add_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 * @node: (transfer full): the value of the member
 *
 * Adds a member named @member_name and containing @node into a #JsonObject.
 * The object will take ownership of the #JsonNode.
 *
 * This function will return if the @object already contains a member
 * @member_name.
 *
 * Deprecated: 0.8: Use json_object_set_member() instead
 */
void
json_object_add_member (JsonObject  *object,
                        const gchar *member_name,
                        JsonNode    *node)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);
  g_return_if_fail (node != NULL);

  if (json_object_has_member (object, member_name))
    {
      g_warning ("JsonObject already has a `%s' member of type `%s'",
                 member_name,
                 json_node_type_name (node));
      return;
    }

  object_set_member_internal (object, member_name, node);
}

/**
 * json_object_set_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 * @node: (transfer full): the value of the member
 *
 * Sets @node as the value of @member_name inside @object.
 *
 * If @object already contains a member called @member_name then
 * the member's current value is overwritten. Otherwise, a new
 * member is added to @object.
 *
 * Since: 0.8
 */
void
json_object_set_member (JsonObject  *object,
                        const gchar *member_name,
                        JsonNode    *node)
{
  JsonNode *old_node;

  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);
  g_return_if_fail (node != NULL);

  old_node = g_hash_table_lookup (object->members, member_name);
  if (old_node == NULL)
    goto set_member;

  if (old_node == node)
    return;

set_member:
  object_set_member_internal (object, member_name, node);
}

/**
 * json_object_set_int_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 * @value: the value of the member
 *
 * Convenience function for setting an integer @value of
 * @member_name inside @object.
 *
 * See also: json_object_set_member()
 *
 * Since: 0.8
 */
void
json_object_set_int_member (JsonObject  *object,
                            const gchar *member_name,
                            gint64       value)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);

  object_set_member_internal (object, member_name, json_node_init_int (json_node_alloc (), value));
}

/**
 * json_object_set_double_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 * @value: the value of the member
 *
 * Convenience function for setting a floating point @value
 * of @member_name inside @object.
 *
 * See also: json_object_set_member()
 *
 * Since: 0.8
 */
void
json_object_set_double_member (JsonObject  *object,
                               const gchar *member_name,
                               gdouble      value)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);

  object_set_member_internal (object, member_name, json_node_init_double (json_node_alloc (), value));
}

/**
 * json_object_set_boolean_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 * @value: the value of the member
 *
 * Convenience function for setting a boolean @value of
 * @member_name inside @object.
 *
 * See also: json_object_set_member()
 *
 * Since: 0.8
 */
void
json_object_set_boolean_member (JsonObject  *object,
                                const gchar *member_name,
                                gboolean     value)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);

  object_set_member_internal (object, member_name, json_node_init_boolean (json_node_alloc (), value));
}

/**
 * json_object_set_string_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 * @value: the value of the member
 *
 * Convenience function for setting a string @value of
 * @member_name inside @object.
 *
 * See also: json_object_set_member()
 *
 * Since: 0.8
 */
void
json_object_set_string_member (JsonObject  *object,
                               const gchar *member_name,
                               const gchar *value)
{
  JsonNode *node;

  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);

  node = json_node_alloc ();

  if (value != NULL)
    json_node_init_string (node, value);
  else
    json_node_init_null (node);

  object_set_member_internal (object, member_name, node);
}

/**
 * json_object_set_null_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 *
 * Convenience function for setting a null @value of
 * @member_name inside @object.
 *
 * See also: json_object_set_member()
 *
 * Since: 0.8
 */
void
json_object_set_null_member (JsonObject  *object,
                             const gchar *member_name)
{
  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);

  object_set_member_internal (object, member_name, json_node_init_null (json_node_alloc ()));
}

/**
 * json_object_set_array_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 * @value: (transfer full): the value of the member
 *
 * Convenience function for setting an array @value of
 * @member_name inside @object.
 *
 * The @object will take ownership of the passed #JsonArray
 *
 * See also: json_object_set_member()
 *
 * Since: 0.8
 */
void
json_object_set_array_member (JsonObject  *object,
                              const gchar *member_name,
                              JsonArray   *value)
{
  JsonNode *node;

  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);

  node = json_node_alloc ();

  if (value != NULL)
    {
      json_node_init_array (node, value);
      json_array_unref (value);
    }
  else
    json_node_init_null (node);

  object_set_member_internal (object, member_name, node);
}

/**
 * json_object_set_object_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 * @value: (transfer full): the value of the member
 *
 * Convenience function for setting an object @value of
 * @member_name inside @object.
 *
 * The @object will take ownership of the passed #JsonObject
 *
 * See also: json_object_set_member()
 *
 * Since: 0.8
 */
void
json_object_set_object_member (JsonObject  *object,
                               const gchar *member_name,
                               JsonObject  *value)
{
  JsonNode *node;

  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);

  node = json_node_alloc ();

  if (value != NULL)
    {
      json_node_init_object (node, value);
      json_object_unref (value);
    }
  else
    json_node_init_null (node);

  object_set_member_internal (object, member_name, node);
}

/**
 * json_object_get_members:
 * @object: a #JsonObject
 *
 * Retrieves all the names of the members of a #JsonObject. You can
 * obtain the value for each member using json_object_get_member().
 *
 * Return value: (element-type utf8) (transfer container): a #GList
 *   of member names. The content of the list is owned by the #JsonObject
 *   and should never be modified or freed. When you have finished using
 *   the returned list, use g_list_free() to free the resources it has
 *   allocated.
 */
GList *
json_object_get_members (JsonObject *object)
{
  GList *copy;

  g_return_val_if_fail (object != NULL, NULL);

  copy = g_list_copy (object->members_ordered);

  return g_list_reverse (copy);
}

/**
 * json_object_get_values:
 * @object: a #JsonObject
 *
 * Retrieves all the values of the members of a #JsonObject.
 *
 * Return value: (element-type JsonNode) (transfer container): a #GList of
 *   #JsonNodes. The content of the list is owned by the #JsonObject
 *   and should never be modified or freed. When you have finished using the
 *   returned list, use g_list_free() to free the resources it has allocated.
 */
GList *
json_object_get_values (JsonObject *object)
{
  GList *values, *l;

  g_return_val_if_fail (object != NULL, NULL);

  values = NULL;
  for (l = object->members_ordered; l != NULL; l = l->next)
    values = g_list_prepend (values, g_hash_table_lookup (object->members, l->data));

  return values;
}

/**
 * json_object_dup_member:
 * @object: a #JsonObject
 * @member_name: the name of the JSON object member to access
 *
 * Retrieves a copy of the #JsonNode containing the value of @member_name
 * inside a #JsonObject
 *
 * Return value: (transfer full): a copy of the node for the requested
 *   object member or %NULL. Use json_node_unref() when done.
 *
 * Since: 0.6
 */
JsonNode *
json_object_dup_member (JsonObject  *object,
                        const gchar *member_name)
{
  JsonNode *retval;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (member_name != NULL, NULL);

  retval = json_object_get_member (object, member_name);
  if (!retval)
    return NULL;

  return json_node_copy (retval);
}

static inline JsonNode *
object_get_member_internal (JsonObject  *object,
                            const gchar *member_name)
{
  return g_hash_table_lookup (object->members, member_name);
}

/**
 * json_object_get_member:
 * @object: a #JsonObject
 * @member_name: the name of the JSON object member to access
 *
 * Retrieves the #JsonNode containing the value of @member_name inside
 * a #JsonObject.
 *
 * Return value: (transfer none): a pointer to the node for the requested object
 *   member, or %NULL
 */
JsonNode *
json_object_get_member (JsonObject  *object,
                        const gchar *member_name)
{
  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (member_name != NULL, NULL);

  return object_get_member_internal (object, member_name);
}

/**
 * json_object_get_int_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 *
 * Convenience function that retrieves the integer value
 * stored in @member_name of @object
 *
 * See also: json_object_get_member()
 *
 * Return value: the integer value of the object's member
 *
 * Since: 0.8
 */
gint64
json_object_get_int_member (JsonObject  *object,
                            const gchar *member_name)
{
  JsonNode *node;

  g_return_val_if_fail (object != NULL, 0);
  g_return_val_if_fail (member_name != NULL, 0);

  node = object_get_member_internal (object, member_name);
  g_return_val_if_fail (node != NULL, 0);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE, 0);

  return json_node_get_int (node);
}

/**
 * json_object_get_double_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 *
 * Convenience function that retrieves the floating point value
 * stored in @member_name of @object
 *
 * See also: json_object_get_member()
 *
 * Return value: the floating point value of the object's member
 *
 * Since: 0.8
 */
gdouble
json_object_get_double_member (JsonObject  *object,
                               const gchar *member_name)
{
  JsonNode *node;

  g_return_val_if_fail (object != NULL, 0.0);
  g_return_val_if_fail (member_name != NULL, 0.0);

  node = object_get_member_internal (object, member_name);
  g_return_val_if_fail (node != NULL, 0.0);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE, 0.0);

  return json_node_get_double (node);
}

/**
 * json_object_get_boolean_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 *
 * Convenience function that retrieves the boolean value
 * stored in @member_name of @object
 *
 * See also: json_object_get_member()
 *
 * Return value: the boolean value of the object's member
 *
 * Since: 0.8
 */
gboolean
json_object_get_boolean_member (JsonObject  *object,
                                const gchar *member_name)
{
  JsonNode *node;

  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (member_name != NULL, FALSE);

  node = object_get_member_internal (object, member_name);
  g_return_val_if_fail (node != NULL, FALSE);
  g_return_val_if_fail (JSON_NODE_TYPE (node) == JSON_NODE_VALUE, FALSE);

  return json_node_get_boolean (node);
}

/**
 * json_object_get_null_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 *
 * Convenience function that checks whether the value
 * stored in @member_name of @object is null
 *
 * See also: json_object_get_member()
 *
 * Return value: %TRUE if the value is null
 *
 * Since: 0.8
 */
gboolean
json_object_get_null_member (JsonObject  *object,
                             const gchar *member_name)
{
  JsonNode *node;

  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (member_name != NULL, FALSE);

  node = object_get_member_internal (object, member_name);
  g_return_val_if_fail (node != NULL, FALSE);

  if (JSON_NODE_HOLDS_NULL (node))
    return TRUE;

  if (JSON_NODE_HOLDS_OBJECT (node))
    return json_node_get_object (node) == NULL;

  if (JSON_NODE_HOLDS_ARRAY (node))
    return json_node_get_array (node) == NULL;

  return FALSE;
}

/**
 * json_object_get_string_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 *
 * Convenience function that retrieves the string value
 * stored in @member_name of @object
 *
 * See also: json_object_get_member()
 *
 * Return value: the string value of the object's member
 *
 * Since: 0.8
 */
const gchar *
json_object_get_string_member (JsonObject  *object,
                               const gchar *member_name)
{
  JsonNode *node;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (member_name != NULL, NULL);

  node = object_get_member_internal (object, member_name);
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_HOLDS_VALUE (node) || JSON_NODE_HOLDS_NULL (node), NULL);

  if (JSON_NODE_HOLDS_NULL (node))
    return NULL;

  return json_node_get_string (node);
}

/**
 * json_object_get_array_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 *
 * Convenience function that retrieves the array
 * stored in @member_name of @object
 *
 * See also: json_object_get_member()
 *
 * Return value: (transfer none): the array inside the object's member
 *
 * Since: 0.8
 */
JsonArray *
json_object_get_array_member (JsonObject  *object,
                              const gchar *member_name)
{
  JsonNode *node;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (member_name != NULL, NULL);

  node = object_get_member_internal (object, member_name);
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_HOLDS_ARRAY (node) || JSON_NODE_HOLDS_NULL (node), NULL);

  if (JSON_NODE_HOLDS_NULL (node))
    return NULL;

  return json_node_get_array (node);
}

/**
 * json_object_get_object_member:
 * @object: a #JsonObject
 * @member_name: the name of the member
 *
 * Convenience function that retrieves the object
 * stored in @member_name of @object. It is an error to specify a @member_name
 * which does not exist.
 *
 * See also: json_object_get_member()
 *
 * Return value: (transfer none) (nullable): the object inside the object’s
 *    member, or %NULL if the value for the member is `null`
 *
 * Since: 0.8
 */
JsonObject *
json_object_get_object_member (JsonObject  *object,
                               const gchar *member_name)
{
  JsonNode *node;

  g_return_val_if_fail (object != NULL, NULL);
  g_return_val_if_fail (member_name != NULL, NULL);

  node = object_get_member_internal (object, member_name);
  g_return_val_if_fail (node != NULL, NULL);
  g_return_val_if_fail (JSON_NODE_HOLDS_OBJECT (node) || JSON_NODE_HOLDS_NULL (node), NULL);

  if (JSON_NODE_HOLDS_NULL (node))
    return NULL;

  return json_node_get_object (node);
}

/**
 * json_object_has_member:
 * @object: a #JsonObject
 * @member_name: the name of a JSON object member
 *
 * Checks whether @object has a member named @member_name.
 *
 * Return value: %TRUE if the JSON object has the requested member
 */
gboolean
json_object_has_member (JsonObject *object,
                        const gchar *member_name)
{
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (member_name != NULL, FALSE);

  return (g_hash_table_lookup (object->members, member_name) != NULL);
}

/**
 * json_object_get_size:
 * @object: a #JsonObject
 *
 * Retrieves the number of members of a #JsonObject.
 *
 * Return value: the number of members
 */
guint
json_object_get_size (JsonObject *object)
{
  g_return_val_if_fail (object != NULL, 0);

  return g_hash_table_size (object->members);
}

/**
 * json_object_remove_member:
 * @object: a #JsonObject
 * @member_name: the name of the member to remove
 *
 * Removes @member_name from @object, freeing its allocated resources.
 */
void
json_object_remove_member (JsonObject  *object,
                           const gchar *member_name)
{
  GList *l;

  g_return_if_fail (object != NULL);
  g_return_if_fail (member_name != NULL);

  for (l = object->members_ordered; l != NULL; l = l->next)
    {
      const gchar *name = l->data;

      if (g_strcmp0 (name, member_name) == 0)
        {
          object->members_ordered = g_list_delete_link (object->members_ordered, l);
          break;
        }
    }

  g_hash_table_remove (object->members, member_name);
}

/**
 * json_object_foreach_member:
 * @object: a #JsonObject
 * @func: (scope call): the function to be called on each member
 * @data: (closure): data to be passed to the function
 *
 * Iterates over all members of @object and calls @func on
 * each one of them.
 *
 * It is safe to change the value of a #JsonNode of the @object
 * from within the iterator @func, but it is not safe to add or
 * remove members from the @object.
 *
 * Since: 0.8
 */
void
json_object_foreach_member (JsonObject        *object,
                            JsonObjectForeach  func,
                            gpointer           data)
{
  GList *members, *l;

  g_return_if_fail (object != NULL);
  g_return_if_fail (func != NULL);

  /* the list is stored in reverse order to have constant time additions */
  members = g_list_last (object->members_ordered);
  for (l = members; l != NULL; l = l->prev)
    {
      const gchar *member_name = l->data;
      JsonNode *member_node = g_hash_table_lookup (object->members, member_name);

      func (object, member_name, member_node, data);
    }
}

/**
 * json_object_hash:
 * @key: (type JsonObject): a JSON object to hash
 *
 * Calculate a hash value for the given @key (a #JsonObject).
 *
 * The hash is calculated over the object and all its members, recursively. If
 * the object is immutable, this is a fast operation; otherwise, it scales
 * proportionally with the number of members in the object.
 *
 * Returns: hash value for @key
 * Since: 1.2
 */
guint
json_object_hash (gconstpointer key)
{
  JsonObject *object = (JsonObject *) key;
  guint hash = 0;
  JsonObjectIter iter;
  const gchar *member_name;
  JsonNode *node;

  g_return_val_if_fail (object != NULL, 0);

  /* If the object is immutable, use the cached hash. */
  if (object->immutable)
    return object->immutable_hash;

  /* Otherwise, calculate from scratch. */
  json_object_iter_init (&iter, object);

  while (json_object_iter_next (&iter, &member_name, &node))
    hash ^= (json_string_hash (member_name) ^ json_node_hash (node));

  return hash;
}

/**
 * json_object_equal:
 * @a: (type JsonObject): a JSON object
 * @b: (type JsonObject): another JSON object
 *
 * Check whether @a and @b are equal #JsonObjects, meaning they have the same
 * set of members, and the values of corresponding members are equal.
 *
 * Returns: %TRUE if @a and @b are equal; %FALSE otherwise
 * Since: 1.2
 */
gboolean
json_object_equal (gconstpointer  a,
                   gconstpointer  b)
{
  JsonObject *object_a, *object_b;
  guint size_a, size_b;
  JsonObjectIter iter_a;
  JsonNode *child_a, *child_b;  /* unowned */
  const gchar *member_name;

  object_a = (JsonObject *) a;
  object_b = (JsonObject *) b;

  /* Identity comparison. */
  if (object_a == object_b)
    return TRUE;

  /* Check sizes. */
  size_a = json_object_get_size (object_a);
  size_b = json_object_get_size (object_b);

  if (size_a != size_b)
    return FALSE;

  /* Check member names and values. Check the member names first
   * to avoid expensive recursive value comparisons which might
   * be unnecessary. */
  json_object_iter_init (&iter_a, object_a);

  while (json_object_iter_next (&iter_a, &member_name, NULL))
    {
      if (!json_object_has_member (object_b, member_name))
        return FALSE;
    }

  json_object_iter_init (&iter_a, object_a);

  while (json_object_iter_next (&iter_a, &member_name, &child_a))
    {
      child_b = json_object_get_member (object_b, member_name);

      if (!json_node_equal (child_a, child_b))
        return FALSE;
    }

  return TRUE;
}

/**
 * json_object_iter_init:
 * @iter: an uninitialised #JsonObjectIter
 * @object: the #JsonObject to iterate over
 *
 * Initialise the @iter and associate it with @object.
 *
 * |[<!-- language="C" -->
 * JsonObjectIter iter;
 * const gchar *member_name;
 * JsonNode *member_node;
 *
 * json_object_iter_init (&iter, some_object);
 * while (json_object_iter_next (&iter, &member_name, &member_node))
 *   {
 *     // Do something with @member_name and @member_node.
 *   }
 * ]|
 *
 * Since: 1.2
 */
void
json_object_iter_init (JsonObjectIter  *iter,
                       JsonObject      *object)
{
  JsonObjectIterReal *iter_real = (JsonObjectIterReal *) iter;;

  g_return_if_fail (iter != NULL);
  g_return_if_fail (object != NULL);
  g_return_if_fail (object->ref_count > 0);

  iter_real->object = object;
  g_hash_table_iter_init (&iter_real->members_iter, object->members);
}

/**
 * json_object_iter_next:
 * @iter: a #JsonObjectIter
 * @member_name: (out callee-allocates) (transfer none) (optional): return
 *    location for the member name, or %NULL to ignore
 * @member_node: (out callee-allocates) (transfer none) (optional): return
 *    location for the member value, or %NULL to ignore
 *
 * Advance @iter and retrieve the next member in the object. If the end of the
 * object is reached, %FALSE is returned and @member_name and @member_node are
 * set to invalid values. After that point, the @iter is invalid.
 *
 * The order in which members are returned by the iterator is undefined. The
 * iterator is invalidated if its #JsonObject is modified during iteration.
 *
 * Returns: %TRUE if @member_name and @member_node are valid; %FALSE if the end
 *    of the object has been reached
 *
 * Since: 1.2
 */
gboolean
json_object_iter_next (JsonObjectIter  *iter,
                       const gchar    **member_name,
                       JsonNode       **member_node)
{
  JsonObjectIterReal *iter_real = (JsonObjectIterReal *) iter;

  g_return_val_if_fail (iter != NULL, FALSE);
  g_return_val_if_fail (iter_real->object != NULL, FALSE);
  g_return_val_if_fail (iter_real->object->ref_count > 0, FALSE);

  return g_hash_table_iter_next (&iter_real->members_iter,
                                 (gpointer *) member_name,
                                 (gpointer *) member_node);
}
