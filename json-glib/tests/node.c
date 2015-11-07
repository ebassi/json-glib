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

/* Test that creating then sealing a node containing an int causes it to be
 * immutable. */
static void
test_seal_int (void)
{
  JsonNode *node = NULL;

  node = json_node_init_int (json_node_alloc (), 1);

  g_assert_false (json_node_is_immutable (node));
  json_node_seal (node);
  g_assert_true (json_node_is_immutable (node));
  json_node_free (node);
}

/* Test that creating then sealing a node containing a double causes it to be
 * immutable. */
static void
test_seal_double (void)
{
  JsonNode *node = NULL;

  node = json_node_init_double (json_node_alloc (), 15.2);
  g_assert_false (json_node_is_immutable (node));
  json_node_seal (node);
  g_assert_true (json_node_is_immutable (node));
  json_node_free (node);
}

/* Test that creating then sealing a node containing a boolean causes it to be
 * immutable. */
static void
test_seal_boolean (void)
{
  JsonNode *node = NULL;

  node = json_node_init_boolean (json_node_alloc (), TRUE);
  g_assert_false (json_node_is_immutable (node));
  json_node_seal (node);
  g_assert_true (json_node_is_immutable (node));
  json_node_free (node);
}

/* Test that creating then sealing a node containing a string causes it to be
 * immutable. */
static void
test_seal_string (void)
{
  JsonNode *node = NULL;

  node = json_node_init_string (json_node_alloc (), "hi there");
  g_assert_false (json_node_is_immutable (node));
  json_node_seal (node);
  g_assert_true (json_node_is_immutable (node));
  json_node_free (node);
}

/* Test that creating then sealing a node containing a null causes it to be
 * immutable. */
static void
test_seal_null (void)
{
  JsonNode *node = NULL;

  node = json_node_init_null (json_node_alloc ());
  g_assert_false (json_node_is_immutable (node));
  json_node_seal (node);
  g_assert_true (json_node_is_immutable (node));
  json_node_free (node);
}

/* Test that creating then sealing a node containing an object causes it to be
 * immutable. */
static void
test_seal_object (void)
{
  JsonNode *node = NULL;
  JsonObject *object = NULL;

  object = json_object_new ();
  node = json_node_init_object (json_node_alloc (), object);

  g_assert_false (json_object_is_immutable (object));
  g_assert_false (json_node_is_immutable (node));
  json_node_seal (node);
  g_assert_true (json_node_is_immutable (node));
  g_assert_true (json_object_is_immutable (object));

  json_node_free (node);
  json_object_unref (object);
}

/* Test that creating then sealing a node containing an array causes it to be
 * immutable. */
static void
test_seal_array (void)
{
  JsonNode *node = NULL;
  JsonArray *array = NULL;

  array = json_array_new ();
  node = json_node_init_array (json_node_alloc (), array);

  g_assert_false (json_array_is_immutable (array));
  g_assert_false (json_node_is_immutable (node));
  json_node_seal (node);
  g_assert_true (json_node_is_immutable (node));
  g_assert_true (json_array_is_immutable (array));

  json_node_free (node);
  json_array_unref (array);
}

/* Test that an immutable node containing an int cannot be modified. */
static void
test_immutable_int (void)
{
  if (g_test_subprocess ())
    {
      JsonNode *node = NULL;

      node = json_node_init_int (json_node_alloc (), 5);
      json_node_seal (node);

      /* Boom. */
      json_node_set_int (node, 1);
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Json-CRITICAL **: json_node_set_int: "
                             "assertion '!node->immutable' failed*");
}

/* Test that an immutable node containing a double cannot be modified. */
static void
test_immutable_double (void)
{
  if (g_test_subprocess ())
    {
      JsonNode *node = NULL;

      node = json_node_init_double (json_node_alloc (), 5.6);
      json_node_seal (node);

      /* Boom. */
      json_node_set_double (node, 1.1);
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Json-CRITICAL **: json_node_set_double: "
                             "assertion '!node->immutable' failed*");
}

/* Test that an immutable node containing a boolean cannot be modified. */
static void
test_immutable_boolean (void)
{
  if (g_test_subprocess ())
    {
      JsonNode *node = NULL;

      node = json_node_init_boolean (json_node_alloc (), TRUE);
      json_node_seal (node);

      /* Boom. */
      json_node_set_boolean (node, FALSE);
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Json-CRITICAL **: json_node_set_boolean: "
                             "assertion '!node->immutable' failed*");
}

