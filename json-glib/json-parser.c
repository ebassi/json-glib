#include "config.h"

#include <string.h>

#include "json-parser.h"
#include "json-private.h"

GQuark
json_parser_error_quark (void)
{
  return g_quark_from_static_string ("json-parser-error");
}

#define JSON_PARSER_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), JSON_TYPE_PARSER, JsonParserPrivate))

struct _JsonParserPrivate
{
  GList *top_levels;
};

G_DEFINE_TYPE (JsonParser, json_parser, G_TYPE_OBJECT);

static void
json_parser_dispose (GObject *gobject)
{
  G_OBJECT_CLASS (json_parser_parent_class)->dispose (gobject);
}

static void
json_parser_class_init (JsonParserClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (JsonParserPrivate));

  gobject_class->dispose = json_parser_dispose;
}

static void
json_parser_init (JsonParser *parser)
{
  JsonParserPrivate *priv;

  parser->priv = priv = JSON_PARSER_GET_PRIVATE (parser);

  priv->top_levels = NULL;
}

/**
 * json_parser_new:
 * 
 * Creates a new #JsonParser instance. You can use the #JsonParser to
 * load a JSON stream from either a file or a buffer and then walk the
 * hierarchy using the data types API.
 *
 * Return value: the newly created #JsonParser. Use g_object_unref()
 *   to release all the memory it allocates.
 */
JsonParser *
json_parser_new (void)
{
  return g_object_new (JSON_TYPE_PARSER, NULL);
}

/**
 * json_parser_load_from_file:
 * @parser: a #JsonParser
 * @filename: the path for the file to parse
 * @error: return location for a #GError, or %NULL
 *
 * Loads a JSON stream from the content of @filename and parses it.
 *
 * Return value: %TRUE if the file was successfully loaded and parsed.
 *   In case of error, @error is set accordingly and %FALSE is returned
 */
gboolean
json_parser_load_from_file (JsonParser   *parser,
                            const gchar  *filename,
                            GError      **error)
{
  GError *internal_error;
  gchar *data;
  gsize length;
  gboolean retval = TRUE;

  g_return_val_if_fail (JSON_IS_PARSER (parser), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);

  internal_error = NULL;
  if (!g_file_get_contents (filename, &data, &length, &internal_error))
    {
      g_propagate_error (error, internal_error);
      return FALSE;
    }

  if (!json_parser_load_from_data (parser, data, length, &internal_error))
    {
      g_propagate_error (error, internal_error);
      retval = FALSE;
    }
  
  g_free (data);

  return retval;
}

/**
 * json_parser_load_from_data:
 * @parser: a #JsonParser
 * @data: the buffer to parse
 * @length: the length of the buffer, or -1
 * @error: return location for a #GError, or %NULL
 *
 * Loads a JSON stream from a buffer and parses it.
 *
 * Return value: %TRUE if the buffer was succesfully parser. In case
 *   of error, @error is set accordingly and %FALSE is returned
 */
gboolean
json_parser_load_from_data (JsonParser   *parser,
                            const gchar  *data,
                            gsize         length,
                            GError      **error)
{
  g_return_val_if_fail (JSON_IS_PARSER (parser), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  if (length < 0)
    length = strlen (data);

  return TRUE;
}

/**
 * json_parser_get_toplevels:
 * @parser: a #JsonParser
 *
 * Retrieves the top level entities from the parsed JSON stream.
 *
 * Return value: a list of pointers to the top-level entites. The
 *   returned list is owned by the #JsonParser and should never be
 *   modified or freed.
 */
GList *
json_parser_get_toplevels (JsonParser *parser)
{
  g_return_val_if_fail (JSON_IS_PARSER (parser), NULL);

  return parser->priv->top_levels;
}

