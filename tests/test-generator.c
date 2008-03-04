#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <json-glib/json-glib.h>

static const gchar *empty_array  = "[ ]";
static const gchar *empty_object = "{ }";

static const gchar *simple_array = "[ true, false, null, 42, \"foo\" ]"; 
static const gchar *nested_array = "[ true, [ false, null ], 42 ]";

static const gchar *simple_object = "{ \"Bool1\" : true, \"Bool2\" : false, \"Null\" : null, \"Int\" : 42, \"String\" : \"foo\" }";

static void
test_empty_array (void)
{
  JsonGenerator *gen = json_generator_new ();
  JsonNode *root;
  gchar *data;
  gsize len;

  root = json_node_new (JSON_NODE_ARRAY);
  json_node_take_array (root, json_array_new ());

  json_generator_set_root (gen, root);
  g_object_set (gen, "pretty", FALSE, NULL);

  data = json_generator_to_data (gen, &len);

  g_assert_cmpint (len, ==, strlen (empty_array));
  g_assert_cmpstr (data, ==, empty_array);

  g_free (data);
  json_node_free (root);
  g_object_unref (gen);
}

static void
test_empty_object (void)
{
  JsonGenerator *gen = json_generator_new ();
  JsonNode *root;
  gchar *data;
  gsize len;

  root = json_node_new (JSON_NODE_OBJECT);
  json_node_take_object (root, json_object_new ());

  json_generator_set_root (gen, root);
  g_object_set (gen, "pretty", FALSE, NULL);

  data = json_generator_to_data (gen, &len);

  g_assert_cmpint (len, ==, strlen (empty_object));
  g_assert_cmpstr (data, ==, empty_object);

  g_free (data);
  json_node_free (root);
  g_object_unref (gen);
}

static void
test_simple_array (void)
{
  JsonGenerator *generator = json_generator_new ();
  JsonNode *root, *val;
  JsonArray *array;
  GValue value = { 0, };
  gchar *data;
  gsize len;

  root = json_node_new (JSON_NODE_ARRAY);
  array = json_array_sized_new (6);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value, TRUE);
  json_node_set_value (val, &value);
  json_array_add_element (array, val);
  g_value_unset (&value);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value, FALSE);
  json_node_set_value (val, &value);
  json_array_add_element (array, val);
  g_value_unset (&value);

  val = json_node_new (JSON_NODE_NULL);
  json_array_add_element (array, val);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 42);
  json_node_set_value (val, &value);
  json_array_add_element (array, val);
  g_value_unset (&value);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string (&value, "foo");
  json_node_set_value (val, &value);
  json_array_add_element (array, val);
  g_value_unset (&value);

  json_node_take_array (root, array);
  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);

  if (g_test_verbose ())
    g_print ("checking simple array `%s' (expected: %s)\n",
             data,
             simple_array);

  g_assert_cmpint (len, ==, strlen (simple_array));
  g_assert_cmpstr (data, ==, simple_array);

  g_free (data);
  json_node_free (root);
  g_object_unref (generator);
}

static void
test_nested_array (void)
{
  JsonGenerator *generator = json_generator_new ();
  JsonNode *root, *val, *nested_val;
  JsonArray *array, *nested;
  GValue value = { 0, };
  gchar *data;
  gsize len;

  root = json_node_new (JSON_NODE_ARRAY);
  array = json_array_sized_new (3);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value, TRUE);
  json_node_set_value (val, &value);
  json_array_add_element (array, val);
  g_value_unset (&value);

  {
    val = json_node_new (JSON_NODE_ARRAY);
    nested = json_array_new ();

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_BOOLEAN);
    g_value_set_boolean (&value, FALSE);
    json_node_set_value (nested_val, &value);
    json_array_add_element (nested, nested_val);
    g_value_unset (&value);

    nested_val = json_node_new (JSON_NODE_NULL);
    json_array_add_element (nested, nested_val);
  
    json_node_take_array (val, nested);
    json_array_add_element (array, val);
  }

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 42);
  json_node_set_value (val, &value);
  json_array_add_element (array, val);
  g_value_unset (&value);

  json_node_take_array (root, array);
  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);

  g_assert_cmpint (len, ==, strlen (nested_array));
  g_assert_cmpstr (data, ==, nested_array);

  g_free (data);
  json_node_free (root);
  g_object_unref (generator);
}

