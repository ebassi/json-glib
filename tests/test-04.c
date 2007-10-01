#include <stdlib.h>
#include <glib.h>
#include <json-glib/json-glib.h>

int
main (int argc, char *argv[])
{
  JsonGenerator *generator;
  JsonNode *root;
  gchar *data;
  gsize len;

  g_type_init ();

  generator = json_generator_new ();

  root = json_node_new (JSON_NODE_ARRAY);
  json_node_take_array (root, json_array_new ());

  json_generator_set_root (generator, root);

  data = json_generator_to_data (generator, &len);
  g_print ("*** Empty array (len:%d): `%s'\n", len, data);

  g_free (data);
  g_object_unref (generator);

  return EXIT_SUCCESS;
}
