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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include "json-types-private.h"

#include "json-gobject.h"
#include "json-parser.h"
#include "json-generator.h"

/* forward declaration */
static JsonNode *json_serialize_pspec   (const GValue *real_value,
                                         GParamSpec   *pspec);
static gboolean  json_deserialize_pspec (GValue       *value,
                                         GParamSpec   *pspec,
                                         JsonNode     *node);

static gboolean
enum_from_string (GType        type,
                  const gchar *string,
                  gint        *enum_value)
{
  GEnumClass *eclass;
  GEnumValue *ev;
  gchar *endptr;
  gint value;
  gboolean retval = TRUE;

  g_return_val_if_fail (G_TYPE_IS_ENUM (type), 0);
  g_return_val_if_fail (string != NULL, 0);

  value = strtoul (string, &endptr, 0);
  if (endptr != string) /* parsed a number */
    *enum_value = value;
  else
    {
      eclass = g_type_class_ref (type);
      ev = g_enum_get_value_by_name (eclass, string);
      if (!ev)
	ev = g_enum_get_value_by_nick (eclass, string);

      if (ev)
	*enum_value = ev->value;
      else
        retval = FALSE;

      g_type_class_unref (eclass);
    }

  return retval;
}

static gboolean
flags_from_string (GType        type,
                   const gchar *string,
                   gint        *flags_value)
{
  GFlagsClass *fclass;
  gchar *endptr, *prevptr;
  guint i, j, ret, value;
  gchar *flagstr;
  GFlagsValue *fv;
  const gchar *flag;
  gunichar ch;
  gboolean eos;

  g_return_val_if_fail (G_TYPE_IS_FLAGS (type), 0);
  g_return_val_if_fail (string != 0, 0);

  ret = TRUE;

  value = strtoul (string, &endptr, 0);
  if (endptr != string) /* parsed a number */
    *flags_value = value;
  else
    {
      fclass = g_type_class_ref (type);

      flagstr = g_strdup (string);
      for (value = i = j = 0; ; i++)
	{
	  eos = flagstr[i] == '\0';

	  if (!eos && flagstr[i] != '|')
	    continue;

	  flag = &flagstr[j];
	  endptr = &flagstr[i];

	  if (!eos)
	    {
	      flagstr[i++] = '\0';
	      j = i;
	    }

	  /* trim spaces */
	  for (;;)
	    {
	      ch = g_utf8_get_char (flag);
	      if (!g_unichar_isspace (ch))
		break;
	      flag = g_utf8_next_char (flag);
	    }

	  while (endptr > flag)
	    {
	      prevptr = g_utf8_prev_char (endptr);
	      ch = g_utf8_get_char (prevptr);
	      if (!g_unichar_isspace (ch))
		break;
	      endptr = prevptr;
	    }

	  if (endptr > flag)
	    {
	      *endptr = '\0';
	      fv = g_flags_get_value_by_name (fclass, flag);

	      if (!fv)
		fv = g_flags_get_value_by_nick (fclass, flag);

	      if (fv)
		value |= fv->value;
	      else
		{
		  ret = FALSE;
		  break;
		}
	    }

	  if (eos)
	    {
	      *flags_value = value;
	      break;
	    }
	}

      g_free (flagstr);

      g_type_class_unref (fclass);
    }

  return ret;
}

static GObject *
json_gobject_new (GType       gtype,
                  JsonObject *object)
{
  JsonSerializableIface *iface = NULL;
  JsonSerializable *serializable = NULL;
  gboolean deserialize_property;
  GList *members, *l;
  guint n_members;
  GObjectClass *klass;
  GObject *retval;

  klass = g_type_class_ref (gtype);
  retval = g_object_new (gtype, NULL);

  if (g_type_is_a (gtype, JSON_TYPE_SERIALIZABLE))
    {
      serializable = JSON_SERIALIZABLE (retval);
      iface = JSON_SERIALIZABLE_GET_IFACE (serializable);
      deserialize_property = (iface->deserialize_property != NULL);
    }
  else
    deserialize_property = FALSE;

  g_object_freeze_notify (retval);

  n_members = json_object_get_size (object);
  members = json_object_get_members (object);

  for (l = members; l != NULL; l = l->next)
    {
      const gchar *member_name = l->data;
      GParamSpec *pspec;
      JsonNode *val;
      GValue value = { 0, };
      gboolean res = FALSE;

      pspec = g_object_class_find_property (klass, member_name);
      if (!pspec)
        continue;

      if (pspec->flags & G_PARAM_CONSTRUCT_ONLY)
        continue;

      if (!(pspec->flags & G_PARAM_WRITABLE))
        continue;

      g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE (pspec));

      val = json_object_get_member (object, member_name);

      if (deserialize_property)
        {
          res = iface->deserialize_property (serializable, pspec->name,
                                             &value,
                                             pspec,
                                             val);
        }

      if (!res)
        res = json_deserialize_pspec (&value, pspec, val);

      if (res)
        g_object_set_property (retval, pspec->name, &value);

      g_value_unset (&value);
    }

  g_list_free (members);

  g_object_thaw_notify (retval);

  g_type_class_unref (klass);

  return retval;
}

