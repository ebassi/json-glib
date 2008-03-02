#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <json-glib/json-types.h>

static void
test_empty_array (void)
{
  JsonArray *array = json_array_new ();

  g_assert_cmpint (json_array_get_length (array), ==, 0);
  g_assert (json_array_get_elements (array) == NULL);

  json_array_unref (array);
}

static void
test_add_element (void)
{
  JsonArray *array = json_array_new ();
  JsonNode *node = json_node_new (JSON_NODE_NULL);

  g_assert_cmpint (json_array_get_length (array), ==, 0);

  json_array_add_element (array, node);
  g_assert_cmpint (json_array_get_length (array), ==, 1);

  node = json_array_get_element (array, 0);
  g_assert_cmpint (JSON_NODE_TYPE (node), ==, JSON_NODE_NULL);

  json_array_remove_element (array, 0);
  g_assert_cmpint (json_array_get_length (array), ==, 0);

  json_array_unref (array);
}

int
main (int   argc,
      char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/array/empty-array", test_empty_array);
  g_test_add_func ("/array/add-element", test_add_element);

  return g_test_run ();
}
