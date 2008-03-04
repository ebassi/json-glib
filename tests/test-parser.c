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
  "[ { \"channel\" : \"/meta/connect\" } ]"
};

static const gchar *test_simple_objects[] = {
  "{ \"test\" : 42 }",
  "{ \"foo\" : \"bar\", \"baz\" : null }"
};

static const gchar *test_nested_objects[] = {
  "{ \"array\" : [ false, \"foo\" ], \"object\" : { \"foo\" : true } }"
};

static const gchar *test_assignments[] = {
  "var test = [ false, false, true ]",
  "var test = [ true, 42 ];",
  "var test = { \"foo\" : false }"
};

static guint n_test_simple_arrays  = G_N_ELEMENTS (test_simple_arrays);
static guint n_test_nested_arrays  = G_N_ELEMENTS (test_nested_arrays);
static guint n_test_simple_objects = G_N_ELEMENTS (test_simple_objects);
static guint n_test_nested_objects = G_N_ELEMENTS (test_nested_objects);
static guint n_test_assignments    = G_N_ELEMENTS (test_assignments);

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
      g_assert_cmpint (JSON_NODE_TYPE (root), ==, JSON_NODE_OBJECT);

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

      if (!json_parser_load_from_data (parser, test_simple_objects[i], -1, &error))
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

      if (!json_parser_load_from_data (parser, test_assignments[i], -1, &error))
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

          g_assert (NULL != json_parser_get_root (parser));
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

  g_test_add_func ("/json-parser/empty-string", test_empty);
  g_test_add_func ("/json-parser/empty-array", test_empty_array);
  g_test_add_func ("/json-parser/simple-array", test_simple_array);
  g_test_add_func ("/json-parser/nested-array", test_nested_array);
  g_test_add_func ("/json-parser/empty-object", test_empty_object);
  g_test_add_func ("/json-parser/simple-object", test_simple_object);
  g_test_add_func ("/json-parser/nested-object", test_nested_object);
  g_test_add_func ("/json-parser/assignment", test_assignment);

  return g_test_run ();
}
