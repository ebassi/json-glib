#include <string.h>
#include <glib.h>
#include <json-glib/json-glib.h>

static const char *test_json =
"{ \"store\": {"
"    \"book\": [ "
"      { \"category\": \"reference\","
"        \"author\": \"Nigel Rees\","
"        \"title\": \"Sayings of the Century\","
"        \"price\": \"8.95\""
"      },"
"      { \"category\": \"fiction\","
"        \"author\": \"Evelyn Waugh\","
"        \"title\": \"Sword of Honour\","
"        \"price\": \"12.99\""
"      },"
"      { \"category\": \"fiction\","
"        \"author\": \"Herman Melville\","
"        \"title\": \"Moby Dick\","
"        \"isbn\": \"0-553-21311-3\","
"        \"price\": \"8.99\""
"      },"
"      { \"category\": \"fiction\","
"        \"author\": \"J. R. R. Tolkien\","
"        \"title\": \"The Lord of the Rings\","
"        \"isbn\": \"0-395-19395-8\","
"        \"price\": \"22.99\""
"      }"
"    ],"
"    \"bicycle\": {"
"      \"color\": \"red\","
"      \"price\": \"19.95\""
"    }"
"  }"
"}";

static const struct {
  const char *desc;
  const char *expr;
  const char *res;

  guint is_valid : 1;

  JsonPathError error_code;
} test_expressions[] = {
  {
    "INVALID: invalid first character",
    "/",
    NULL,
    FALSE,
    JSON_PATH_ERROR_INVALID_QUERY,
  },
  {
    "INVALID: Invalid character following root",
    "$ponies",
    NULL,
    FALSE,
    JSON_PATH_ERROR_INVALID_QUERY,
  },
  {
    "INVALID: missing member name or wildcard after dot",
    "$.store.",
    NULL,
    FALSE,
    JSON_PATH_ERROR_INVALID_QUERY,
  },
  {
    "INVALID: Malformed slice (missing step)",
    "$.store.book[0:1:]",
    NULL,
    FALSE,
    JSON_PATH_ERROR_INVALID_QUERY,
  },
  {
    "INVALID: Malformed set",
    "$.store.book[0,1~2]",
    NULL,
    FALSE,
    JSON_PATH_ERROR_INVALID_QUERY,
  },
  {
    "INVALID: Malformed array notation",
    "${'store'}",
    NULL,
    FALSE,
    JSON_PATH_ERROR_INVALID_QUERY,
  },
  {
    "INVALID: Malformed slice (invalid separator)",
    "$.store.book[0~2]",
    NULL,
    FALSE,
    JSON_PATH_ERROR_INVALID_QUERY,
  },
  {
    "Title of the first book in the store, using objct notation.",
    "$.store.book[0].title",
    "[\"Sayings of the Century\"]",
    TRUE,
  },
  {
    "Title of the first book in the store, using array notation.",
    "$['store']['book'][0]['title']",
    "[\"Sayings of the Century\"]",
    TRUE,
  },
  {
    "All the authors from the every book.",
    "$.store.book[*].author",
    "[\"Nigel Rees\",\"Evelyn Waugh\",\"Herman Melville\",\"J. R. R. Tolkien\"]",
    TRUE,
  },
  {
    "All the authors.",
    "$..author",
    "[\"Nigel Rees\",\"Evelyn Waugh\",\"Herman Melville\",\"J. R. R. Tolkien\"]",
    TRUE,
  },
  {
    "Everything inside the store.",
    "$.store.*",
    NULL,
    TRUE,
  },
  {
    "All the prices in the store.",
    "$.store..price",
    "[\"8.95\",\"12.99\",\"8.99\",\"22.99\",\"19.95\"]",
    TRUE,
  },
  {
    "The third book.",
    "$..book[2]",
    "[{\"category\":\"fiction\",\"author\":\"Herman Melville\",\"title\":\"Moby Dick\",\"isbn\":\"0-553-21311-3\",\"price\":\"8.99\"}]",
    TRUE,
  },
  {
    "The last book.",
    "$..book[-1:]",
    "[{\"category\":\"fiction\",\"author\":\"J. R. R. Tolkien\",\"title\":\"The Lord of the Rings\",\"isbn\":\"0-395-19395-8\",\"price\":\"22.99\"}]",
    TRUE,
  },
  {
    "The first two books.",
    "$..book[0,1]",
    "[{\"category\":\"reference\",\"author\":\"Nigel Rees\",\"title\":\"Sayings of the Century\",\"price\":\"8.95\"},{\"category\":\"fiction\",\"author\":\"Evelyn Waugh\",\"title\":\"Sword of Honour\",\"price\":\"12.99\"}]",
    TRUE,
  },
  {
    "The first two books, using a slice.",
    "$..book[:2]",
    "[{\"category\":\"reference\",\"author\":\"Nigel Rees\",\"title\":\"Sayings of the Century\",\"price\":\"8.95\"},{\"category\":\"fiction\",\"author\":\"Evelyn Waugh\",\"title\":\"Sword of Honour\",\"price\":\"12.99\"}]",
    TRUE,
  },
  {
    "All the books.",
    "$['store']['book'][*]",
    "[{\"category\":\"reference\",\"author\":\"Nigel Rees\",\"title\":\"Sayings of the Century\",\"price\":\"8.95\"},{\"category\":\"fiction\",\"author\":\"Evelyn Waugh\",\"title\":\"Sword of Honour\",\"price\":\"12.99\"},{\"category\":\"fiction\",\"author\":\"Herman Melville\",\"title\":\"Moby Dick\",\"isbn\":\"0-553-21311-3\",\"price\":\"8.99\"},{\"category\":\"fiction\",\"author\":\"J. R. R. Tolkien\",\"title\":\"The Lord of the Rings\",\"isbn\":\"0-395-19395-8\",\"price\":\"22.99\"}]",
    TRUE,
  },
  {
    "All the members of the bicycle object.",
    "$.store.bicycle.*",
    "[\"red\",\"19.95\"]",
    TRUE,
  },
  {
    "The root node.",
    "$",
    "[{\"store\":{\"book\":[{\"category\":\"reference\",\"author\":\"Nigel Rees\",\"title\":\"Sayings of the Century\",\"price\":\"8.95\"},"
                           "{\"category\":\"fiction\",\"author\":\"Evelyn Waugh\",\"title\":\"Sword of Honour\",\"price\":\"12.99\"},"
                           "{\"category\":\"fiction\",\"author\":\"Herman Melville\",\"title\":\"Moby Dick\",\"isbn\":\"0-553-21311-3\",\"price\":\"8.99\"},"
                           "{\"category\":\"fiction\",\"author\":\"J. R. R. Tolkien\",\"title\":\"The Lord of the Rings\",\"isbn\":\"0-395-19395-8\",\"price\":\"22.99\"}],"
                 "\"bicycle\":{\"color\":\"red\",\"price\":\"19.95\"}}}]",
    TRUE,
  }
};