static JsonObject *
json_gobject_dump (GObject *gobject)
{
  JsonSerializableIface *iface = NULL;
  JsonSerializable *serializable = NULL;
  gboolean serialize_property = FALSE;
  JsonObject *object;
  GParamSpec **pspecs;
  guint n_pspecs, i;

  if (JSON_IS_SERIALIZABLE (gobject))
    {
      serializable = JSON_SERIALIZABLE (gobject);
      iface = JSON_SERIALIZABLE_GET_IFACE (gobject);
      serialize_property = (iface->serialize_property != NULL);
    }

  object = json_object_new ();

  pspecs = g_object_class_list_properties (G_OBJECT_GET_CLASS (gobject), &n_pspecs);
  for (i = 0; i < n_pspecs; i++)
    {
      GParamSpec *pspec = pspecs[i];
      GValue value = { 0, };
      JsonNode *node = NULL;

      /* read only what we can */
      if (!(pspec->flags & G_PARAM_READABLE))
        continue;

      g_value_init (&value, G_PARAM_SPEC_VALUE_TYPE (pspec));
      g_object_get_property (gobject, pspec->name, &value);

      if (serialize_property)
        {
          node = iface->serialize_property (serializable, pspec->name,
                                            &value,
                                            pspec);
        }

      if (!node)
        node = json_serialize_pspec (&value, pspec);

      if (node)
        json_object_set_member (object, pspec->name, node);

      g_value_unset (&value);
    }

  g_free (pspecs);

  return object;
}

