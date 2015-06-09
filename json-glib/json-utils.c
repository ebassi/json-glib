/* json-utils.c - JSON utility API
 * 
 * This file is part of JSON-GLib
 * Copyright 2015  Emmanuele Bassi
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
 */

/**
 * SECTION:json-utils
 * @Title: Utility API
 * @Short_description: Various utility functions
 *
 * Various utility functions.
 */

#include "config.h"

#include "json-utils.h"
#include "json-parser.h"
#include "json-generator.h"

/**
 * json_from_string:
 * @str: a valid UTF-8 string containing JSON data
 * @error: return location for a #GError
 *
 * Parses the string in @str and returns a #JsonNode representing
 * the JSON tree.
 *
 * In case of parsing error, this function returns %NULL and sets
 * @error appropriately.
 *
 * Returns: (transfer full): a #JsonNode, or %NULL
 *
 * Since: 1.2
 */
JsonNode *
json_from_string (const char  *str,
                  GError     **error)
{
  JsonParser *parser;
  JsonNode *retval;

  g_return_val_if_fail (str != NULL, NULL);

  error = NULL;
  parser = json_parser_new ();
  if (!json_parser_load_from_data (parser, str, -1, error))
    {
      g_object_unref (parser);
      return NULL;
    }

  retval = json_node_copy (json_parser_get_root (parser));

  g_object_unref (parser);

  return retval;
}

/**
 * json_to_string:
 * @node: a #JsonNode
 * @pretty: whether the output should be prettyfied for printing
 *
 * Generates a stringified JSON representation of the contents of
 * the passed @node.
 *
 * Returns: (transfer full): the string representation of the #JsonNode
 *
 * Since: 1.2
 */
char *
json_to_string (JsonNode *node,
                gboolean  pretty)
{
  JsonGenerator *generator;
  char *retval;

  g_return_val_if_fail (node != NULL, NULL);

  generator = json_generator_new ();
  json_generator_set_pretty (generator, pretty);
  json_generator_set_root (generator, node);

  retval = json_generator_to_data (generator, NULL);

  g_object_unref (generator);

  return retval;
}