/* Test that an immutable node containing a string cannot be modified. */
static void
test_immutable_string (void)
{
  if (g_test_subprocess ())
    {
      JsonNode *node = NULL;

      node = json_node_init_string (json_node_alloc (), "bonghits");
      json_node_seal (node);

      /* Boom. */
      json_node_set_string (node, "asdasd");
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Json-CRITICAL **: json_node_set_string: "
                             "assertion '!node->immutable' failed*");
}

/* Test that an immutable node containing an object cannot be modified. */
static void
test_immutable_object (void)
{
  if (g_test_subprocess ())
    {
      JsonNode *node = NULL;

      node = json_node_init_object (json_node_alloc (), json_object_new ());
      json_node_seal (node);

      /* Boom. */
      json_node_set_object (node, json_object_new ());
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Json-CRITICAL **: json_node_set_object: "
                             "assertion '!node->immutable' failed*");
}

/* Test that an immutable node containing an array cannot be modified. */
static void
test_immutable_array (void)
{
  if (g_test_subprocess ())
    {
      JsonNode *node = NULL;

      node = json_node_init_array (json_node_alloc (), json_array_new ());
      json_node_seal (node);

      /* Boom. */
      json_node_set_array (node, json_array_new ());
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Json-CRITICAL **: json_node_set_array: "
                             "assertion '!node->immutable' failed*");
}

/* Test that an immutable node containing a value cannot be modified. */
static void
test_immutable_value (void)
{
  if (g_test_subprocess ())
    {
      JsonNode *node = NULL;
      GValue val = G_VALUE_INIT;

      node = json_node_init_int (json_node_alloc (), 5);
      json_node_seal (node);

      /* Boom. */
      g_value_init (&val, G_TYPE_INT);
      g_value_set_int (&val, 50);
      json_node_set_value (node, &val);
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Json-CRITICAL **: json_node_set_value: "
                             "assertion '!node->immutable' failed*");
}

/* Test that an immutable node can be reparented but not to an immutable
 * parent. */
static void
test_immutable_parent (void)
{
  if (g_test_subprocess ())
    {
      JsonNode *node = NULL;
      JsonNode *parent_mutable = NULL;
      JsonNode *parent_immutable = NULL;
      JsonObject *object_mutable = NULL;
      JsonObject *object_immutable = NULL;

      node = json_node_init_int (json_node_alloc (), 5);
      json_node_seal (node);

      object_mutable = json_object_new ();
      object_immutable = json_object_new ();

      parent_mutable = json_node_init_object (json_node_alloc (),
                                              object_mutable);
      parent_immutable = json_node_init_object (json_node_alloc (),
                                                object_immutable);

      json_node_seal (parent_immutable);

      /* Can we reparent the immutable node? */
      json_object_set_member (object_mutable, "test", node);
      json_node_set_parent (node, parent_mutable);

      json_object_remove_member (object_mutable, "test");
      json_node_set_parent (node, NULL);

      /* Boom. */
      json_node_set_parent (node, parent_immutable);
    }

  g_test_trap_subprocess (NULL, 0, 0);
  g_test_trap_assert_failed ();
  g_test_trap_assert_stderr ("*Json-CRITICAL **: json_node_set_parent: *");
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
  g_test_add_func ("/nodes/seal/int", test_seal_int);
  g_test_add_func ("/nodes/seal/double", test_seal_double);
  g_test_add_func ("/nodes/seal/boolean", test_seal_boolean);
  g_test_add_func ("/nodes/seal/string", test_seal_string);
  g_test_add_func ("/nodes/seal/null", test_seal_null);
  g_test_add_func ("/nodes/seal/object", test_seal_object);
  g_test_add_func ("/nodes/seal/array", test_seal_array);
  g_test_add_func ("/nodes/immutable/int", test_immutable_int);
  g_test_add_func ("/nodes/immutable/double", test_immutable_double);
  g_test_add_func ("/nodes/immutable/boolean", test_immutable_boolean);
  g_test_add_func ("/nodes/immutable/string", test_immutable_string);
  g_test_add_func ("/nodes/immutable/object", test_immutable_object);
  g_test_add_func ("/nodes/immutable/array", test_immutable_array);
  g_test_add_func ("/nodes/immutable/value", test_immutable_value);
  g_test_add_func ("/nodes/immutable/parent", test_immutable_parent);

  return g_test_run ();
}
