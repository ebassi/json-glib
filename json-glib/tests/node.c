#include <glib.h>
#include <json-glib/json-glib.h>
#include <string.h>

static void
test_init_int (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);

  json_node_set_int (node, 42);
  g_assert_cmpint (json_node_get_int (node), ==, 42);

  json_node_free (node);
}

static void
test_init_double (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);

  json_node_set_double (node, 3.14159);
  g_assert_cmpfloat (json_node_get_double (node), ==, 3.14159);

  json_node_free (node);
}

static void
test_init_boolean (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);

  json_node_set_boolean (node, TRUE);
  g_assert (json_node_get_boolean (node));

  json_node_free (node);
}

static void
test_init_string (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);

  json_node_set_string (node, "Hello, World");
  g_assert_cmpstr (json_node_get_string (node), ==, "Hello, World");

  json_node_free (node);
}

static void
test_copy_null (void)
{
  JsonNode *node = json_node_new (JSON_NODE_NULL);
  JsonNode *copy = json_node_copy (node);

  g_assert_cmpint (json_node_get_node_type (node), ==, json_node_get_node_type (copy));
  g_assert_cmpint (json_node_get_value_type (node), ==, json_node_get_value_type (copy));
  g_assert_cmpstr (json_node_type_name (node), ==, json_node_type_name (copy));

  json_node_free (copy);
  json_node_free (node);
}

static void
test_copy_value (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);
  JsonNode *copy;

  json_node_set_string (node, "hello");

  copy = json_node_copy (node);
  g_assert_cmpint (json_node_get_node_type (node), ==, json_node_get_node_type (copy));
  g_assert_cmpstr (json_node_type_name (node), ==, json_node_type_name (copy));
  g_assert_cmpstr (json_node_get_string (node), ==, json_node_get_string (copy));

  json_node_free (copy);
  json_node_free (node);
}

static void
test_copy_object (void)
{
  JsonObject *obj = json_object_new ();
  JsonNode *node = json_node_new (JSON_NODE_OBJECT);
  JsonNode *value = json_node_new (JSON_NODE_VALUE);
  JsonNode *copy;

  json_node_set_int (value, 42);
  json_object_set_member (obj, "answer", value);

  json_node_take_object (node, obj);

  copy = json_node_copy (node);

  g_assert_cmpint (json_node_get_node_type (node), ==, json_node_get_node_type (copy));
  g_assert (json_node_get_object (node) == json_node_get_object (copy));

  json_node_free (copy);
  json_node_free (node);
}

static void
test_null (void)
{
  JsonNode *node = json_node_new (JSON_NODE_NULL);

  g_assert (JSON_NODE_HOLDS_NULL (node));
  g_assert (json_node_is_null (node));
  g_assert_cmpint (json_node_get_value_type (node), ==, G_TYPE_INVALID);
  g_assert_cmpstr (json_node_type_name (node), ==, "NULL");

  json_node_free (node);
}

static void
test_get_int (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);

  json_node_set_int (node, 0);
  g_assert_cmpint (json_node_get_int (node), ==, 0);
  g_assert_cmpfloat (json_node_get_double (node), ==, 0.0);
  g_assert (!json_node_get_boolean (node));
  g_assert (!json_node_is_null (node));

  json_node_set_int (node, 42);
  g_assert_cmpint (json_node_get_int (node), ==, 42);
  g_assert_cmpfloat (json_node_get_double (node), ==, 42.0);
  g_assert (json_node_get_boolean (node));
  g_assert (!json_node_is_null (node));

  json_node_free (node);
}

static void
test_get_double (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);

  json_node_set_double (node, 3.14);
  g_assert_cmpfloat (json_node_get_double (node), ==, 3.14);
  g_assert_cmpint (json_node_get_int (node), ==, 3);
  g_assert (json_node_get_boolean (node));

  json_node_free (node);
}

