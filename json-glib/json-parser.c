/* json-parser.c - JSON streams parser
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
 * SECTION:json-parser
 * @short_description: Parse JSON data streams
 *
 * #JsonParser provides an object for parsing a JSON data stream, either
 * inside a file or inside a static buffer.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "json-types-private.h"

#include "json-marshal.h"
#include "json-parser.h"
#include "json-scanner.h"

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

  JsonScanner *scanner;

  GError *last_error;

  guint has_assignment : 1;
  guint is_filename    : 1;

  gchar *variable_name;
  gchar *filename;
};

static const gchar symbol_names[] =
  "true\0"
  "false\0"
  "null\0"
  "var\0";

static const struct
{
  guint name_offset;
  guint token;
} symbols[] = {
  {  0, JSON_TOKEN_TRUE  },
  {  5, JSON_TOKEN_FALSE },
  { 11, JSON_TOKEN_NULL  },
  { 16, JSON_TOKEN_VAR   }
};

static const guint n_symbols = G_N_ELEMENTS (symbols);

enum
{
  PARSE_START,
  OBJECT_START,
  OBJECT_MEMBER,
  OBJECT_END,
  ARRAY_START,
  ARRAY_ELEMENT,
  ARRAY_END,
  PARSE_END,
  ERROR,

  LAST_SIGNAL
};

static guint parser_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (JsonParser, json_parser, G_TYPE_OBJECT);

static guint json_parse_array  (JsonParser *parser,
                                JsonScanner   *scanner,
                                gboolean    nested);
static guint json_parse_object (JsonParser *parser,
                                JsonScanner   *scanner,
                                gboolean    nested);

static inline void
json_parser_clear (JsonParser *parser)
{
  JsonParserPrivate *priv = parser->priv;

  g_free (priv->variable_name);
  priv->variable_name = NULL;

  if (priv->last_error)
    {
      g_error_free (priv->last_error);
      priv->last_error = NULL;
    }

  if (priv->root)
    {
      json_node_free (priv->root);
      priv->root = NULL;
    }
}

static void
json_parser_dispose (GObject *gobject)
{
  json_parser_clear (JSON_PARSER (gobject));

  G_OBJECT_CLASS (json_parser_parent_class)->dispose (gobject);
}

static void
json_parser_finalize (GObject *gobject)
{
  JsonParserPrivate *priv = JSON_PARSER (gobject)->priv;

  g_free (priv->variable_name);
  g_free (priv->filename);

  G_OBJECT_CLASS (json_parser_parent_class)->finalize (gobject);
}

static void
json_parser_class_init (JsonParserClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (JsonParserPrivate));

  gobject_class->dispose = json_parser_dispose;
  gobject_class->finalize = json_parser_finalize;

  /**
   * JsonParser::parse-start:
   * @parser: the #JsonParser that received the signal
   * 
   * The ::parse-start signal is emitted when the parser began parsing
   * a JSON data stream.
   */
  parser_signals[PARSE_START] =
    g_signal_new ("parse-start",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, parse_start),
                  NULL, NULL,
                  _json_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  /**
   * JsonParser::parse-end:
   * @parser: the #JsonParser that received the signal
   *
   * The ::parse-end signal is emitted when the parser successfully
   * finished parsing a JSON data stream
   */
  parser_signals[PARSE_END] =
    g_signal_new ("parse-end",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, parse_end),
                  NULL, NULL,
                  _json_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  /**
   * JsonParser::object-start:
   * @parser: the #JsonParser that received the signal
   * 
   * The ::object-start signal is emitted each time the #JsonParser
   * starts parsing a #JsonObject.
   */
  parser_signals[OBJECT_START] =
    g_signal_new ("object-start",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, object_start),
                  NULL, NULL,
                  _json_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  /**
   * JsonParser::object-member:
   * @parser: the #JsonParser that received the signal
   * @object: a #JsonObject
   * @member_name: the name of the newly parsed member
   *
   * The ::object-member signal is emitted each time the #JsonParser
   * has successfully parsed a single member of a #JsonObject. The
   * object and member are passed to the signal handlers.
   */
  parser_signals[OBJECT_MEMBER] =
    g_signal_new ("object-member",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, object_member),
                  NULL, NULL,
                  _json_marshal_VOID__BOXED_STRING,
                  G_TYPE_NONE, 2,
                  JSON_TYPE_OBJECT,
                  G_TYPE_STRING);
  /**
   * JsonParser::object-end:
   * @parser: the #JsonParser that received the signal
   * @object: the parsed #JsonObject
   *
   * The ::object-end signal is emitted each time the #JsonParser
   * has successfully parsed an entire #JsonObject.
   */
  parser_signals[OBJECT_END] =
    g_signal_new ("object-end",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, object_end),
                  NULL, NULL,
                  _json_marshal_VOID__BOXED,
                  G_TYPE_NONE, 1,
                  JSON_TYPE_OBJECT);
  /**
   * JsonParser::array-start:
   * @parser: the #JsonParser that received the signal
   *
   * The ::array-start signal is emitted each time the #JsonParser
   * starts parsing a #JsonArray
   */
  parser_signals[ARRAY_START] =
    g_signal_new ("array-start",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, array_start),
                  NULL, NULL,
                  _json_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
  /**
   * JsonParser::array-element:
   * @parser: the #JsonParser that received the signal
   * @array: a #JsonArray
   * @index_: the index of the newly parsed element
   *
   * The ::array-element signal is emitted each time the #JsonParser
   * has successfully parsed a single element of a #JsonArray. The
   * array and element index are passed to the signal handlers.
   */
  parser_signals[ARRAY_ELEMENT] =
    g_signal_new ("array-element",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, array_element),
                  NULL, NULL,
                  _json_marshal_VOID__BOXED_INT,
                  G_TYPE_NONE, 2,
                  JSON_TYPE_ARRAY,
                  G_TYPE_INT);
  /**
   * JsonParser::array-end:
   * @parser: the #JsonParser that received the signal
   * @array: the parsed #JsonArrary
   *
   * The ::array-end signal is emitted each time the #JsonParser
   * has successfully parsed an entire #JsonArray
   */
  parser_signals[ARRAY_END] =
    g_signal_new ("array-end",
                  G_OBJECT_CLASS_TYPE (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (JsonParserClass, array_end),
                  NULL, NULL,
                  _json_marshal_VOID__BOXED,
                  G_TYPE_NONE, 1,
                  JSON_TYPE_ARRAY);
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

  priv->has_assignment = FALSE;
  priv->variable_name = NULL;

  priv->is_filename = FALSE;
  priv->filename = FALSE;
}

