/* json-generator.c - JSON streams generator
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

/**
 * SECTION:json-generator
 * @short_description: Generates JSON data streams
 *
 * #JsonGenerator provides an object for generating a JSON data stream and
 * put it into a buffer or a file.
 */

#include "config.h"

#include <stdlib.h>
#include <string.h>

#include "json-types-private.h"

#include "json-generator.h"

struct _JsonGeneratorPrivate
{
  JsonNode *root;

  guint indent;
  gunichar indent_char;

  guint pretty : 1;
};

enum
{
  PROP_0,

  PROP_PRETTY,
  PROP_INDENT,
  PROP_ROOT,
  PROP_INDENT_CHAR,

  PROP_LAST
};

static void   dump_value  (GString       *buffer,
                           gint           level,
                           JsonNode      *node);
static void   dump_array  (JsonGenerator *generator,
                           GString       *buffer,
                           gint           level,
                           JsonArray     *array);
static void   dump_object (JsonGenerator *generator,
                           GString       *buffer,
                           gint           level,
                           JsonObject    *object);

static GParamSpec *generator_props[PROP_LAST] = { NULL, };

G_DEFINE_TYPE_WITH_PRIVATE (JsonGenerator, json_generator, G_TYPE_OBJECT)

static void
json_strescape (GString     *output,
                const gchar *str)
{
  const gchar *p;
  const gchar *end;
  gsize len;

  len = strlen (str);
  end = str + len;

  for (p = str; p < end; p++)
    {
      if (*p == '\\' || *p == '"')
        {
          g_string_append_c (output, '\\');
          g_string_append_c (output, *p);
        }
      else if ((*p > 0 && *p < 0x1f) || *p == 0x7f)
        {
          switch (*p)
            {
            case '\b':
              g_string_append (output, "\\b");
              break;
            case '\f':
              g_string_append (output, "\\f");
              break;
            case '\n':
              g_string_append (output, "\\n");
              break;
            case '\r':
              g_string_append (output, "\\r");
              break;
            case '\t':
              g_string_append (output, "\\t");
              break;
            default:
              g_string_append_printf (output, "\\u00%02x", (guint)*p);
              break;
            }
        }
      else
        {
          g_string_append_c (output, *p);
        }
    }
}

static void
json_generator_finalize (GObject *gobject)
{
  JsonGeneratorPrivate *priv;

  priv = json_generator_get_instance_private ((JsonGenerator *) gobject);
  if (priv->root != NULL)
    json_node_unref (priv->root);

  G_OBJECT_CLASS (json_generator_parent_class)->finalize (gobject);
}

