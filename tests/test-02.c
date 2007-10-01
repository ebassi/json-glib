#include <stdlib.h>
#include <stdio.h>

#include <json-glib/json-glib.h>

static const gchar *test_arrays[] = {
  "[ ]",
  "[ true ]",
  "[ true, false, null ]",
  "[ 1, 2, 3.14, \"test\" ]",
  "[ 42, [ ], null ]",
  "[ [ ], [ true, [ true ] ] ]",
  "[ [ false, true, 42 ], [ true, false, 3.14 ], \"test\" ]"
};

static guint n_test_arrays = G_N_ELEMENTS (test_arrays);

static void
print_value (gint      indent,
             JsonNode *node)
{
  gint j;
  gchar indent_str[80];
  GValue value = { 0, };

  for (j = 0; j < indent; j++)
    indent_str[j] = ' ';

  indent_str[j] = '\0';

  json_node_get_value (node, &value);

  switch (G_VALUE_TYPE (&value))
    {
    case G_TYPE_INT:
      g_print ("%sFound integer: `%d'\n",
               indent_str,
               g_value_get_int (&value));
      break;

    case G_TYPE_STRING:
      g_print ("%sFound string: `%s'\n",
               indent_str,
               g_value_get_string (&value));
      break;

    case G_TYPE_FLOAT:
      g_print ("%sFound float: `%f'\n",
               indent_str,
               g_value_get_float (&value));
      break;

    case G_TYPE_BOOLEAN:
      g_print ("%sFound boolean: `%s'\n",
               indent_str,
               g_value_get_boolean (&value) ? "true" : "false");
      break;

    default:
      g_print ("%sUnknown value\n", indent_str);
      break;
    }

  g_value_unset (&value);
}

static void
print_array (gint       indent,
             JsonArray *array)
{
  gint array_len = json_array_get_length (array);
  gint j;
  gchar indent_str[80];

  for (j = 0; j < indent; j++)
    indent_str[j] = ' ';
  
  indent_str[j] = '\0';

  if (array_len == 0)
    {
      g_print ("%sFound empty array\n", indent_str);
      return;
    }

  for (j = 0; j < array_len; j++)
    {
      JsonNode *node = json_array_get_element (array, j);

      switch (JSON_NODE_TYPE (node))
        {
        case JSON_NODE_ARRAY:
          g_print ("%sFound array (len:%d)\n",
                   indent_str,
                   json_array_get_length (json_node_get_array (node)));
          print_array (indent + 1, json_node_get_array (node));
          break;
        case JSON_NODE_OBJECT:
          g_print ("%sFound object (size:%d)\n",
                   indent_str,
                   json_object_get_size (json_node_get_object (node)));
          break;
        case JSON_NODE_VALUE:
          print_value (indent, node);
          break;
        case JSON_NODE_NULL:
          g_print ("%sFound null\n", indent_str);
          break;
        }
    }
}

int
main (int argc, char *argv[])
{
  JsonParser *parser;
  gint i;

  g_type_init ();

  parser = json_parser_new ();

  for (i = 0; i < n_test_arrays; i++)
    {
      GError *error = NULL;
      JsonNode *node;
      JsonArray *array;

      if (!json_parser_load_from_data (parser, test_arrays[i], -1, &error))
        {
          g_print ("* Error, test %d:\n"
                   "* \t%s:\n"
                   "* Message: %s\n",
                   i, test_arrays[i], error->message);
          g_error_free (error);
          g_object_unref (parser);
          return EXIT_FAILURE;
        }

      node = json_parser_get_root (parser);
      g_assert (node != NULL);
      g_assert (JSON_NODE_TYPE (node) == JSON_NODE_ARRAY);

      array = json_node_get_array (node);
      g_assert (array != NULL);

      g_print ("*** Test %d ***\n", i);
      print_array (1, array);
    }

  g_object_unref (parser);

  return EXIT_SUCCESS;
}
