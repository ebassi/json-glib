#include <stdlib.h>
#include <glib.h>
#include <json-glib/json-glib.h>

static void
test_empty (JsonGenerator *generator)
{
  JsonNode *root;
  gchar *data;
  gsize len;

  root = json_node_new (JSON_NODE_ARRAY);
  json_node_take_array (root, json_array_new ());

  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Empty array (len:%d): `%s'\n", len, data);

  g_free (data);

  json_node_free (root);
}

static void
test_simple (JsonGenerator *generator)
{
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

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_FLOAT);
  g_value_set_float (&value, 3.14);
  json_node_set_value (val, &value);
  json_array_add_element (array, val);
  g_value_unset (&value);

  json_node_take_array (root, array);
  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Simple array (len:%d): `%s'\n", len, data);
  g_free (data);

  g_object_set (generator, "pretty", TRUE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Simple array (pretty, len:%d):\n%s\n", len, data);
  g_free (data);

  json_node_free (root);
}

static void
test_nested (JsonGenerator *generator)
{
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
  g_print ("*** Nested array (len:%d): `%s'\n", len, data);
  g_free (data);

  g_object_set (generator, "pretty", TRUE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Nested array (pretty, len:%d):\n%s\n", len, data);
  g_free (data);

  json_node_free (root);
}

static void
test_object (JsonGenerator *generator)
{
  JsonNode *root, *val, *nested_val;
  JsonArray *array;
  JsonObject *nested;
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
    val = json_node_new (JSON_NODE_OBJECT);
    nested = json_object_new ();

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_BOOLEAN);
    g_value_set_boolean (&value, FALSE);
    json_node_set_value (nested_val, &value);
    json_object_add_member (nested, "foo", nested_val);
    g_value_unset (&value);

    nested_val = json_node_new (JSON_NODE_NULL);
    json_object_add_member (nested, "bar", nested_val);
  
    json_node_take_object (val, nested);
    json_array_add_element (array, val);
  }

  {
    val = json_node_new (JSON_NODE_OBJECT);
    nested = json_object_new ();

    nested_val = json_node_new (JSON_NODE_VALUE);
    g_value_init (&value, G_TYPE_INT);
    g_value_set_int (&value, 42);
    json_node_set_value (nested_val, &value);
    json_object_add_member (nested, "baz", nested_val);
    g_value_unset (&value);

    json_node_take_object (val, nested);
    json_array_add_element (array, val);
  }

  json_node_take_array (root, array);
  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Nested object (len:%d): `%s'\n", len, data);
  g_free (data);

  g_object_set (generator, "pretty", TRUE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Nested object (pretty, len:%d):\n%s\n", len, data);
  g_free (data);

  json_node_free (root);
}

int
main (int argc, char *argv[])
{
  JsonGenerator *generator;

  g_type_init ();

  generator = json_generator_new ();
  
  test_empty (generator);
  test_simple (generator);
  test_nested (generator);
  test_object (generator);

  g_object_unref (generator);

  return EXIT_SUCCESS;
}