static guint
json_parse_array (JsonParser *parser,
                  JsonScanner   *scanner,
                  gboolean    nested)
{
  JsonParserPrivate *priv = parser->priv;
  JsonArray *array;
  guint token;

  if (!nested)
    {
      /* the caller already swallowed the opening '[' */
      token = json_scanner_get_next_token (scanner);
      if (token != G_TOKEN_LEFT_BRACE)
        return G_TOKEN_LEFT_BRACE;
    }

  g_signal_emit (parser, parser_signals[ARRAY_START], 0);

  array = json_array_new ();

  token = json_scanner_get_next_token (scanner);
  while (token != G_TOKEN_RIGHT_BRACE)
    {
      JsonNode *node = NULL;
      gboolean negative = FALSE;

      if (token == G_TOKEN_COMMA)
        {
          /* swallow the comma */
          token = json_scanner_get_next_token (scanner);

          continue;
        }

      /* nested object */
      if (token == G_TOKEN_LEFT_CURLY)
        {
          JsonNode *old_node = priv->current_node;

          priv->current_node = json_node_new (JSON_NODE_OBJECT);

          token = json_parse_object (parser, scanner, TRUE);

          node = priv->current_node;
          priv->current_node = old_node;

          if (token != G_TOKEN_NONE)
            {
              json_node_free (node);
              json_array_unref (array);

              return token;
            }

          json_array_add_element (array, node);
          node->parent = priv->current_node;

          g_signal_emit (parser, parser_signals[ARRAY_ELEMENT], 0,
                         array,
                         json_array_get_length (array));

          token = json_scanner_get_next_token (scanner);
          if (token == G_TOKEN_RIGHT_BRACE)
            break;

          if (token != G_TOKEN_COMMA)
            {
              json_array_unref (array);

              return G_TOKEN_RIGHT_BRACE;
            }

          continue;
        }

      /* nested array */
      if (token == G_TOKEN_LEFT_BRACE)
        {
          JsonNode *old_node = priv->current_node;

          priv->current_node = json_node_new (JSON_NODE_ARRAY);

          token = json_parse_array (parser, scanner, TRUE);

          node = priv->current_node;
          priv->current_node = old_node;

          if (token != G_TOKEN_NONE)
            {
              json_node_free (node);
              json_array_unref (array);

              return token;
            }

          json_array_add_element (array, node);
          node->parent = priv->current_node;

          g_signal_emit (parser, parser_signals[ARRAY_ELEMENT], 0,
                         array,
                         json_array_get_length (array));

          token = json_scanner_get_next_token (scanner);
          if (token == G_TOKEN_RIGHT_BRACE)
            break;

          if (token != G_TOKEN_COMMA)
            {
              json_array_unref (array);

              return G_TOKEN_RIGHT_BRACE;
            }

          continue;
        }

      if (token == '-')
        {
          guint next_token = json_scanner_peek_next_token (scanner);

          if (next_token == G_TOKEN_INT ||
              next_token == G_TOKEN_FLOAT)
            {
              negative = TRUE;
              token = json_scanner_get_next_token (scanner);
            }
          else
            {
              json_array_unref (array);

              return G_TOKEN_INT;
            }
        }

      switch (token)
        {
        case G_TOKEN_INT:
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_int (node, negative ? scanner->value.v_int64 * -1
                                            : scanner->value.v_int64);
          break;

        case G_TOKEN_FLOAT:
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_double (node, negative ? scanner->value.v_float * -1.0
                                               : scanner->value.v_float);
          break;

        case G_TOKEN_STRING:
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_string (node, scanner->value.v_string);
          break;

        case JSON_TOKEN_TRUE:
        case JSON_TOKEN_FALSE:
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_boolean (node, token == JSON_TOKEN_TRUE ? TRUE : FALSE);
          break;

        case JSON_TOKEN_NULL:
          node = json_node_new (JSON_NODE_NULL);
          break;

        default:
          json_array_unref (array);
          return G_TOKEN_RIGHT_BRACE;
        }

      if (node)
        {
          json_array_add_element (array, node);
          node->parent = priv->current_node;

          g_signal_emit (parser, parser_signals[ARRAY_ELEMENT], 0,
                         array,
                         json_array_get_length (array));
        }

      token = json_scanner_get_next_token (scanner);
      if (token != G_TOKEN_COMMA && token != G_TOKEN_RIGHT_BRACE)
        {
          json_array_unref (array);

          return G_TOKEN_RIGHT_BRACE;
        }
    }

  json_node_take_array (priv->current_node, array);

  g_signal_emit (parser, parser_signals[ARRAY_END], 0, array);

  return G_TOKEN_NONE;
}

