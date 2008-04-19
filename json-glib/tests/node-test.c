#include <glib/gtestutils.h>
#include <json-glib/json-types.h>
#include <string.h>

static void
test_copy (void)
{
  JsonNode *node = json_node_new (JSON_NODE_NULL);
  JsonNode *copy = json_node_copy (node);

  g_assert_cmpint (node->type, ==, copy->type);
  g_assert_cmpint (json_node_get_value_type (node), ==, json_node_get_value_type (copy));
  g_assert_cmpstr (json_node_type_name (node), ==, json_node_type_name (copy));

  json_node_free (copy);
  json_node_free (node);
}

static void
test_null (void)
{
  JsonNode *node = json_node_new (JSON_NODE_NULL);

  g_assert_cmpint (node->type, ==, JSON_NODE_NULL);
  g_assert_cmpint (json_node_get_value_type (node), ==, G_TYPE_INVALID);
  g_assert_cmpstr (json_node_type_name (node), ==, "NULL");

  json_node_free (node);
}

static void
test_value (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);
  GValue value = { 0, };
  GValue check = { 0, };

  g_assert_cmpint (JSON_NODE_TYPE (node), ==, JSON_NODE_VALUE);

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 42);

  g_assert_cmpint (G_VALUE_TYPE (&value), ==, G_TYPE_INT);
  g_assert_cmpint (g_value_get_int (&value), ==, 42);

  json_node_set_value (node, &value);
  json_node_get_value (node, &check);

  g_assert_cmpint (G_VALUE_TYPE (&value), ==, G_VALUE_TYPE (&check));
  g_assert_cmpint (g_value_get_int (&value), ==, g_value_get_int (&check));
  g_assert_cmpint (G_VALUE_TYPE (&check), ==, G_TYPE_INT);
  g_assert_cmpint (g_value_get_int (&check), ==, 42);

  g_value_unset (&value);
  g_value_unset (&check);
  json_node_free (node);
}

int
main (int   argc,
      char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/nodes/null-node", test_null);
  g_test_add_func ("/nodes/copy-node", test_copy);
  g_test_add_func ("/nodes/value", test_value);

  return g_test_run ();
}