static gboolean
json_deserialize_pspec (GValue     *value,
                        GParamSpec *pspec,
                        JsonNode   *node)
{
  GValue node_value = { 0, };
  gboolean retval = FALSE;

  switch (JSON_NODE_TYPE (node))
    {
    case JSON_NODE_OBJECT:
      if (g_type_is_a (G_VALUE_TYPE (value), G_TYPE_OBJECT))
        {
          GObject *object;

          object = json_gobject_new (G_VALUE_TYPE (value), json_node_get_object (node));
          if (object != NULL)
            g_value_take_object (value, object);
          else
            g_value_set_object (value, NULL);

          retval = TRUE;
        }
      break;

    case JSON_NODE_ARRAY:
      if (G_VALUE_HOLDS (value, G_TYPE_STRV))
        {
          JsonArray *array = json_node_get_array (node);
          guint i, array_len = json_array_get_length (array);
          GPtrArray *str_array = g_ptr_array_sized_new (array_len + 1);

          for (i = 0; i < array_len; i++)
            {
              JsonNode *val = json_array_get_element (array, i);

              if (JSON_NODE_TYPE (val) != JSON_NODE_VALUE)
                continue;

              if (json_node_get_string (val) != NULL);
                g_ptr_array_add (str_array, (gpointer) json_node_get_string (val));
            }

          g_ptr_array_add (str_array, NULL);

          g_value_set_boxed (value, str_array->pdata);

          g_ptr_array_free (str_array, TRUE);

          retval = TRUE;
        }
      break;

    case JSON_NODE_VALUE:
      json_node_get_value (node, &node_value);
#if 0
      {
        gchar *node_str = g_strdup_value_contents (&node_value);
        g_debug ("%s: value type '%s' := node value type '%s' -> '%s'",
                 G_STRLOC,
                 g_type_name (G_VALUE_TYPE (value)),
                 g_type_name (G_VALUE_TYPE (&node_value)),
                 node_str);
        g_free (node_str);
      }
#endif

      switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (value)))
        {
        case G_TYPE_BOOLEAN:
        case G_TYPE_INT64:
        case G_TYPE_DOUBLE:
        case G_TYPE_STRING:
          g_value_copy (&node_value, value);
          retval = TRUE;
          break;

        case G_TYPE_INT:
          g_value_set_int (value, (gint) g_value_get_int64 (&node_value));
          retval = TRUE;
          break;

        case G_TYPE_CHAR:
          g_value_set_char (value, (gchar) g_value_get_int64 (&node_value));
          retval = TRUE;
          break;

        case G_TYPE_UINT:
          g_value_set_uint (value, (guint) g_value_get_int64 (&node_value));
          retval = TRUE;
          break;

        case G_TYPE_UCHAR:
          g_value_set_uchar (value, (guchar) g_value_get_int64 (&node_value));
          retval = TRUE;
          break;

        case G_TYPE_FLOAT:
          g_value_set_float (value, (gfloat) g_value_get_double (&node_value));
          retval = TRUE;
          break;

        case G_TYPE_ENUM:
          {
            gint enum_value;

            if (G_VALUE_HOLDS (&node_value, G_TYPE_INT64))
              {
                enum_value = g_value_get_int64 (&node_value);
                retval = TRUE;
              }
            else if (G_VALUE_HOLDS (&node_value, G_TYPE_STRING))
              {
                retval = enum_from_string (G_VALUE_TYPE (value),
                                           g_value_get_string (&node_value),
                                           &enum_value);
              }

            if (retval)
              g_value_set_enum (value, enum_value);
          }
          break;

        case G_TYPE_FLAGS:
          {
            gint flags_value;

            if (G_VALUE_HOLDS (&node_value, G_TYPE_INT64))
              {
                flags_value = g_value_get_int64 (&node_value);
                retval = TRUE;
              }
            else if (G_VALUE_HOLDS (&node_value, G_TYPE_STRING))
              {
                retval = flags_from_string (G_VALUE_TYPE (value),
                                            g_value_get_string (&node_value),
                                            &flags_value);
              }

            if (retval)
              g_value_set_flags (value, flags_value);
          }
          break;

        default:
          retval = FALSE;
          break;
        }

      g_value_unset (&node_value);
      break;

    case JSON_NODE_NULL:
      retval = FALSE;
      break;
    }

  return retval;
}

static JsonNode *
json_serialize_pspec (const GValue *real_value,
                      GParamSpec   *pspec)
{
  JsonNode *retval = NULL;
  GValue value = { 0, };

  switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (real_value)))
    {
    case G_TYPE_INT64:
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

    case G_TYPE_INT:
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_int (retval, g_value_get_int (real_value));
      break;

    case G_TYPE_FLOAT:
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_double (retval, g_value_get_float (real_value));
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
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_int (retval, g_value_get_uint (real_value));
      break;

    case G_TYPE_LONG:
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_int (retval, g_value_get_long (real_value));
      break;

    case G_TYPE_ULONG:
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_int (retval, g_value_get_long (real_value));
      break;

    case G_TYPE_CHAR:
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_int (retval, g_value_get_char (real_value));
      break;

    case G_TYPE_UCHAR:
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_int (retval, g_value_get_uchar (real_value));
      break;

    case G_TYPE_ENUM:
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_int (retval, g_value_get_enum (real_value));
      break;

    case G_TYPE_FLAGS:
      retval = json_node_new (JSON_NODE_VALUE);
      json_node_set_int (retval, g_value_get_flags (real_value));
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
 * json_serializable_serialize_property:
 * @serializable: a #JsonSerializable object
 * @property_name: the name of the property
 * @value: the value of the property
 * @pspec: a #GParamSpec
 *
 * Asks a #JsonSerializable implementation to serialize a #GObject
 * property into a #JsonNode object.
 *
 * Return value: a #JsonNode containing the serialized property
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

  return iface->deserialize_property (serializable,
                                      property_name,
                                      value,
                                      pspec,
                                      property_node);
}