static guint
json_parse_object (JsonParser *parser,
                   JsonScanner   *scanner,
                   gboolean    nested)
{
  JsonParserPrivate *priv = parser->priv;
  JsonObject *object;
  guint token;

  if (!nested)
    {
      /* the caller already swallowed the opening '{' */
      token = json_scanner_get_next_token (scanner);
      if (token != G_TOKEN_LEFT_CURLY)
        return G_TOKEN_LEFT_CURLY;
    }

  g_signal_emit (parser, parser_signals[OBJECT_START], 0);

  object = json_object_new ();

  token = json_scanner_get_next_token (scanner);
  while (token != G_TOKEN_RIGHT_CURLY)
    {
      JsonNode *node = NULL;
      gchar *name = NULL;
      gboolean negative = FALSE;

      if (token == G_TOKEN_COMMA)
        {
          /* swallow the comma */
          token = json_scanner_get_next_token (scanner);

          continue;
        }

      if (token == G_TOKEN_STRING)
        {
          name = g_strdup (scanner->value.v_string);

          token = json_scanner_get_next_token (scanner);
          if (token != ':')
            {
              g_free (name);
              json_object_unref (object);

              return ':';
            }
          else
            {
              /* swallow the colon */
              token = json_scanner_get_next_token (scanner);
            }
        }

      if (!name)
        {
          json_object_unref (object);

          return G_TOKEN_STRING;
        }

      if (token == G_TOKEN_LEFT_CURLY)
        {
          JsonNode *old_node = priv->current_node;
      
          priv->current_node = json_node_new (JSON_NODE_OBJECT);

          token = json_parse_object (parser, scanner, TRUE);

          node = priv->current_node;
          priv->current_node = old_node;

          if (token != G_TOKEN_NONE)
            {
              g_free (name);
              
              if (node)
                json_node_free (node);

              json_object_unref (object);

              return token;
            }

          json_object_set_member (object, name, node);
          node->parent = priv->current_node;

          g_signal_emit (parser, parser_signals[OBJECT_MEMBER], 0,
                         object,
                         name);

          g_free (name);

          token = json_scanner_get_next_token (scanner);
          if (token == G_TOKEN_RIGHT_CURLY)
            break;

          if (token != G_TOKEN_COMMA)
            {
              json_object_unref (object);

              return G_TOKEN_RIGHT_CURLY;
            }

          continue;
        }
     
      if (token == G_TOKEN_LEFT_BRACE)
        {
          JsonNode *old_node = priv->current_node;

          priv->current_node = json_node_new (JSON_NODE_ARRAY);

          token = json_parse_array (parser, scanner, TRUE);

          node = priv->current_node;
          priv->current_node = old_node;

          if (token != G_TOKEN_NONE)
            {
              g_free (name);
              json_node_free (node);
              json_object_unref (object);

              return token;
            }

          json_object_set_member (object, name, node);
          node->parent = priv->current_node;
          
          g_signal_emit (parser, parser_signals[OBJECT_MEMBER], 0,
                         object,
                         name);

          g_free (name);

          token = json_scanner_get_next_token (scanner);
          if (token == G_TOKEN_RIGHT_CURLY)
            break;

          if (token != G_TOKEN_COMMA)
            {
              json_object_unref (object);

              return G_TOKEN_RIGHT_CURLY;
            }

          continue;
        }

      if (token == '-')
        {
          guint next_token = json_scanner_peek_next_token (scanner);

          if (next_token == G_TOKEN_INT || next_token == G_TOKEN_FLOAT)
            {
              negative = TRUE;
              token = json_scanner_get_next_token (scanner);
            }
          else
            {
              g_free (name);
              json_object_unref (object);

              return G_TOKEN_INT;
            }
        }

      switch (token)
        {
        case G_TOKEN_INT:
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_int (node, negative ? scanner->value.v_int64 * -1
                                            : scanner->value.v_int64);
          break;

        case G_TOKEN_FLOAT:
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_double (node, negative ? scanner->value.v_float * -1.0
                                               : scanner->value.v_float);
          break;

        case G_TOKEN_STRING:
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_string (node, scanner->value.v_string);
          break;

        case JSON_TOKEN_TRUE:
        case JSON_TOKEN_FALSE:
          node = json_node_new (JSON_NODE_VALUE);
          json_node_set_boolean (node, token == JSON_TOKEN_TRUE ? TRUE : FALSE);
          break;

        case JSON_TOKEN_NULL:
          node = json_node_new (JSON_NODE_NULL);
          break;

        default:
          g_free (name);
          json_object_unref (object);
          return G_TOKEN_SYMBOL;
        }

      if (node)
        {
          json_object_set_member (object, name, node);
          node->parent = priv->current_node;
          
          g_signal_emit (parser, parser_signals[OBJECT_MEMBER], 0,
                         object,
                         name);
        }

      g_free (name);

      token = json_scanner_get_next_token (scanner);
      if (token != G_TOKEN_COMMA && token != G_TOKEN_RIGHT_CURLY)
        return G_TOKEN_RIGHT_CURLY;
    }

  json_node_take_object (priv->current_node, object);

  g_signal_emit (parser, parser_signals[OBJECT_END], 0, object);

  return G_TOKEN_NONE;
}

