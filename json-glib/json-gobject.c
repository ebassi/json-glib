/* json-gobject.h - JSON GObject integration
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

/**
 * SECTION:json-gobject
 * @short_description: Serialize and deserialize GObjects
 *
 * JSON-GLib provides API for serializing and deserializing #GObject<!-- -->s
 * to and from JSON data streams.
 *
 * Simple #GObject classes can be (de)serialized into JSON objects, if the
 * properties have compatible types with the native JSON types (integers,
 * booleans, strings, string vectors). If the class to be (de)serialized has
 * complex data types for properties (like boxed types or other objects)
 * then the class should implement the provided #JsonSerializable interface
 * and its virtual functions.
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>

#include "json-gobject.h"
#include "json-parser.h"
#include "json-generator.h"

GType
json_serializable_get_type (void)
{
  static GType iface_type = 0;

  if (!iface_type)
    iface_type =
      g_type_register_static_simple (G_TYPE_INTERFACE, "JsonSerializable",
                                     sizeof (JsonSerializableIface),
                                     NULL, 0, NULL, 0);

  return iface_type;
}

/**
 * json_serializable_serialize_property:
 * @serializable: a #JsonSerializable object
 * @property_name: the name of the property
 * @value: the value of the property
 * @pspec: a #GParamSpec
 *
 * Asks a #JsonSerializable implementation to serialize a #GObject
 * property into a #JsonNode object.
 *
 * Return value: a #JsonNode containing the serialize property
 */
JsonNode *
json_serializable_serialize_property (JsonSerializable *serializable,
                                      const gchar      *property_name,
                                      const GValue     *value,
                                      GParamSpec       *pspec)
{
  JsonSerializableIface *iface;

  g_return_val_if_fail (JSON_IS_SERIALIZABLE (serializable), NULL);
  g_return_val_if_fail (property_name != NULL, NULL);
  g_return_val_if_fail (value != NULL, NULL);
  g_return_val_if_fail (pspec != NULL, NULL);

  iface = JSON_SERIALIZABLE_GET_IFACE (serializable);
  if (!iface->serialize_property)
    return json_node_new (JSON_NODE_NULL);

  return iface->serialize_property (serializable, property_name, value, pspec);
}

/**
 * json_serializable_deserialize_property:
 * @serializable: a #JsonSerializable
 * @property_name: the name of the property
 * @value: a pointer to an uninitialized #GValue
 * @pspec: a #GParamSpec
 * @property_node: a #JsonNode containing the serialized property
 *
 * Asks a #JsonSerializable implementation to deserialize the
 * property contained inside @property_node into @value.
 *
 * Return value: %TRUE if the property was successfully deserialized.
 */
gboolean
json_serializable_deserialize_property (JsonSerializable *serializable,
                                        const gchar      *property_name,
                                        GValue           *value,
                                        GParamSpec       *pspec,
                                        JsonNode         *property_node)
{
  JsonSerializableIface *iface;

  g_return_val_if_fail (JSON_IS_SERIALIZABLE (serializable), FALSE);
  g_return_val_if_fail (property_name != NULL, FALSE);
  g_return_val_if_fail (value != NULL, FALSE);
  g_return_val_if_fail (pspec != NULL, FALSE);
  g_return_val_if_fail (property_node != NULL, FALSE);

  iface = JSON_SERIALIZABLE_GET_IFACE (serializable);
  if (!iface->deserialize_property)
    {
      g_param_value_defaults (pspec, value);
      return TRUE;
    }

  return iface->deserialize_property (serializable,
                                      property_name,
                                      value,
                                      pspec,
                                      property_node);
}

