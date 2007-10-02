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
 * The simplest form to serialize a #GObject class is calling the
 * json_serialize_gobject() function on a #GObject instance: every
 * property using a fundamental type (or a type that can be coherced
 * into a fundamental type) will be converted into a JSON type.
 */

#include "config.h"

#include <string.h>
#include <stdlib.h>

#include "json-gobject.h"
#include "json-parser.h"
#include "json-generator.h"

static JsonNode *
json_serialize_pspec (GObject    *gobject,
                      GParamSpec *pspec)
{
  JsonNode *retval = NULL;
  GValue real_value = { 0, };
  GValue value = { 0, };

  g_value_init (&real_value, G_PARAM_SPEC_VALUE_TYPE (pspec));
  g_object_get_property (gobject, pspec->name, &real_value);
  
  if (!G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (&real_value)))
    {
      g_warning ("Complex types are not supported.");
      return NULL;
    }

  switch (G_VALUE_TYPE (&real_value))
    {
    case G_TYPE_INT:
    case G_TYPE_BOOLEAN:
    case G_TYPE_FLOAT:
      /* JSON native types */
      retval = json_node_new (JSON_NODE_VALUE);
      g_value_init (&value, G_VALUE_TYPE (&real_value));
      g_value_copy (&real_value, &value);
      json_node_set_value (retval, &value);
      g_value_unset (&value);
      break;

    case G_TYPE_STRING:
      /* strings might be NULL */
      if (!g_value_get_string (&real_value))
        retval = json_node_new (JSON_NODE_NULL);
      else
        {
          retval = json_node_new (JSON_NODE_VALUE);
          g_value_init (&value, G_TYPE_STRING);
          g_value_set_string (&value, g_value_get_string (&real_value));
          json_node_set_value (retval, &value);
          g_value_unset (&value);
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
      g_value_copy (&real_value, &value);
      json_node_set_value (retval, &value);
      g_value_unset (&value);
      break;

    case G_TYPE_NONE:
      retval = json_node_new (JSON_NODE_NULL);
      break;

    default:
      g_warning ("Unsupported type `%s'",
                 g_type_name (G_VALUE_TYPE (&real_value)));
      break;
    }

  g_value_unset (&real_value);

  return retval;
}

/**
 * json_serialize_gobject:
 * @gobject: a #GObject
 * @length: return value for the length of the buffer, or %NULL
 *
 * Serializes a #GObject into a JSON data stream.
 *
 * Return value: a JSON data stream representing the passed #GObject
 */
gchar *
json_serialize_gobject (GObject *gobject,
                        gsize   *length)
{
  JsonGenerator *gen;
  JsonNode *root;
  JsonObject *object;
  GParamSpec **pspecs;
  guint n_pspecs, i;
  gchar *data;

  g_return_val_if_fail (G_IS_OBJECT (gobject), NULL);

  object = json_object_new ();

  root = json_node_new (JSON_NODE_OBJECT);
  json_node_take_object (root, object);

  pspecs = g_object_class_list_properties (G_OBJECT_GET_CLASS (gobject),
                                           &n_pspecs);

  for (i = 0; i < n_pspecs; i++)
    {
      GParamSpec *pspec = pspecs[i];
      JsonNode *value;

      /* read only what we can */
      if (!(pspec->flags & G_PARAM_READABLE))
        continue;

      value = json_serialize_pspec (gobject, pspec);
      if (value)
        json_object_add_member (object, pspec->name, value);
    }

  g_free (pspecs);

  gen = json_generator_new ();
  json_generator_set_root (gen, root);
  g_object_set (gen, "pretty", TRUE, NULL);
  data = json_generator_to_data (gen, length);
  g_object_unref (gen);

  return data;
}