static guint
json_parse_statement (JsonParser  *parser,
                      JsonScanner *scanner)
{
  JsonParserPrivate *priv = parser->priv;
  guint token;

  token = json_scanner_peek_next_token (scanner);
  switch (token)
    {
    case G_TOKEN_LEFT_CURLY:
      priv->root = priv->current_node = json_node_new (JSON_NODE_OBJECT);
      return json_parse_object (parser, scanner, FALSE);

    case G_TOKEN_LEFT_BRACE:
      priv->root = priv->current_node = json_node_new (JSON_NODE_ARRAY);
      return json_parse_array (parser, scanner, FALSE);

    /* some web APIs are not only passing the data structures: they are
     * also passing an assigment, which makes parsing horribly complicated
     * only because web developers are lazy, and writing "var foo = " is
     * evidently too much to request from them.
     */
    case JSON_TOKEN_VAR:
      {
        guint next_token;
        gchar *name;

        /* swallow the 'var' token... */
        token = json_scanner_get_next_token (scanner);

        /* ... swallow the variable name... */
        next_token = json_scanner_get_next_token (scanner);
        if (next_token != G_TOKEN_IDENTIFIER)
          return G_TOKEN_IDENTIFIER;

        name = g_strdup (scanner->value.v_identifier);

        /* ... and finally swallow the '=' */
        next_token = json_scanner_get_next_token (scanner);
        if (next_token != '=')
          return '=';

        priv->has_assignment = TRUE;
        priv->variable_name = name;

        token = json_parse_statement (parser, scanner);

        /* remove the trailing semi-colon */
        next_token = json_scanner_peek_next_token (scanner);
        if (next_token == ';')
          {
            token = json_scanner_get_next_token (scanner);
            return G_TOKEN_NONE;
          }

        return token;
      }
      break;

    case JSON_TOKEN_NULL:
      priv->root = priv->current_node = json_node_new (JSON_NODE_NULL);
      json_scanner_get_next_token (scanner);
      return G_TOKEN_NONE;

    case JSON_TOKEN_TRUE:
    case JSON_TOKEN_FALSE:
      priv->root = priv->current_node = json_node_new (JSON_NODE_VALUE);
      json_node_set_boolean (priv->current_node,
                             token == JSON_TOKEN_TRUE ? TRUE : FALSE);
      json_scanner_get_next_token (scanner);
      return G_TOKEN_NONE;

    case '-':
      {
        guint next_token;
        
        token = json_scanner_get_next_token (scanner);
        next_token = json_scanner_peek_next_token (scanner);

        if (next_token == G_TOKEN_INT || next_token == G_TOKEN_FLOAT)
          {
            priv->root = priv->current_node = json_node_new (JSON_NODE_VALUE);
            
            token = json_scanner_get_next_token (scanner);
            switch (token)
              {
              case G_TOKEN_INT:
                json_node_set_int (priv->current_node,
                                   scanner->value.v_int64 * -1);
                break;
              case G_TOKEN_FLOAT:
                json_node_set_double (priv->current_node,
                                      scanner->value.v_float * -1.0);
                break;
              default:
                return G_TOKEN_INT;
              }

            json_scanner_get_next_token (scanner);
            return G_TOKEN_NONE;
          }
        else
          return G_TOKEN_INT;
      }
      break;

    case G_TOKEN_INT:
    case G_TOKEN_FLOAT:
    case G_TOKEN_STRING:
      priv->root = priv->current_node = json_node_new (JSON_NODE_VALUE);

      if (token == G_TOKEN_INT)
        json_node_set_int (priv->current_node, scanner->value.v_int64);
      else if (token == G_TOKEN_FLOAT)
        json_node_set_double (priv->current_node, scanner->value.v_float);
      else
        json_node_set_string (priv->current_node, scanner->value.v_string);

      json_scanner_get_next_token (scanner);
      return G_TOKEN_NONE;

    default:
      json_scanner_get_next_token (scanner);
      return G_TOKEN_SYMBOL;
    }
}

