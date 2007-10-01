#include "config.h"

#include <string.h>

#include "json-marshal.h"
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
  JsonNode *root;
  JsonNode *current_node;
};

static const GScannerConfig json_scanner_config =
{
  ( " \t\r\n" )		/* cset_skip_characters */,
  (
   "_"
   G_CSET_a_2_z
   G_CSET_A_2_Z
  )			/* cset_identifier_first */,
  (
   G_CSET_DIGITS
   "-_"
   G_CSET_a_2_z
   G_CSET_A_2_Z
  )			/* cset_identifier_nth */,
  ( "#\n" )		/* cpair_comment_single */,
  TRUE			/* case_sensitive */,
  TRUE			/* skip_comment_multi */,
  TRUE			/* skip_comment_single */,
  FALSE			/* scan_comment_multi */,
  TRUE			/* scan_identifier */,
  TRUE			/* scan_identifier_1char */,
  FALSE			/* scan_identifier_NULL */,
  TRUE			/* scan_symbols */,
  TRUE			/* scan_binary */,
  TRUE			/* scan_octal */,
  TRUE			/* scan_float */,
  TRUE			/* scan_hex */,
  TRUE			/* scan_hex_dollar */,
  TRUE			/* scan_string_sq */,
  TRUE			/* scan_string_dq */,
  TRUE			/* numbers_2_int */,
  FALSE			/* int_2_float */,
  FALSE			/* identifier_2_string */,
  TRUE			/* char_2_token */,
  TRUE			/* symbol_2_token */,
  FALSE			/* scope_0_fallback */,
};


static const gchar symbol_names[] =
  "true\0"
  "false\0"
  "null\0";

static const struct
{
  guint name_offset;
  guint token;
} symbols[] = {
  {  0, JSON_TOKEN_TRUE },
  {  5, JSON_TOKEN_FALSE },
  { 11, JSON_TOKEN_NULL }
};

static const guint n_symbols = G_N_ELEMENTS (symbols);

enum
{
  ERROR,

  LAST_SIGNAL
};

static guint parser_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (JsonParser, json_parser, G_TYPE_OBJECT);

static guint json_parse_array  (JsonParser *parser,
                                GScanner   *scanner,
                                gboolean    nested);
static guint json_parse_object (JsonParser *parser,
                                GScanner   *scanner);

static void
json_parser_dispose (GObject *gobject)
{
  JsonParserPrivate *priv = JSON_PARSER_GET_PRIVATE (gobject);

  if (priv->root)
    {
      json_node_free (priv->root);
      priv->root = NULL;
    }

  G_OBJECT_CLASS (json_parser_parent_class)->dispose (gobject);
}

static void
json_parser_class_init (JsonParserClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (JsonParserPrivate));

  gobject_class->dispose = json_parser_dispose;

  /**
   * JsonParser::error:
   * @parser: the parser instance that received the signal
   * @error: a pointer to the #GError
   *
   * The ::error signal is emitted each time a #JsonParser encounters
   * an error in a JSON stream.
   */
  parser_signals[ERROR] =
    g_signal_new ("error",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, error),
                  NULL, NULL,
                  _json_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1,
                  G_TYPE_POINTER);
}

static void
json_parser_init (JsonParser *parser)
{
  JsonParserPrivate *priv;

  parser->priv = priv = JSON_PARSER_GET_PRIVATE (parser);

  priv->root = NULL;
  priv->current_node = NULL;
}

static guint
json_parse_array (JsonParser *parser,
                  GScanner   *scanner,
                  gboolean    nested)
{
  JsonParserPrivate *priv = parser->priv;
  JsonArray *array;
  guint token;

  if (!nested)
    {
      /* the caller already swallowed the opening '[' */
      token = g_scanner_get_next_token (scanner);
      if (token != G_TOKEN_LEFT_BRACE)
        return G_TOKEN_LEFT_BRACE;
    }

  array = json_array_new ();

  token = g_scanner_get_next_token (scanner);
  while (token != G_TOKEN_RIGHT_BRACE)
    {
      JsonNode *node = NULL;
      GValue value = { 0, };
      
      /* nested array */
      if (token == G_TOKEN_LEFT_BRACE)
        {
          JsonNode *old_node = priv->current_node;

          priv->current_node = json_node_new (JSON_NODE_ARRAY);

          token = json_parse_array (parser, scanner, TRUE);

          node = priv->current_node;
          priv->current_node = old_node;

          if (token != G_TOKEN_NONE)
            return token;

          json_array_add_element (array, node);
          node->parent = priv->current_node;

          token = g_scanner_get_next_token (scanner);
          if (token == G_TOKEN_RIGHT_BRACE)
            break;

          continue;
        }

      switch (token)
        {
        case G_TOKEN_COMMA:
          /* eat the comma and continue */
          break;
        
        case G_TOKEN_INT:
          g_value_init (&value, G_TYPE_INT);
          g_value_set_int (&value, scanner->value.v_int);
          
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_value (node, &value);

          g_value_unset (&value);
          break;

        case G_TOKEN_FLOAT:
          g_value_init (&value, G_TYPE_FLOAT);
          g_value_set_float (&value, scanner->value.v_float);
          
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_value (node, &value);

          g_value_unset (&value);
          break;

        case G_TOKEN_STRING:
          g_value_init (&value, G_TYPE_STRING);
          g_value_set_string (&value, scanner->value.v_string);

          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_value (node, &value);

          g_value_unset (&value);
          break;

        case JSON_TOKEN_TRUE:
        case JSON_TOKEN_FALSE:
          g_value_init (&value, G_TYPE_BOOLEAN);
          g_value_set_boolean (&value, token == JSON_TOKEN_TRUE ? TRUE : FALSE);

          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_value (node, &value);

          g_value_unset (&value);
          break;

        case JSON_TOKEN_NULL:
          node = json_node_new (JSON_NODE_NULL);
          break;

        default:
          return G_TOKEN_RIGHT_BRACE;
        }

      if (node)
        {
          json_array_add_element (array, node);
          node->parent = priv->current_node;
        }

      token = g_scanner_get_next_token (scanner);
    }

  json_node_set_array (priv->current_node, array);

  return G_TOKEN_NONE;
}

