#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <json-glib/json-types.h>

static void
test_empty_object (void)
{
  JsonObject *object = json_object_new ();

  g_assert_cmpint (json_object_get_size (object), ==, 0);
  g_assert (json_object_get_members (object) == NULL);

  json_object_unref (object);
}

static void
test_add_member (void)
{
  JsonObject *object = json_object_new ();
  JsonNode *node = json_node_new (JSON_NODE_NULL);

  g_assert_cmpint (json_object_get_size (object), ==, 0);

  json_object_set_member (object, "Null", node);
  g_assert_cmpint (json_object_get_size (object), ==, 1);

  node = json_object_get_member (object, "Null");
  g_assert_cmpint (JSON_NODE_TYPE (node), ==, JSON_NODE_NULL);

  json_object_unref (object);
}

static void
test_remove_member (void)
{
  JsonObject *object = json_object_new ();
  JsonNode *node = json_node_new (JSON_NODE_NULL);

  json_object_set_member (object, "Null", node);

  json_object_remove_member (object, "Null");
  g_assert_cmpint (json_object_get_size (object), ==, 0);

  json_object_unref (object);
}

int
main (int   argc,
      char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/object/empty-object", test_empty_object);
  g_test_add_func ("/object/add-member", test_add_member);
  g_test_add_func ("/object/remove-member", test_remove_member);

  return g_test_run ();
}
