#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <json-glib/json-glib.h>

static const gchar *test_empty_string = "";
static const gchar *test_empty_array_string = "[ ]";
static const gchar *test_empty_object_string = "{ }";

static const struct {
  const gchar *str;
  JsonNodeType type;
  GType gtype;
} test_base_values[] = {
  { "null",       JSON_NODE_NULL, G_TYPE_INVALID },
  { "42",         JSON_NODE_VALUE, G_TYPE_INT64 },
  { "true",       JSON_NODE_VALUE, G_TYPE_BOOLEAN },
  { "\"string\"", JSON_NODE_VALUE, G_TYPE_STRING },
  { "10.2e3",     JSON_NODE_VALUE, G_TYPE_DOUBLE }
};

static const struct {
  const gchar *str;
  gint len;
  gint element;
  JsonNodeType type;
  GType gtype;
} test_simple_arrays[] = {
  { "[ true ]",                 1, 0, JSON_NODE_VALUE, G_TYPE_BOOLEAN },
  { "[ true, false, null ]",    3, 2, JSON_NODE_NULL,  G_TYPE_INVALID },
  { "[ 1, 2, 3.14, \"test\" ]", 4, 3, JSON_NODE_VALUE, G_TYPE_STRING  }
};

static const gchar *test_nested_arrays[] = {
  "[ 42, [ ], null ]",
  "[ [ ], [ true, [ true ] ] ]",
  "[ [ false, true, 42 ], [ true, false, 3.14 ], \"test\" ]",
  "[ true, { } ]",
  "[ false, { \"test\" : 42 } ]",
  "[ { \"test\" : 42 }, null ]",
  "[ true, { \"test\" : 42 }, null ]",
  "[ { \"channel\" : \"/meta/connect\" } ]"
};

static const struct {
  const gchar *str;
  gint size;
  const gchar *member;
  JsonNodeType type;
  GType gtype;
} test_simple_objects[] = {
  { "{ \"test\" : 42 }", 1, "test", JSON_NODE_VALUE, G_TYPE_INT64 },
  { "{ \"name\" : \"\", \"state\" : 1 }", 2, "name", JSON_NODE_VALUE, G_TYPE_STRING },
  { "{ \"foo\" : \"bar\", \"baz\" : null }", 2, "baz", JSON_NODE_NULL, G_TYPE_INVALID },
  { "{ \"channel\" : \"/meta/connect\" }", 1, "channel", JSON_NODE_VALUE, G_TYPE_STRING }
};

static const gchar *test_nested_objects[] = {
  "{ \"array\" : [ false, \"foo\" ], \"object\" : { \"foo\" : true } }"
};

static const struct {
  const gchar *str;
  const gchar *var;
} test_assignments[] = {
  { "var foo = [ false, false, true ]", "foo" },
  { "var bar = [ true, 42 ];", "bar" },
  { "var baz = { \"foo\" : false }", "baz" }
};

static const struct
{
  const gchar *str;
  const gchar *member;
  const gchar *match;
} test_unicode[] = {
  { "{ \"test\" : \"foo \\u00e8\" }", "test", "foo è" }
};

static guint n_test_base_values    = G_N_ELEMENTS (test_base_values);
static guint n_test_simple_arrays  = G_N_ELEMENTS (test_simple_arrays);
static guint n_test_nested_arrays  = G_N_ELEMENTS (test_nested_arrays);
static guint n_test_simple_objects = G_N_ELEMENTS (test_simple_objects);
static guint n_test_nested_objects = G_N_ELEMENTS (test_nested_objects);
static guint n_test_assignments    = G_N_ELEMENTS (test_assignments);
static guint n_test_unicode        = G_N_ELEMENTS (test_unicode);

static void
test_empty (void)
{
  JsonParser *parser;
  GError *error = NULL;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with empty string...\n");

  if (!json_parser_load_from_data (parser, test_empty_string, -1, &error))
    {
      if (g_test_verbose ())
        g_print ("Error: %s\n", error->message);
      g_error_free (error);
      g_object_unref (parser);
      exit (1);
    }
  else
    {
      if (g_test_verbose ())
        g_print ("checking json_parser_get_root...\n");

      g_assert (NULL == json_parser_get_root (parser));
    }

  g_object_unref (parser);
}