static void
test_simple_object (void)
{
  JsonGenerator *generator = json_generator_new ();
  JsonNode *root, *val;
  JsonObject *object;
  GValue value = { 0, };
  gchar *data;
  gsize len;

  root = json_node_new (JSON_NODE_OBJECT);
  object = json_object_new ();

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value, TRUE);
  json_node_set_value (val, &value);
  json_object_add_member (object, "Bool1", val);
  g_value_unset (&value);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_BOOLEAN);
  g_value_set_boolean (&value, FALSE);
  json_node_set_value (val, &value);
  json_object_add_member (object, "Bool2", val);
  g_value_unset (&value);

  val = json_node_new (JSON_NODE_NULL);
  json_object_add_member (object, "Null", val);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 42);
  json_node_set_value (val, &value);
  json_object_add_member (object, "Int", val);
  g_value_unset (&value);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string (&value, "foo");
  json_node_set_value (val, &value);
  json_object_add_member (object, "String", val);
  g_value_unset (&value);

  json_node_take_object (root, object);
  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);

  if (g_test_verbose ())
    g_print ("checking simple array `%s' (expected: %s)\n",
             data,
             simple_object);

  g_assert_cmpint (len, ==, strlen (simple_object));

  /* we cannot compare the strings literal because JsonObject does not
   * guarantee any ordering
   */

  g_free (data);
  json_node_free (root);
  g_object_unref (generator);
}

#if 0
/* this is just overkill, but I'll add it commented out, so it
 * can be enabled if I feel like running this just to compare
 * the length of the strings
 */
static void
test_nested_object (void)
{
  JsonGenerator *generator = json_generator_new ();
  JsonNode *root, *val, *nested_val;
  JsonObject *object, *nested;
  JsonArray *array;
  GValue value = { 0, };
  gchar *data;
  gsize len;

  root = json_node_new (JSON_NODE_OBJECT);
  object = json_object_new ();

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string (&value, "View from 15th Floor");
  json_node_set_value (val, &value);
  json_object_add_member (object, "Title", val);
  g_value_unset (&value);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 800);
  json_node_set_value (val, &value);
  json_object_add_member (object, "Width", val);
  g_value_unset (&value);

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_INT);
  g_value_set_int (&value, 600);
  json_node_set_value (val, &value);
  json_object_add_member (object, "Height", val);
  g_value_unset (&value);

  {
    val = json_node_new (JSON_NODE_ARRAY);
    array = json_array_new ();

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, 116);
    json_node_set_value (nested_val, &value);
    json_array_add_element (array, nested_val);
    g_value_unset (&value);

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, 943);
    json_node_set_value (nested_val, &value);
    json_array_add_element (array, nested_val);
    g_value_unset (&value);

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, 234);
    json_node_set_value (nested_val, &value);
    json_array_add_element (array, nested_val);
    g_value_unset (&value);

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, 38793);
    json_node_set_value (nested_val, &value);
    json_array_add_element (array, nested_val);
    g_value_unset (&value);

    json_node_take_array (val, array);
    json_object_add_member (object, "IDs", val);
  }

  {
    val = json_node_new (JSON_NODE_OBJECT);
    nested = json_object_new ();

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_STRING);
    g_value_set_string (&value, "http://www.example.com/image/481989943");
    json_node_set_value (nested_val, &value);
    json_object_add_member (nested, "Url", nested_val);
    g_value_unset (&value);

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, 125);
    json_node_set_value (nested_val, &value);
    json_object_add_member (nested, "Width", nested_val);
    g_value_unset (&value);

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, 100);
    json_node_set_value (nested_val, &value);
    json_object_add_member (nested, "Height", nested_val);
    g_value_unset (&value);

    json_node_take_object (val, nested);
    json_object_add_member (object, "Thumbnail", val);
  }

  json_node_take_object (root, object);
  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);

  if (g_test_verbose ())
    g_print ("checking nested object `%s' (expected: %s)\n",
             data,
             nested_object);

  g_assert_cmpint (len, ==, strlen (nested_object));

  /* we cannot compare the strings literal because JsonObject does not
   * guarantee any ordering
   */

  g_free (data);
  json_node_free (root);
  g_object_unref (generator);
}
#endif

int
main (int   argc,
      char *argv[])
{
  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  g_test_add_func ("/json-generator/empty-array", test_empty_array);
  g_test_add_func ("/json-generator/empty-object", test_empty_object);
  g_test_add_func ("/json-generator/simple-array", test_simple_array);
  g_test_add_func ("/json-generator/nested-array", test_nested_array);
  g_test_add_func ("/json-generator/simple-object", test_simple_object);

  return g_test_run ();
}
