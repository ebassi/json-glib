#include <stdlib.h>
#include <glib.h>
#include <json-glib/json-glib.h>

static void
test_empty (JsonGenerator *generator)
{
  JsonNode *root;
  gchar *data;
  gsize len;

  root = json_node_new (JSON_NODE_OBJECT);
  json_node_take_object (root, json_object_new ());

  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Empty object (len:%d): `%s'\n", len, data);

  g_free (data);

  json_node_free (root);
}

static void
test_simple (JsonGenerator *generator)
{
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

  val = json_node_new (JSON_NODE_VALUE);
  g_value_init (&value, G_TYPE_FLOAT);
  g_value_set_float (&value, 3.14);
  json_node_set_value (val, &value);
  json_object_add_member (object, "Float", val);
  g_value_unset (&value);

  json_node_take_object (root, object);
  json_generator_set_root (generator, root);

  g_object_set (generator, "pretty", FALSE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Simple object (len:%d): `%s'\n", len, data);
  g_free (data);

  g_object_set (generator, "pretty", TRUE, NULL);
  data = json_generator_to_data (generator, &len);
  g_print ("*** Simple object (pretty, len:%d):\n%s\n", len, data);
  g_free (data);

  json_node_free (root);
}

static void
test_nested (JsonGenerator *generator)
{
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
  g_print ("*** Nested object (len:%d): `%s'\n", len, data);
  g_free (data);

  g_object_set (generator,
                "pretty", TRUE,
                "indent", 1,
                "indent-char", '\t',
                NULL);
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

  g_object_unref (generator);

  return EXIT_SUCCESS;
}
