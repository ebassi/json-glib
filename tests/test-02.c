#include <stdlib.h>
#include <stdio.h>

#include <json-glib/json-glib.h>

static const gchar *test_arrays[] = {
  "[ ]",
  "[ true ]",
  "[ true, false, null ]",
  "[ 1, 2, 3.14, \"test\" ]",
  "[ 42, [ ], null ]",
  "[ [ ], [ true, [ true ] ] ]",
  "[ [ false, true, 42 ], [ true, false, 3.14 ], \"test\" ]"
};

static guint n_test_arrays = G_N_ELEMENTS (test_arrays);

int
main (int argc, char *argv[])
{
  JsonParser *parser;
  gint i;
  GList *l;

  g_type_init ();

  parser = json_parser_new ();

  for (i = 0; i < n_test_arrays; i++)
    {
      GError *error = NULL;

      if (!json_parser_load_from_data (parser, test_arrays[i], -1, &error))
        {
          g_print ("* Error, test %d:\n"
                   "* \t%s:\n"
                   "* Message: %s\n",
                   i, test_arrays[i], error->message);
          g_error_free (error);
          g_object_unref (parser);
          return EXIT_FAILURE;
        }
    }

  l = json_parser_get_toplevels (parser);
  g_assert (l != NULL);
  g_assert (g_list_length (l) == n_test_arrays);

  g_object_unref (parser);

  return EXIT_SUCCESS;
}