static void
json_generator_set_property (GObject      *gobject,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  JsonGenerator *generator = JSON_GENERATOR (gobject);

  switch (prop_id)
    {
    case PROP_PRETTY:
      json_generator_set_pretty (generator, g_value_get_boolean (value));
      break;

    case PROP_INDENT:
      json_generator_set_indent (generator, g_value_get_uint (value));
      break;

    case PROP_INDENT_CHAR:
      json_generator_set_indent_char (generator, g_value_get_uint (value));
      break;

    case PROP_ROOT:
      json_generator_set_root (generator, g_value_get_boxed (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
json_generator_get_property (GObject    *gobject,
                             guint       prop_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  JsonGeneratorPrivate *priv = JSON_GENERATOR (gobject)->priv;

  switch (prop_id)
    {
    case PROP_PRETTY:
      g_value_set_boolean (value, priv->pretty);
      break;
    case PROP_INDENT:
      g_value_set_uint (value, priv->indent);
      break;
    case PROP_INDENT_CHAR:
      g_value_set_uint (value, priv->indent_char);
      break;
    case PROP_ROOT:
      g_value_set_boxed (value, priv->root);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
json_generator_class_init (JsonGeneratorClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  /**
   * JsonGenerator:pretty:
   *
   * Whether the output should be "pretty-printed", with indentation and
   * newlines. The indentation level can be controlled by using the
   * JsonGenerator:indent property
   */
  generator_props[PROP_PRETTY] =
    g_param_spec_boolean ("pretty",
                          "Pretty",
                          "Pretty-print the output",
                          FALSE,
                          G_PARAM_READWRITE);

  /**
   * JsonGenerator:indent:
   *
   * Number of spaces to be used to indent when pretty printing.
   */
  generator_props[PROP_INDENT] =
    g_param_spec_uint ("indent",
                       "Indent",
                       "Number of indentation spaces",
                       0, G_MAXUINT,
                       2,
                       G_PARAM_READWRITE);

  /**
   * JsonGenerator:root:
   *
   * The root #JsonNode to be used when constructing a JSON data
   * stream.
   *
   * Since: 0.4
   */
  generator_props[PROP_ROOT] =
    g_param_spec_boxed ("root",
                        "Root",
                        "Root of the JSON data tree",
                        JSON_TYPE_NODE,
                        G_PARAM_READWRITE);

  /**
   * JsonGenerator:indent-char:
   *
   * The character that should be used when indenting in pretty print.
   *
   * Since: 0.6
   */
  generator_props[PROP_INDENT_CHAR] =
    g_param_spec_unichar ("indent-char",
                          "Indent Char",
                          "Character that should be used when indenting",
                          ' ',
                          G_PARAM_READWRITE);

  gobject_class->set_property = json_generator_set_property;
  gobject_class->get_property = json_generator_get_property;
  gobject_class->finalize = json_generator_finalize;
  g_object_class_install_properties (gobject_class, PROP_LAST, generator_props);
}

static void
json_generator_init (JsonGenerator *generator)
{
  JsonGeneratorPrivate *priv = json_generator_get_instance_private (generator);

  generator->priv = priv;

  priv->pretty = FALSE;
  priv->indent = 2;
  priv->indent_char = ' ';
}

static void
dump_node (JsonGenerator *generator,
           GString       *buffer,
           gint           level,
           const gchar   *name,
           JsonNode      *node)
{
  JsonGeneratorPrivate *priv = generator->priv;
  gboolean pretty = priv->pretty;
  guint indent = priv->indent;

  if (pretty)
    {
      guint i;

      for (i = 0; i < (level * indent); i++)
        g_string_append_c (buffer, priv->indent_char);
    }

  if (name)
    {
      g_string_append_c (buffer, '"');
      json_strescape (buffer, name);
      g_string_append_c (buffer, '"');

      if (pretty)
        g_string_append (buffer, " : ");
      else
        g_string_append_c (buffer, ':');
    }

  switch (JSON_NODE_TYPE (node))
    {
    case JSON_NODE_NULL:
      g_string_append (buffer, "null");
      break;

    case JSON_NODE_VALUE:
      dump_value (buffer, level, node);
      break;

    case JSON_NODE_ARRAY:
      dump_array (generator, buffer, level,
                  json_node_get_array (node));
      break;

    case JSON_NODE_OBJECT:
      dump_object (generator, buffer, level,
                   json_node_get_object (node));
      break;
    }
}

static void
dump_value (GString  *buffer,
            gint      level,
            JsonNode *node)
{
  const JsonValue *value;

  value = node->data.value;

  switch (value->type)
    {
    case JSON_VALUE_INT:
      g_string_append_printf (buffer, "%" G_GINT64_FORMAT, json_value_get_int (value));
      break;

    case JSON_VALUE_STRING:
      {
        g_string_append_c (buffer, '"');
        json_strescape (buffer, json_value_get_string (value));
        g_string_append_c (buffer, '"');
      }
      break;

    case JSON_VALUE_DOUBLE:
      {
        gchar buf[G_ASCII_DTOSTR_BUF_SIZE];

        g_string_append (buffer,
                         g_ascii_dtostr (buf, sizeof (buf),
                                         json_value_get_double (value)));
	/* ensure doubles don't become ints */
	if (g_strstr_len (buf, G_ASCII_DTOSTR_BUF_SIZE, ".") == NULL)
	  {
	    g_string_append (buffer, ".0");
          }
      }
      break;

    case JSON_VALUE_BOOLEAN:
      g_string_append (buffer, json_value_get_boolean (value) ? "true" : "false");
      break;

    case JSON_VALUE_NULL:
      g_string_append (buffer, "null");
      break;

    default:
      break;
    }
}

static void
dump_array (JsonGenerator *generator,
            GString       *buffer,
            gint           level,
            JsonArray     *array)
{
  JsonGeneratorPrivate *priv = generator->priv;
  guint array_len = json_array_get_length (array);
  guint i;
  gboolean pretty = priv->pretty;
  guint indent = priv->indent;

  g_string_append_c (buffer, '[');

  if (pretty)
    g_string_append_c (buffer, '\n');

  for (i = 0; i < array_len; i++)
    {
      JsonNode *cur = json_array_get_element (array, i);

      dump_node (generator, buffer, level + 1, NULL, cur);

      if ((i + 1) != array_len)
        g_string_append_c (buffer, ',');

      if (pretty)
        g_string_append_c (buffer, '\n');
    }

  if (pretty)
    {
      for (i = 0; i < (level * indent); i++)
        g_string_append_c (buffer, priv->indent_char);
    }

  g_string_append_c (buffer, ']');
}

static void
dump_object (JsonGenerator *generator,
             GString       *buffer,
             gint           level,
             JsonObject    *object)
{
  JsonGeneratorPrivate *priv = generator->priv;
  GList *members, *l;
  gboolean pretty = priv->pretty;
  guint indent = priv->indent;
  guint i;

  g_string_append_c (buffer, '{');

  if (pretty)
    g_string_append_c (buffer, '\n');

  members = json_object_get_members (object);

  for (l = members; l != NULL; l = l->next)
    {
      const gchar *member_name = l->data;
      JsonNode *cur = json_object_get_member (object, member_name);

      dump_node (generator, buffer, level + 1, member_name, cur);

      if (l->next != NULL)
        g_string_append_c (buffer, ',');

      if (pretty)
        g_string_append_c (buffer, '\n');
    }

  g_list_free (members);

  if (pretty)
    {
      for (i = 0; i < (level * indent); i++)
        g_string_append_c (buffer, priv->indent_char);
    }

  g_string_append_c (buffer, '}');
}

/**
 * json_generator_new:
 * 
 * Creates a new #JsonGenerator. You can use this object to generate a
 * JSON data stream starting from a data object model composed by
 * #JsonNodes.
 *
 * Return value: the newly created #JsonGenerator instance
 */
JsonGenerator *
json_generator_new (void)
{
  return g_object_new (JSON_TYPE_GENERATOR, NULL);
}

/**
 * json_generator_to_data:
 * @generator: a #JsonGenerator
 * @length: (out): return location for the length of the returned
 *   buffer, or %NULL
 *
 * Generates a JSON data stream from @generator and returns it as a
 * buffer.
 *
 * Return value: a newly allocated buffer holding a JSON data stream.
 *   Use g_free() to free the allocated resources.
 */
gchar *
json_generator_to_data (JsonGenerator *generator,
                        gsize         *length)
{
  JsonNode *root;
  GString *string;

  g_return_val_if_fail (JSON_IS_GENERATOR (generator), NULL);

  root = generator->priv->root;
  if (!root)
    {
      if (length)
        *length = 0;

      return NULL;
    }

  string = g_string_new ("");
  dump_node (generator, string, 0, NULL, root);

  if (length)
    *length = string->len;

  return g_string_free (string, FALSE);
}

/**
 * json_generator_to_file:
 * @generator: a #JsonGenerator
 * @filename: path to the target file
 * @error: return location for a #GError, or %NULL
 *
 * Creates a JSON data stream and puts it inside @filename, overwriting the
 * current file contents. This operation is atomic.
 *
 * Return value: %TRUE if saving was successful.
 */
gboolean
json_generator_to_file (JsonGenerator  *generator,
                        const gchar    *filename,
                        GError        **error)
{
  gchar *buffer;
  gsize len;
  gboolean retval;

  g_return_val_if_fail (JSON_IS_GENERATOR (generator), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);

  buffer = json_generator_to_data (generator, &len);
  retval = g_file_set_contents (filename, buffer, len, error);
  g_free (buffer);

  return retval;
}

/**
 * json_generator_to_stream:
 * @generator: a #JsonGenerator
 * @stream: a #GOutputStream
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @error: return location for a #GError, or %NULL
 *
 * Outputs JSON data and streams it (synchronously) to @stream.
 *
 * Return value: %TRUE if the write operation was successful, and %FALSE
 *   on failure. In case of error, the #GError will be filled accordingly
 *
 * Since: 0.12
 */
gboolean
json_generator_to_stream (JsonGenerator  *generator,
                          GOutputStream  *stream,
                          GCancellable   *cancellable,
                          GError        **error)
{
  gboolean retval;
  gchar *buffer;
  gsize len;

  g_return_val_if_fail (JSON_IS_GENERATOR (generator), FALSE);
  g_return_val_if_fail (G_IS_OUTPUT_STREAM (stream), FALSE);

  if (g_cancellable_set_error_if_cancelled (cancellable, error))
    return FALSE;

  buffer = json_generator_to_data (generator, &len);
  retval = g_output_stream_write (stream, buffer, len, cancellable, error);
  g_free (buffer);

  return retval;
}

/**
 * json_generator_set_root:
 * @generator: a #JsonGenerator
 * @node: a #JsonNode
 *
 * Sets @node as the root of the JSON data stream to be serialized by
 * the #JsonGenerator.
 *
 * The passed @node is copied by the generator object, so it can be
 * safely freed after calling this function.
 */
void
json_generator_set_root (JsonGenerator *generator,
                         JsonNode      *node)
{
  g_return_if_fail (JSON_IS_GENERATOR (generator));

  if (generator->priv->root == node)
    return;

  if (generator->priv->root != NULL)
    {
      json_node_unref (generator->priv->root);
      generator->priv->root = NULL;
    }

  if (node != NULL)
    generator->priv->root = json_node_copy (node);

  g_object_notify_by_pspec (G_OBJECT (generator), generator_props[PROP_ROOT]);
}

/**
 * json_generator_get_root:
 * @generator: a #JsonGenerator
 *
 * Retrieves a pointer to the root #JsonNode set using
 * json_generator_set_root().
 *
 * Return value: (transfer none): a #JsonNode, or %NULL. The returned node
 *   is owned by the #JsonGenerator and it should not be freed
 *
 * Since: 0.14
 */
JsonNode *
json_generator_get_root (JsonGenerator *generator)
{
  g_return_val_if_fail (JSON_IS_GENERATOR (generator), NULL);

  return generator->priv->root;
}

/**
 * json_generator_set_pretty:
 * @generator: a #JsonGenerator
 * @is_pretty: whether the generated string should be pretty printed
 *
 * Sets whether the generated JSON should be pretty printed, using the
 * indentation character specified in the #JsonGenerator:indent-char
 * property and the spacing specified in #JsonGenerator:indent property.
 *
 * Since: 0.14
 */
void
json_generator_set_pretty (JsonGenerator *generator,
                           gboolean       is_pretty)
{
  JsonGeneratorPrivate *priv;

  g_return_if_fail (JSON_IS_GENERATOR (generator));

  priv = generator->priv;

  is_pretty = !!is_pretty;

  if (priv->pretty != is_pretty)
    {
      priv->pretty = is_pretty;

      g_object_notify_by_pspec (G_OBJECT (generator), generator_props[PROP_PRETTY]);
    }
}

/**
 * json_generator_get_pretty:
 * @generator: a #JsonGenerator
 *
 * Retrieves the value set using json_generator_set_pretty().
 *
 * Return value: %TRUE if the generated JSON should be pretty-printed, and
 *   %FALSE otherwise
 *
 * Since: 0.14
 */
gboolean
json_generator_get_pretty (JsonGenerator *generator)
{
  g_return_val_if_fail (JSON_IS_GENERATOR (generator), FALSE);

  return generator->priv->pretty;
}

/**
 * json_generator_set_indent:
 * @generator: a #JsonGenerator
 * @indent_level: the number of repetitions of the indentation character
 *   that should be applied when pretty printing
 *
 * Sets the number of repetitions for each indentation level.
 *
 * Since: 0.14
 */
void
json_generator_set_indent (JsonGenerator *generator,
                           guint          indent_level)
{
  JsonGeneratorPrivate *priv;

  g_return_if_fail (JSON_IS_GENERATOR (generator));

  priv = generator->priv;

  if (priv->indent != indent_level)
    {
      priv->indent = indent_level;

      g_object_notify_by_pspec (G_OBJECT (generator), generator_props[PROP_INDENT]);
    }
}

/**
 * json_generator_get_indent:
 * @generator: a #JsonGenerator
 *
 * Retrieves the value set using json_generator_set_indent().
 *
 * Return value: the number of repetitions per indentation level
 *
 * Since: 0.14
 */
guint
json_generator_get_indent (JsonGenerator *generator)
{
  g_return_val_if_fail (JSON_IS_GENERATOR (generator), FALSE);

  return generator->priv->indent;
}

/**
 * json_generator_set_indent_char:
 * @generator: a #JsonGenerator
 * @indent_char: a Unicode character to be used when indenting
 *
 * Sets the character to be used when indenting
 *
 * Since: 0.14
 */
void
json_generator_set_indent_char (JsonGenerator *generator,
                                gunichar       indent_char)
{
  JsonGeneratorPrivate *priv;

  g_return_if_fail (JSON_IS_GENERATOR (generator));

  priv = generator->priv;

  if (priv->indent_char != indent_char)
    {
      priv->indent_char = indent_char;

      g_object_notify_by_pspec (G_OBJECT (generator), generator_props[PROP_INDENT_CHAR]);
    }
}

/**
 * json_generator_get_indent_char:
 * @generator: a #JsonGenerator
 *
 * Retrieves the value set using json_generator_set_indent_char().
 *
 * Return value: the character to be used when indenting
 *
 * Since: 0.14
 */
gunichar
json_generator_get_indent_char (JsonGenerator *generator)
{
  g_return_val_if_fail (JSON_IS_GENERATOR (generator), FALSE);

  return generator->priv->indent_char;
}
