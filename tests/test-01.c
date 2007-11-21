#include <stdlib.h>
#include <stdio.h>

#include <json-glib/json-glib.h>

static const gchar *test_empty = "";

int
main (int argc, char *argv[])
{
  JsonParser *parser;
  GError *error = NULL;

  g_type_init ();

  parser = json_parser_new ();
  if (!json_parser_load_from_data (parser, test_empty, -1, &error))
    {
      g_print ("Error: %s\n", error->message);
      g_error_free (error);
      g_object_unref (parser);

      return EXIT_FAILURE;
    }

  g_assert (NULL == json_parser_peek_root (parser));

  g_object_unref (parser);

  return EXIT_SUCCESS;
}