static void
test_base_value (void)
{
  gint i;
  JsonParser *parser;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with base-values...\n");

  for (i = 0; i < n_test_base_values; i++)
    {
      GError *error = NULL;

      if (!json_parser_load_from_data (parser, test_base_values[i].str, -1, &error))
        {
          if (g_test_verbose ())
            g_print ("Error: %s\n", error->message);

          g_error_free (error);
          g_object_unref (parser);
          exit (1);
        }
      else
        {
          JsonNode *root;

          g_assert (NULL != json_parser_get_root (parser));

          root = json_parser_get_root (parser);
          g_assert (root != NULL);
          g_assert (json_node_get_parent (root) == NULL);

          if (g_test_verbose ())
            g_print ("checking root node is of the desired type %s...\n",
                     test_base_values[i].gtype == G_TYPE_INVALID ? "<null>"
                                                                 : g_type_name (test_base_values[i].gtype));
          g_assert_cmpint (JSON_NODE_TYPE (root), ==, test_base_values[i].type);
          g_assert_cmpint (json_node_get_value_type (root), ==, test_base_values[i].gtype);
       }
    }

  g_object_unref (parser);
}

static void
test_empty_array (void)
{
  JsonParser *parser;
  GError *error = NULL;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with empty array...\n");

  if (!json_parser_load_from_data (parser, test_empty_array_string, -1, &error))
    {
      if (g_test_verbose ())
        g_print ("Error: %s\n", error->message);
      g_error_free (error);
      g_object_unref (parser);
      exit (1);
    }
  else
    {
      JsonNode *root;
      JsonArray *array;

      g_assert (NULL != json_parser_get_root (parser));

      if (g_test_verbose ())
        g_print ("checking root node is an array...\n");
      root = json_parser_get_root (parser);
      g_assert_cmpint (JSON_NODE_TYPE (root), ==, JSON_NODE_ARRAY);
      g_assert (json_node_get_parent (root) == NULL);

      array = json_node_get_array (root);
      g_assert (array != NULL);

      if (g_test_verbose ())
        g_print ("checking array is empty...\n");
      g_assert_cmpint (json_array_get_length (array), ==, 0);
    }

  g_object_unref (parser);
}

static void
test_simple_array (void)
{
  gint i;
  JsonParser *parser;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with simple arrays...\n");

  for (i = 0; i < n_test_simple_arrays; i++)
    {
      GError *error = NULL;

      if (!json_parser_load_from_data (parser, test_simple_arrays[i].str, -1, &error))
        {
          if (g_test_verbose ())
            g_print ("Error: %s\n", error->message);

          g_error_free (error);
          g_object_unref (parser);
          exit (1);
        }
      else
        {
          JsonNode *root, *node;
          JsonArray *array;

          g_assert (NULL != json_parser_get_root (parser));

          if (g_test_verbose ())
            g_print ("checking root node is an array...\n");
          root = json_parser_get_root (parser);
          g_assert_cmpint (JSON_NODE_TYPE (root), ==, JSON_NODE_ARRAY);
          g_assert (json_node_get_parent (root) == NULL);

          array = json_node_get_array (root);
          g_assert (array != NULL);

          if (g_test_verbose ())
            g_print ("checking array is of the desired length (%d)...\n",
                     test_simple_arrays[i].len);
          g_assert_cmpint (json_array_get_length (array), ==, test_simple_arrays[i].len);

          if (g_test_verbose ())
            g_print ("checking element %d is of the desired type %s...\n",
                     test_simple_arrays[i].element,
                     g_type_name (test_simple_arrays[i].gtype));
          node = json_array_get_element (array, test_simple_arrays[i].element);
          g_assert (node != NULL);
          g_assert (json_node_get_parent (node) == root);
          g_assert_cmpint (JSON_NODE_TYPE (node), ==, test_simple_arrays[i].type);
          g_assert_cmpint (json_node_get_value_type (node), ==, test_simple_arrays[i].gtype);
        }
    }

  g_object_unref (parser);
}