static guint
json_parse_object (JsonParser *parser,
                   GScanner   *scanner)
{
  guint token;
  guint result = G_TOKEN_NONE;

  token = g_scanner_get_next_token (scanner);
  if (token != G_TOKEN_LEFT_CURLY)
    return G_TOKEN_LEFT_CURLY;

  token = g_scanner_get_next_token (scanner);
  if (token == G_TOKEN_RIGHT_CURLY)
    return G_TOKEN_NONE;
  else
    {
    }
  
  return result;
}

static guint
json_parse_statement (JsonParser *parser,
                      GScanner   *scanner)
{
  JsonParserPrivate *priv = parser->priv;
  guint token;

  token = g_scanner_peek_next_token (scanner);
  switch (token)
    {
    case G_TOKEN_LEFT_CURLY:
      priv->root = priv->current_node = json_node_new (JSON_NODE_OBJECT);
      return json_parse_object (parser, scanner);

    case G_TOKEN_LEFT_BRACE:
      priv->root = priv->current_node = json_node_new (JSON_NODE_ARRAY);
      return json_parse_array (parser, scanner, FALSE);

    default:
      g_scanner_get_next_token (scanner);
      return G_TOKEN_SYMBOL;
    }
}

static void
json_scanner_msg_handler (GScanner *scanner,
                          gchar    *message,
                          gboolean  error)
{
  JsonParser *parser = scanner->user_data;

  if (error)
    {
      GError *error = NULL;

      g_warning ("Line %d: %s", scanner->line, message);

      g_set_error (&error, JSON_PARSER_ERROR,
                   JSON_PARSER_ERROR_PARSE,
                   "Parse error on line %d: %s",
                   scanner->line,
                   message);
      
      g_signal_emit (parser, parser_signals[ERROR], 0, error);

      g_error_free (error);
    }
  else
    {
      g_warning ("Line %d: %s", scanner->line, message);
    }
}

static GScanner *
json_scanner_new (JsonParser *parser)
{
  GScanner *scanner;

  scanner = g_scanner_new (&json_scanner_config);
  scanner->msg_handler = json_scanner_msg_handler;
  scanner->user_data = parser;

  return scanner;
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
  GScanner *scanner;
  gboolean done;
  gboolean retval = TRUE;
  gint i;

  g_return_val_if_fail (JSON_IS_PARSER (parser), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  if (length < 0)
    length = strlen (data);

  if (parser->priv->root)
    {
      json_node_free (parser->priv->root);
      parser->priv->root = NULL;
    }

  scanner = json_scanner_new (parser);
  g_scanner_input_text (scanner, data, strlen (data));

  for (i = 0; i < n_symbols; i++)
    {
      g_scanner_scope_add_symbol (scanner, 0,
                                  symbol_names + symbols[i].name_offset,
                                  GINT_TO_POINTER (symbols[i].token));
    }

  done = FALSE;
  while (!done)
    {
      if (g_scanner_peek_next_token (scanner) == G_TOKEN_EOF)
        done = TRUE;
      else
        {
          guint expected_token;

          expected_token = json_parse_statement (parser, scanner);
          if (expected_token != G_TOKEN_NONE)
            {
              const gchar *symbol_name;
              gchar *msg;

              msg = NULL;
              symbol_name = NULL;
              if (scanner->scope_id == 0)
                {
                  if (expected_token > JSON_TOKEN_INVALID &&
                      expected_token < JSON_TOKEN_LAST)
                    {
                      for (i = 0; i < n_symbols; i++)
                        if (symbols[i].token == expected_token)
                          msg = (gchar *) symbol_names + symbols[i].name_offset;

                      if (msg)
                        msg = g_strconcat ("e.g. `", msg, "'", NULL);
                    }

                  if (scanner->token > JSON_TOKEN_INVALID &&
                      scanner->token < JSON_TOKEN_LAST)
                    {
                      symbol_name = "???";

                      for (i = 0; i < n_symbols; i++)
                        if (symbols[i].token == scanner->token)
                          symbol_name = symbol_names + symbols[i].name_offset;
                    }
                }

              /* this will emit the ::error signal via the custom
               * message handler we install
               */
              g_scanner_unexp_token (scanner, expected_token,
                                     NULL, "keyword",
                                     symbol_name, msg,
                                     TRUE);
              
              /* we set a generic error here; the message from
               * GScanner is relayed in the ::error signal
               */
              g_set_error (error, JSON_PARSER_ERROR,
                           JSON_PARSER_ERROR_PARSE,
                           "Invalid token `%s' found: expecting %s",
                           symbol_name ? symbol_name : "???",
                           msg ? msg : "unknown");

              retval = FALSE;

              g_free (msg);
              done = TRUE;
            }
        }
    }

  g_scanner_destroy (scanner);

  parser->priv->current_node = NULL;

  return retval;
}

/**
 * json_parser_get_root:
 * @parser: a #JsonParser
 *
 * Retrieves the top level node from the parsed JSON stream.
 *
 * Return value: the root #JsonNode . The returned node is owned by
 *   the #JsonParser and should never be modified or freed.
 */
JsonNode *
json_parser_get_root (JsonParser *parser)
{
  g_return_val_if_fail (JSON_IS_PARSER (parser), NULL);

  return parser->priv->root;
}