static JsonNode *
json_serialize_pspec (const GValue *real_value,
                      GParamSpec   *pspec)
{
  JsonNode *retval = NULL;
  GValue value = { 0, };

  if (!G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (real_value)))
    {
      g_warning ("Complex types are not supported.");
      return NULL;
    }

  switch (G_VALUE_TYPE (real_value))
    {
    case G_TYPE_INT:
    case G_TYPE_BOOLEAN:
    case G_TYPE_DOUBLE:
      /* JSON native types */
      retval = json_node_new (JSON_NODE_VALUE);
      g_value_init (&value, G_VALUE_TYPE (real_value));
      g_value_copy (real_value, &value);
      json_node_set_value (retval, &value);
      g_value_unset (&value);
      break;

    case G_TYPE_STRING:
      /* strings might be NULL */
      if (!g_value_get_string (real_value))
        retval = json_node_new (JSON_NODE_NULL);
      else
        {
          retval = json_node_new (JSON_NODE_VALUE);
          json_node_set_string (retval, g_value_get_string (real_value));
          break;
        }
      break;

    case G_TYPE_BOXED:
      if (G_VALUE_HOLDS (real_value, G_TYPE_STRV))
        {
          gchar **strv = g_value_get_boxed (real_value);
          gint i, strv_len;
          JsonArray *array;

          strv_len = g_strv_length (strv);
          array = json_array_sized_new (strv_len);

          for (i = 0; i < strv_len; i++)
            {
              JsonNode *str = json_node_new (JSON_NODE_VALUE);

              json_node_set_string (str, strv[i]);
              json_array_add_element (array, str);
            }

          retval = json_node_new (JSON_NODE_ARRAY);
          json_node_take_array (retval, array);
        }
      else
        {
          g_warning ("Unsupported type `%s'",
                     g_type_name (G_VALUE_TYPE (real_value)));
        }
      break;

    case G_TYPE_UINT:
    case G_TYPE_LONG:
    case G_TYPE_ULONG:
    case G_TYPE_CHAR:
    case G_TYPE_UCHAR:
    case G_TYPE_ENUM:
    case G_TYPE_FLAGS:
      /* these should fit into an int */
      retval = json_node_new (JSON_NODE_VALUE);
      g_value_init (&value, G_TYPE_INT);
      g_value_copy (real_value, &value);
      json_node_set_value (retval, &value);
      g_value_unset (&value);
      break;

    case G_TYPE_NONE:
      retval = json_node_new (JSON_NODE_NULL);
      break;

    default:
      g_warning ("Unsupported type `%s'",
                 g_type_name (G_VALUE_TYPE (real_value)));
      break;
    }

  return retval;
}

/**
 * json_serialize_gobject:
 * @gobject: a #GObject
 * @length: return value for the length of the buffer, or %NULL
 *
 * Serializes a #GObject into a JSON data stream. If @gobject implements
 * the #JsonSerializableIface interface, it will be responsible to
 * serizalize all its properties; otherwise, the default implementation
 * will be use to translate the compatible types into JSON native types.
 *
 * Return value: a JSON data stream representing the passed #GObject
 */
gchar *
json_serialize_gobject (GObject *gobject,
                        gsize   *length)
{
  JsonSerializableIface *iface = NULL;
  JsonGenerator *gen;
  JsonNode *root;
  JsonObject *object;
  GParamSpec **pspecs;
  guint n_pspecs, i;
  gchar *data;

  g_return_val_if_fail (G_OBJECT (gobject), NULL);

  if (JSON_IS_SERIALIZABLE (gobject))
    iface = JSON_SERIALIZABLE_GET_IFACE (gobject);

  object = json_object_new ();

  root = json_node_new (JSON_NODE_OBJECT);
  json_node_take_object (root, object);

  pspecs = g_object_class_list_properties (G_OBJECT_GET_CLASS (gobject),
                                           &n_pspecs);
  for (i = 0; i < n_pspecs; i++)
    {
      GParamSpec *pspec = pspecs[i];
      GValue value = { 0, };
      JsonNode *node;

      /* read only what we can */
      if (!(pspec->flags & G_PARAM_READABLE))
        continue;

      g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE (pspec));
      g_object_get_property (gobject, pspec->name, &value);

      if (iface && iface->serialize_property)
        {
          JsonSerializable *serializable = JSON_SERIALIZABLE (gobject);

          node = iface->serialize_property (serializable, pspec->name,
                                            &value,
                                            pspec);
        }
      else
        node = json_serialize_pspec (&value, pspec);

      if (node)
        json_object_add_member (object, pspec->name, node);

      g_value_unset (&value);
    }

  g_free (pspecs);

  gen = json_generator_new ();
  json_generator_set_root (gen, root);
  g_object_set (gen, "pretty", TRUE, NULL);
  data = json_generator_to_data (gen, length);
  g_object_unref (gen);

  return data;
}
