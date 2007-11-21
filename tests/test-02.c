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
  "[ [ false, true, 42 ], [ true, false, 3.14 ], \"test\" ]",
  "[ true, { } ]",
  "[ false, { \"test\" : 42 } ]",
  "var test = [ false, false, true ]",
  "var test = [ true, 42 ];",
};

static guint n_test_arrays = G_N_ELEMENTS (test_arrays);

static void print_array  (gint indent, JsonArray  *array);
static void print_value  (gint indent, JsonNode   *node);
static void print_object (gint indent, JsonObject *object);

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

    case G_TYPE_DOUBLE:
      g_print ("%sFound float: `%f'\n",
               indent_str,
               g_value_get_double (&value));
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
          g_print ("%sFound array (index: %d, len: %d)\n",
                   indent_str,
                   j,
                   json_array_get_length (json_node_get_array (node)));
          print_array (indent + 1, json_node_get_array (node));
          break;
        case JSON_NODE_OBJECT:
          g_print ("%sFound object (index: %d, size: %d)\n",
                   indent_str,
                   j,
                   json_object_get_size (json_node_get_object (node)));
          print_object (indent + 1, json_node_get_object (node));
          break;
        case JSON_NODE_VALUE:
          g_print ("%sFound value (index: %d)\n", indent_str, j);
          print_value (indent + 1, node);
          break;
        case JSON_NODE_NULL:
          g_print ("%sFound null (index: %d)\n", indent_str, j);
          break;
        }
    }
}

static void
print_object (gint        indent,
              JsonObject *object)
{
  gint object_size = json_object_get_size (object);
  gint j;
  gchar indent_str[80];
  GList *members, *l;

  for (j = 0; j < indent; j++)
    indent_str[j] = ' ';
  
  indent_str[j] = '\0';

  if (object_size == 0)
    {
      g_print ("%sFound empty object\n", indent_str);
      return;
    }

  members = json_object_get_members (object);
  for (l = members; l; l = l->next)
    {
      const gchar *name = l->data;
      JsonNode *node = json_object_get_member (object, name);

      switch (JSON_NODE_TYPE (node))
        {
        case JSON_NODE_ARRAY:
          g_print ("%sFound array (member: `%s', len: %d)\n",
                   indent_str,
                   name,
                   json_array_get_length (json_node_get_array (node)));
          print_array (indent + 1, json_node_get_array (node));
          break;
        case JSON_NODE_OBJECT:
          g_print ("%sFound object (member: `%s', size: %d)\n",
                   indent_str,
                   name,
                   json_object_get_size (json_node_get_object (node)));
          print_object (indent + 1, json_node_get_object (node));
          break;
        case JSON_NODE_VALUE:
          g_print ("%sFound value (member: `%s')\n", indent_str, name);
          print_value (indent + 1, node);
          break;
        case JSON_NODE_NULL:
          g_print ("%sFound null (member: `%s')\n", indent_str, name);
          break;
        }
    }

  g_list_free (members);
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
      gchar *var_name;

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

      g_print ("*** Test %d: '%s' ***\n", i, test_arrays[i]);
      if (json_parser_has_assignment (parser, &var_name))
        g_print ("*** Test %d: assigns '%s'\n", i, var_name);

      print_array (1, array);

      json_node_free (node);
    }

  g_object_unref (parser);

  return EXIT_SUCCESS;
}