static void
json_scanner_msg_handler (JsonScanner *scanner,
                          gchar    *message,
                          gboolean  is_error)
{
  JsonParser *parser = scanner->user_data;
  JsonParserPrivate *priv = parser->priv;

  if (is_error)
    {
      GError *error = NULL;

      g_set_error (&error, JSON_PARSER_ERROR,
                   JSON_PARSER_ERROR_PARSE,
                   "%s:%d: Parse error: %s",
                   priv->is_filename ? priv->filename : "<none>",
                   scanner->line,
                   message);
      
      parser->priv->last_error = error;
      g_signal_emit (parser, parser_signals[ERROR], 0, error);
    }
  else
    g_warning ("%s:%d: Parse error: %s",
               priv->is_filename ? priv->filename : "<none>",
               scanner->line,
               message);
}

static JsonScanner *
json_scanner_create (JsonParser *parser)
{
  JsonScanner *scanner;
  gint i;

  scanner = json_scanner_new ();
  scanner->msg_handler = json_scanner_msg_handler;
  scanner->user_data = parser;

  for (i = 0; i < n_symbols; i++)
    {
      json_scanner_scope_add_symbol (scanner, 0,
                                     symbol_names + symbols[i].name_offset,
                                     GINT_TO_POINTER (symbols[i].token));
    }

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

static gboolean
json_parser_load (JsonParser   *parser,
                  const gchar  *data,
                  gsize         length,
                  GError      **error)
{
  JsonParserPrivate *priv = parser->priv;
  JsonScanner *scanner;
  gboolean done;
  gboolean retval = TRUE;
  gint i;

  json_parser_clear (parser);

  scanner = json_scanner_create (parser);
  json_scanner_input_text (scanner, data, length);

  priv->scanner = scanner;

  g_signal_emit (parser, parser_signals[PARSE_START], 0);

  done = FALSE;
  while (!done)
    {
      if (json_scanner_peek_next_token (scanner) == G_TOKEN_EOF)
        done = TRUE;
      else
        {
          guint expected_token;

          /* we try to show the expected token, if possible */
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
                          symbol_name = symbol_names + symbols[i].name_offset;

                      if (!msg)
                        msg = g_strconcat ("e.g. '", symbol_name, "'", NULL);
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
              json_scanner_unexp_token (scanner, expected_token,
                                        NULL, "keyword",
                                        symbol_name, msg,
                                        TRUE);

              /* and this will propagate the error we create in the
               * same message handler
               */
              if (priv->last_error)
                {
                  g_propagate_error (error, priv->last_error);
                  priv->last_error = NULL;
                }

              retval = FALSE;

              g_free (msg);
              done = TRUE;
            }
        }
    }

  g_signal_emit (parser, parser_signals[PARSE_END], 0);

  /* remove the scanner */
  json_scanner_destroy (scanner);
  priv->scanner = NULL;
  priv->current_node = NULL;

  return retval;
}