static gboolean
json_serializable_real_deserialize (JsonSerializable *serializable,
                                    const gchar      *name,
                                    GValue           *value,
                                    GParamSpec       *pspec,
                                    JsonNode         *node)
{
  return json_deserialize_pspec (value, pspec, node);
}

static JsonNode *
json_serializable_real_serialize (JsonSerializable *serializable,
                                  const gchar      *name,
                                  const GValue     *value,
                                  GParamSpec       *pspec)
{
  return json_serialize_pspec (value, pspec);
}

static void
json_serializable_base_init (gpointer g_class,
                             gpointer data)
{
  static gboolean is_initialized = FALSE;

  if (G_UNLIKELY (!is_initialized))
    {
      JsonSerializableIface *iface = g_class;

      iface->serialize_property = json_serializable_real_serialize;
      iface->deserialize_property = json_serializable_real_deserialize;

      is_initialized = TRUE;
    }
}

GType
json_serializable_get_type (void)
{
  static GType iface_type = 0;

  if (!iface_type)
    iface_type =
      g_type_register_static_simple (G_TYPE_INTERFACE,
                                     g_intern_static_string ("JsonSerializable"),
                                     sizeof (JsonSerializableIface),
                                     json_serializable_base_init,
                                     0, NULL, 0);

  return iface_type;
}

/**
 * json_construct_gobject:
 * @gtype: the #GType of object to construct
 * @data: a JSON data stream
 * @length: length of the data stream, or -1 if it is NUL-terminated
 * @error: return location for a #GError, or %NULL
 *
 * Deserializes a JSON data stream and creates the corresponding
 * #GObject class. If @gtype implements the #JsonSerializableIface
 * interface, it will be asked to deserialize all the JSON members
 * into the respective properties; otherwise, the default implementation
 * will be used to translate the compatible JSON native types.
 *
 * Note: the JSON data stream must be an object declaration.
 *
 * Return value: a #GObject or %NULL
 *
 * Since: 0.4
 */
GObject *
json_construct_gobject (GType         gtype,
                        const gchar  *data,
                        gsize         length,
                        GError      **error)
{
  JsonParser *parser;
  JsonNode *root;
  GError *parse_error;
  GObject *retval;

  g_return_val_if_fail (gtype != G_TYPE_INVALID, NULL);
  g_return_val_if_fail (data != NULL, NULL);

  if (length < 0)
    length = strlen (data);

  parser = json_parser_new ();

  parse_error = NULL;
  json_parser_load_from_data (parser, data, length, &parse_error);
  if (parse_error)
    {
      g_propagate_error (error, parse_error);
      g_object_unref (parser);
      return NULL;
    }

  root = json_parser_get_root (parser);
  if (root == NULL || JSON_NODE_TYPE (root) != JSON_NODE_OBJECT)
    {
      g_set_error (error, JSON_PARSER_ERROR,
                   JSON_PARSER_ERROR_PARSE,
                   "Expecting a JSON object, but the root node "
                   "is of type `%s'",
                   json_node_type_name (root));
      g_object_unref (parser);
      return NULL;
    }

  retval = json_gobject_new (gtype, json_node_get_object (root));

  g_object_unref (parser);

  return retval;
}

/**
 * json_serialize_gobject:
 * @gobject: a #GObject
 * @length: (out): return value for the length of the buffer, or %NULL
 *
 * Serializes a #GObject into a JSON data stream. If @gobject implements
 * the #JsonSerializableIface interface, it will be asked to serizalize all
 * its properties; otherwise, the default implementation will be use to
 * translate the compatible types into JSON native types.
 *
 * Return value: a JSON data stream representing the passed #GObject
 */
gchar *
json_serialize_gobject (GObject *gobject,
                        gsize   *length)
{
  JsonGenerator *gen;
  JsonNode *root;
  gchar *data;

  g_return_val_if_fail (G_OBJECT (gobject), NULL);

  root = json_node_new (JSON_NODE_OBJECT);
  json_node_take_object (root, json_gobject_dump (gobject));

  gen = g_object_new (JSON_TYPE_GENERATOR,
                      "root", root,
                      "pretty", TRUE,
                      "indent", 2,
                      NULL);

  data = json_generator_to_data (gen, length);
  g_object_unref (gen);

  json_node_free (root);

  return data;
}