static void
path_expressions_valid (gconstpointer data)
{
  const int index_ = GPOINTER_TO_INT (data);
  const char *expr = test_expressions[index_].expr;
  const char *desc = test_expressions[index_].desc;

  JsonPath *path = json_path_new ();
  GError *error = NULL;

  if (g_test_verbose ())
    g_print ("* %s ('%s')\n", desc, expr);

  g_assert (json_path_compile (path, expr, &error));
  g_assert_no_error (error);

  g_object_unref (path);
}

static void
path_expressions_invalid (gconstpointer data)
{
  const int index_ = GPOINTER_TO_INT (data);
  const char *expr = test_expressions[index_].expr;
  const char *desc = test_expressions[index_].desc;
  const JsonPathError code = test_expressions[index_].error_code;

  JsonPath *path = json_path_new ();
  GError *error = NULL;

  if (g_test_verbose ())
    g_print ("* %s ('%s')\n", desc, expr);


  g_assert (!json_path_compile (path, expr, &error));
  g_assert_error (error, JSON_PATH_ERROR, code);

  g_object_unref (path);
}

static void
path_match (gconstpointer data)
{
  const int index_ = GPOINTER_TO_INT (data);
  const char *desc = test_expressions[index_].desc;
  const char *expr = test_expressions[index_].expr;
  const char *res  = test_expressions[index_].res;

  JsonParser *parser = json_parser_new ();
  JsonGenerator *gen = json_generator_new ();
  JsonPath *path = json_path_new ();
  JsonNode *root;
  JsonNode *matches;
  char *str;

  json_parser_load_from_data (parser, test_json, -1, NULL);
  root = json_parser_get_root (parser);

  g_assert (json_path_compile (path, expr, NULL));

  matches = json_path_match (path, root);
  g_assert (JSON_NODE_HOLDS_ARRAY (matches));

  json_generator_set_root (gen, matches);
  str = json_generator_to_data (gen, NULL);

  if (g_test_verbose ())
    {
      g_print ("* %s ('%s') =>\n"
               "- result:   %s\n"
               "- expected: %s\n",
               desc, expr,
               str,
               res);
    }

  g_assert_cmpstr (str, ==, res);

  g_free (str);
  json_node_free (matches);

  g_object_unref (parser);
  g_object_unref (path);
  g_object_unref (gen);
}

int
main (int   argc,
      char *argv[])
{
  int i, j;

  g_test_init (&argc, &argv, NULL);
  g_test_bug_base ("http://bugzilla.gnome.org/show_bug.cgi?id=");

  for (i = 0, j = 1; i < G_N_ELEMENTS (test_expressions); i++)
    {
      char *path;

      if (!test_expressions[i].is_valid)
        continue;

      path = g_strdup_printf ("/path/expressions/valid/%d", j++);

      g_test_add_data_func (path, GINT_TO_POINTER (i), path_expressions_valid);

      g_free (path);
    }

  for (i = 0, j = 1; i < G_N_ELEMENTS (test_expressions); i++)
    {
      char *path;

      if (test_expressions[i].is_valid)
        continue;

      path = g_strdup_printf ("/path/expressions/invalid/%d", j++);

      g_test_add_data_func (path, GINT_TO_POINTER (i), path_expressions_invalid);

      g_free (path);
    }

  for (i = 0, j = 1; i < G_N_ELEMENTS (test_expressions); i++)
    {
      char *path;

      if (!test_expressions[i].is_valid || test_expressions[i].res == NULL)
        continue;

      path = g_strdup_printf ("/path/match/%d", j++);

      g_test_add_data_func (path, GINT_TO_POINTER (i), path_match);

      g_free (path);
    }

  return g_test_run ();
}