static void
test_nested_array (void)
{
  gint i;
  JsonParser *parser;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with nested arrays...\n");

  for (i = 0; i < n_test_nested_arrays; i++)
    {
      GError *error = NULL;

      if (!json_parser_load_from_data (parser, test_nested_arrays[i], -1, &error))
        {
          if (g_test_verbose ())
            g_print ("Error: %s\n", error->message);

          g_error_free (error);
          g_object_unref (parser);
          exit (1);
        }
      else
        {
          JsonNode *root;
          JsonArray *array;

          g_assert (NULL != json_parser_get_root (parser));

          if (g_test_verbose ())
            g_print ("checking root node is an array...\n");
          root = json_parser_get_root (parser);
          g_assert_cmpint (JSON_NODE_TYPE (root), ==, JSON_NODE_ARRAY);
          g_assert (json_node_get_parent (root) == NULL);

          array = json_node_get_array (root);
          g_assert (array != NULL);

          if (g_test_verbose ())
            g_print ("checking array is not empty...\n");
          g_assert_cmpint (json_array_get_length (array), >, 0);
        }
    }

  g_object_unref (parser);
}

static void
test_empty_object (void)
{
  JsonParser *parser;
  GError *error = NULL;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with empty object...\n");

  if (!json_parser_load_from_data (parser, test_empty_object_string, -1, &error))
    {
      if (g_test_verbose ())
        g_print ("Error: %s\n", error->message);
      g_error_free (error);
      g_object_unref (parser);
      exit (1);
    }
  else
    {
      JsonNode *root;
      JsonObject *object;

      g_assert (NULL != json_parser_get_root (parser));

      if (g_test_verbose ())
        g_print ("checking root node is an object...\n");
      root = json_parser_get_root (parser);
      g_assert (json_node_get_parent (root) == NULL);
      g_assert_cmpint (JSON_NODE_TYPE (root), ==, JSON_NODE_OBJECT);
      g_assert (json_node_get_parent (root) == NULL);

      object = json_node_get_object (root);
      g_assert (object != NULL);

      if (g_test_verbose ())
        g_print ("checking object is empty...\n");
      g_assert_cmpint (json_object_get_size (object), ==, 0);
    }

  g_object_unref (parser);
}

static void
test_simple_object (void)
{
  gint i;
  JsonParser *parser;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with simple objects...\n");

  for (i = 0; i < n_test_simple_objects; i++)
    {
      GError *error = NULL;

      if (!json_parser_load_from_data (parser, test_simple_objects[i].str, -1, &error))
        {
          if (g_test_verbose ())
            g_print ("Error: %s\n", error->message);

          g_error_free (error);
          g_object_unref (parser);
          exit (1);
        }
      else
        {
          JsonNode *root, *node;
          JsonObject *object;

          g_assert (NULL != json_parser_get_root (parser));

          if (g_test_verbose ())
            g_print ("checking root node is an object...\n");
          root = json_parser_get_root (parser);
          g_assert_cmpint (JSON_NODE_TYPE (root), ==, JSON_NODE_OBJECT);
          g_assert (json_node_get_parent (root) == NULL);

          object = json_node_get_object (root);
          g_assert (object != NULL);

          if (g_test_verbose ())
            g_print ("checking object is of the desired size (%d)...\n",
                     test_simple_objects[i].size);
          g_assert_cmpint (json_object_get_size (object), ==, test_simple_objects[i].size);

          if (g_test_verbose ())
            g_print ("checking member '%s' is of the desired type %s...\n",
                     test_simple_objects[i].member,
                     g_type_name (test_simple_objects[i].gtype));
          node = json_object_get_member (object, test_simple_objects[i].member);
          g_assert (node != NULL);
          g_assert (json_node_get_parent (node) == root);
          g_assert_cmpint (JSON_NODE_TYPE (node), ==, test_simple_objects[i].type);
          g_assert_cmpint (json_node_get_value_type (node), ==, test_simple_objects[i].gtype);
        }
    }

  g_object_unref (parser);
}

