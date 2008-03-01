#include <glib/gtestutils.h>
#include <json-glib/json-types.h>
#include <string.h>

static void
test_null (void)
{
  JsonNode *node = json_node_new (JSON_NODE_NULL);

  g_assert_cmpint (node->type, ==, JSON_NODE_NULL);
  g_assert_cmpint (json_node_get_value_type (node), ==, G_TYPE_INVALID);
  g_assert_cmpstr (json_node_type_name (node), ==, "NULL");

  json_node_free (node);
}

int
main (int   argc,
      char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/nodes/null-node", test_null);

  return g_test_run ();
}