static void
test_gvalue (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);
  GValue value = { 0, };
  GValue check = { 0, };

  g_assert_cmpint (JSON_NODE_TYPE (node), ==, JSON_NODE_VALUE);

  g_value_init (&value, G_TYPE_INT64);
  g_value_set_int64 (&value, 42);

  g_assert_cmpint (G_VALUE_TYPE (&value), ==, G_TYPE_INT64);
  g_assert_cmpint (g_value_get_int64 (&value), ==, 42);

  json_node_set_value (node, &value);
  json_node_get_value (node, &check);

  g_assert_cmpint (G_VALUE_TYPE (&value), ==, G_VALUE_TYPE (&check));
  g_assert_cmpint (g_value_get_int64 (&value), ==, g_value_get_int64 (&check));
  g_assert_cmpint (G_VALUE_TYPE (&check), ==, G_TYPE_INT64);
  g_assert_cmpint (g_value_get_int64 (&check), ==, 42);

  g_value_unset (&value);
  g_value_unset (&check);

  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string (&value, "Hello, World!");

  g_assert_cmpint (G_VALUE_TYPE (&value), ==, G_TYPE_STRING);
  g_assert_cmpstr (g_value_get_string (&value), ==, "Hello, World!");

  json_node_set_value (node, &value);
  json_node_get_value (node, &check);

  g_assert_cmpint (G_VALUE_TYPE (&value), ==, G_VALUE_TYPE (&check));
  g_assert_cmpstr (g_value_get_string (&value), ==, g_value_get_string (&check));
  g_assert_cmpint (G_VALUE_TYPE (&check), ==, G_TYPE_STRING);
  g_assert_cmpstr (g_value_get_string (&check), ==, "Hello, World!");

  g_value_unset (&value);
  g_value_unset (&check);
  json_node_free (node);
}

static void
test_gvalue_autopromotion (void)
{
  JsonNode *node = json_node_new (JSON_NODE_VALUE);
  GValue value = { 0, };
  GValue check = { 0, };

  g_assert_cmpint (JSON_NODE_TYPE (node), ==, JSON_NODE_VALUE);

  if (g_test_verbose ())
    g_print ("Autopromotion of int to int64\n");

  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 42);

  json_node_set_value (node, &value);
  json_node_get_value (node, &check);

  if (g_test_verbose ())
    g_print ("Expecting an gint64, got a %s\n", g_type_name (G_VALUE_TYPE (&check)));

  g_assert_cmpint (G_VALUE_TYPE (&check), ==, G_TYPE_INT64);
  g_assert_cmpint (g_value_get_int64 (&check), ==, 42);
  g_assert_cmpint (G_VALUE_TYPE (&value), !=, G_VALUE_TYPE (&check));
  g_assert_cmpint ((gint64) g_value_get_int (&value), ==, g_value_get_int64 (&check));

  g_value_unset (&value);
  g_value_unset (&check);

  if (g_test_verbose ())
    g_print ("Autopromotion of float to double\n");

  g_value_init (&value, G_TYPE_FLOAT);
  g_value_set_float (&value, 3.14159f);

  json_node_set_value (node, &value);
  json_node_get_value (node, &check);

  if (g_test_verbose ())
    g_print ("Expecting a gdouble, got a %s\n", g_type_name (G_VALUE_TYPE (&check))); 

  g_assert_cmpint (G_VALUE_TYPE (&check), ==, G_TYPE_DOUBLE);
  g_assert_cmpfloat ((float) g_value_get_double (&check), ==, 3.14159f);
  g_assert_cmpint (G_VALUE_TYPE (&value), !=, G_VALUE_TYPE (&check));
  g_assert_cmpfloat ((gdouble) g_value_get_float (&value), ==, g_value_get_double (&check));

  g_value_unset (&value);
  g_value_unset (&check);

  json_node_free (node);
}

int
main (int   argc,
      char *argv[])
{
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/nodes/init/int", test_init_int);
  g_test_add_func ("/nodes/init/double", test_init_double);
  g_test_add_func ("/nodes/init/boolean", test_init_boolean);
  g_test_add_func ("/nodes/init/string", test_init_string);
  g_test_add_func ("/nodes/init/null", test_null);
  g_test_add_func ("/nodes/copy/null", test_copy_null);
  g_test_add_func ("/nodes/copy/value", test_copy_value);
  g_test_add_func ("/nodes/copy/object", test_copy_object);
  g_test_add_func ("/nodes/get/int", test_get_int);
  g_test_add_func ("/nodes/get/double", test_get_double);
  g_test_add_func ("/nodes/gvalue", test_gvalue);
  g_test_add_func ("/nodes/gvalue/autopromotion", test_gvalue_autopromotion);

  return g_test_run ();
}
