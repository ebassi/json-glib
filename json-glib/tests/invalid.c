#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include <glib.h>

#include <json-glib/json-glib.h>

static void
test_invalid_bareword (gconstpointer user_data)
{
  const char *json = user_data;
  GError *error = NULL;
  JsonParser *parser;
  gboolean res;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("invalid data: '%s'...", json);

  res = json_parser_load_from_data (parser, json, -1, &error);

  g_assert (!res);
  g_assert_error (error, JSON_PARSER_ERROR, JSON_PARSER_ERROR_INVALID_BAREWORD);

  if (g_test_verbose ())
    g_print ("expected error: %s\n", error->message);

  g_clear_error (&error);

  g_object_unref (parser);
}

static void
test_invalid_assignment (gconstpointer user_data)
{
  const char *json = user_data;
  GError *error = NULL;
  JsonParser *parser;
  gboolean res;

  parser = json_parser_new ();
  g_assert (JSON_IS_PARSER (parser));

  if (g_test_verbose ())
    g_print ("invalid data: '%s'...", json);

  res = json_parser_load_from_data (parser, json, -1, &error);

  g_assert (!res);
  g_assert (error != NULL);

  if (g_test_verbose ())
    g_print ("expected error: %s\n", error->message);

  g_clear_error (&error);

  g_object_unref (parser);
}

static const struct
{
  const char *path;
  const char *json;
  gpointer func;
} test_invalid[] = {
  /* bareword */
  { "bareword-1", "rainbows", test_invalid_bareword },
  { "bareword-2", "[ unicorns ]", test_invalid_bareword },
  { "bareword-3", "{ \"foo\" : ponies }", test_invalid_bareword },
  { "bareword-4", "[ 3, 2, 1, lift_off ]", test_invalid_bareword },
  { "bareword-5", "{ foo : 42 }", test_invalid_bareword },

  /* assignment */
  { "assignment-1", "var foo", test_invalid_assignment },
  { "assignment-2", "var foo = no", test_invalid_assignment },
  { "assignment-3", "var = true", test_invalid_assignment },
  { "assignment-4", "var blah = 42:", test_invalid_assignment },
  { "assignment-5", "let foo = true;", test_invalid_assignment },

};

static guint n_test_invalid = G_N_ELEMENTS (test_invalid);

#if 0
static const struct
{
  const gchar *str;
  JsonParserError code;
} test_invalid[] = {
  { "test", JSON_PARSER_ERROR_INVALID_BAREWORD },
  { "[ foo, ]", JSON_PARSER_ERROR_INVALID_BAREWORD },
  { "[ true, ]", JSON_PARSER_ERROR_TRAILING_COMMA },
  { "{ \"foo\" : true \"bar\" : false }", JSON_PARSER_ERROR_MISSING_COMMA },
  { "[ true, [ false, ] ]", JSON_PARSER_ERROR_TRAILING_COMMA },
  { "{ \"foo\" : { \"bar\" : false, } }", JSON_PARSER_ERROR_TRAILING_COMMA },
  { "[ { }, { }, { }, ]", JSON_PARSER_ERROR_TRAILING_COMMA },
  { "{ \"foo\" false }", JSON_PARSER_ERROR_MISSING_COLON }
};
#endif

int
main (int   argc,
      char *argv[])
{
  int i;

  g_type_init ();
  g_test_init (&argc, &argv, NULL);

  for (i = 0; i < n_test_invalid; i++)
    {
      char *test_path = g_strconcat ("/invalid/json/", test_invalid[i].path, NULL);

      g_test_add_data_func_full (test_path,
                                 (gpointer) test_invalid[i].json,
                                 test_invalid[i].func,
                                 NULL);

      g_free (test_path);
    }

  return g_test_run ();
}