/**
 * json_parser_load_from_file:
 * @parser: a #JsonParser
 * @filename: the path for the file to parse
 * @error: return location for a #GError, or %NULL
 *
 * Loads a JSON stream from the content of @filename and parses it. See
 * json_parser_load_from_data().
 *
 * Return value: %TRUE if the file was successfully loaded and parsed.
 *   In case of error, @error is set accordingly and %FALSE is returned
 */
gboolean
json_parser_load_from_file (JsonParser   *parser,
                            const gchar  *filename,
                            GError      **error)
{
  JsonParserPrivate *priv;
  GError *internal_error;
  gchar *data;
  gsize length;
  gboolean retval = TRUE;

  g_return_val_if_fail (JSON_IS_PARSER (parser), FALSE);
  g_return_val_if_fail (filename != NULL, FALSE);

  priv = parser->priv;

  internal_error = NULL;
  if (!g_file_get_contents (filename, &data, &length, &internal_error))
    {
      g_propagate_error (error, internal_error);
      return FALSE;
    }

  g_free (priv->filename);

  priv->is_filename = TRUE;
  priv->filename = g_strdup (filename);

  if (!json_parser_load (parser, data, length, &internal_error))
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
 * Loads a JSON stream from a buffer and parses it. You can call this function
 * multiple times with the same #JsonParser object, but the contents of the
 * parser will be destroyed each time.
 *
 * Return value: %TRUE if the buffer was succesfully parser. In case
 *   of error, @error is set accordingly and %FALSE is returned
 */
gboolean
json_parser_load_from_data (JsonParser   *parser,
                            const gchar  *data,
                            gssize        length,
                            GError      **error)
{
  JsonParserPrivate *priv;
  GError *internal_error;
  gboolean retval = TRUE;

  g_return_val_if_fail (JSON_IS_PARSER (parser), FALSE);
  g_return_val_if_fail (data != NULL, FALSE);

  priv = parser->priv;

  if (length < 0)
    length = strlen (data);

  priv->is_filename = FALSE;
  g_free (priv->filename);

  internal_error = NULL;
  if (!json_parser_load (parser, data, length, &internal_error))
    {
      g_propagate_error (error, internal_error);
      retval = FALSE;
    }

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

/**
 * json_parser_get_current_line:
 * @parser: a #JsonParser
 *
 * Retrieves the line currently parsed, starting from 1.
 *
 * This function has defined behaviour only while parsing; calling this
 * function from outside the signal handlers emitted by #JsonParser will
 * yield 0.
 *
 * Return value: the currently parsed line, or 0.
 */
guint
json_parser_get_current_line (JsonParser *parser)
{
  g_return_val_if_fail (JSON_IS_PARSER (parser), 0);

  if (parser->priv->scanner)
    return json_scanner_cur_line (parser->priv->scanner);

  return 0;
}

/**
 * json_parser_get_current_pos:
 * @parser: a #JsonParser
 *
 * Retrieves the current position inside the current line, starting
 * from 0.
 *
 * This function has defined behaviour only while parsing; calling this
 * function from outside the signal handlers emitted by #JsonParser will
 * yield 0.
 *
 * Return value: the position in the current line, or 0.
 */
guint
json_parser_get_current_pos (JsonParser *parser)
{
  g_return_val_if_fail (JSON_IS_PARSER (parser), 0);

  if (parser->priv->scanner)
    return json_scanner_cur_line (parser->priv->scanner);

  return 0;
}

/**
 * json_parser_has_assignment:
 * @parser: a #JsonParser
 * @variable_name: return location for the variable name, or %NULL
 *
 * A JSON data stream might sometimes contain an assignment, like:
 *
 * |[
 *   var _json_data = { "member_name" : [ ...
 * ]|
 *
 * even though it would technically constitute a violation of the RFC.
 *
 * #JsonParser will ignore the left hand identifier and parse the right
 * hand value of the assignment. #JsonParser will record, though, the
 * existence of the assignment in the data stream and the variable name
 * used.
 *
 * Return value: %TRUE if there was an assignment, %FALSE otherwise. If
 *   @variable_name is not %NULL it will be set to the name of the variable
 *   used in the assignment. The string is owned by #JsonParser and should
 *   never be modified or freed.
 *
 * Since: 0.4
 */
gboolean
json_parser_has_assignment (JsonParser  *parser,
                            gchar      **variable_name)
{
  JsonParserPrivate *priv;

  g_return_val_if_fail (JSON_IS_PARSER (parser), FALSE);

  priv = parser->priv;

  if (priv->has_assignment && variable_name)
    *variable_name = priv->variable_name;

  return priv->has_assignment;
}