static void
test_nested_object (void)
{
  gint i;
  JsonParser *parser;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with nested objects...\n");

  for (i = 0; i < n_test_nested_objects; i++)
    {
      GError *error = NULL;

      if (!json_parser_load_from_data (parser, test_nested_objects[i], -1, &error))
        {
          if (g_test_verbose ())
            g_print ("Error: %s\n", error->message);

          g_error_free (error);
          g_object_unref (parser);
          exit (1);
        }
      else
        {
          JsonNode *root;
          JsonObject *object;

          g_assert (NULL != json_parser_get_root (parser));

          if (g_test_verbose ())
            g_print ("checking root node is an object...\n");
          root = json_parser_get_root (parser);
          g_assert_cmpint (JSON_NODE_TYPE (root), ==, JSON_NODE_OBJECT);
          g_assert (json_node_get_parent (root) == NULL);

          object = json_node_get_object (root);
          g_assert (object != NULL);

          if (g_test_verbose ())
            g_print ("checking object is not empty...\n");
          g_assert_cmpint (json_object_get_size (object), >, 0);
        }
    }

  g_object_unref (parser);
}

static void
test_assignment (void)
{
  gint i;
  JsonParser *parser;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with assignments...\n");

  for (i = 0; i < n_test_assignments; i++)
    {
      GError *error = NULL;

      if (!json_parser_load_from_data (parser, test_assignments[i].str, -1, &error))
        {
          if (g_test_verbose ())
            g_print ("Error: %s\n", error->message);

          g_error_free (error);
          g_object_unref (parser);
          exit (1);
        }
      else
        {
          gchar *var;

          if (g_test_verbose ())
            g_print ("checking assignment...\n");

          g_assert (json_parser_has_assignment (parser, &var) == TRUE);
          g_assert (var != NULL);
          g_assert_cmpstr (var, ==, test_assignments[i].var);
          g_assert (NULL != json_parser_get_root (parser));
        }
    }

  g_object_unref (parser);
}

static void
test_unicode_escape (void)
{
  gint i;
  JsonParser *parser;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("checking json_parser_load_from_data with unicode escape...\n");

  for (i = 0; i < n_test_unicode; i++)
    {
      GError *error = NULL;

      if (!json_parser_load_from_data (parser, test_unicode[i].str, -1, &error))
        {
          if (g_test_verbose ())
            g_print ("Error: %s\n", error->message);

          g_error_free (error);
          g_object_unref (parser);
          exit (1);
        }
      else
        {
          JsonNode *root, *node;
          JsonObject *object;

          g_assert (NULL != json_parser_get_root (parser));

          if (g_test_verbose ())
            g_print ("checking root node is an object...\n");
          root = json_parser_get_root (parser);
          g_assert_cmpint (JSON_NODE_TYPE (root), ==, JSON_NODE_OBJECT);

          object = json_node_get_object (root);
          g_assert (object != NULL);

          if (g_test_verbose ())
            g_print ("checking object is not empty...\n");
          g_assert_cmpint (json_object_get_size (object), >, 0);

          node = json_object_get_member (object, test_unicode[i].member);
          g_assert_cmpint (JSON_NODE_TYPE (node), ==, JSON_NODE_VALUE);

          if (g_test_verbose ())
            g_print ("checking simple string equality...\n");
          g_assert_cmpstr (json_node_get_string (node), ==, test_unicode[i].match);

          if (g_test_verbose ())
            g_print ("checking for valid UTF-8...\n");
          g_assert (g_utf8_validate (json_node_get_string (node), -1, NULL));
        }
    }

  g_object_unref (parser);
}

int
main (int   argc,
      char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/parser/empty-string", test_empty);
  g_test_add_func ("/parser/base-value", test_base_value);
  g_test_add_func ("/parser/empty-array", test_empty_array);
  g_test_add_func ("/parser/simple-array", test_simple_array);
  g_test_add_func ("/parser/nested-array", test_nested_array);
  g_test_add_func ("/parser/empty-object", test_empty_object);
  g_test_add_func ("/parser/simple-object", test_simple_object);
  g_test_add_func ("/parser/nested-object", test_nested_object);
  g_test_add_func ("/parser/assignment", test_assignment);
  g_test_add_func ("/parser/unicode-escape", test_unicode_escape);

  return g_test_run ();
}
