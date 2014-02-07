/* json-glib-validate - Checks JSON data for errors
 * 
 * This file is part of JSON-GLib
 *
 * Copyright © 2013  Emmanuele Bassi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author:
 *   Emmanuele Bassi  <ebassi@gnome.org>
 */

#include "config.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <errno.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <json-glib/json-glib.h>

static char **files = NULL;

static GOptionEntry entries[] = {
  { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &files, NULL, NULL },
  { NULL },
};

static gboolean
validate (JsonParser *parser,
          GFile      *file)
{
  GInputStream *in;
  GError *error;
  gboolean res = TRUE;
  gboolean parse_res;
  gboolean close_res;

  error = NULL;

  in = (GInputStream *) g_file_read (file, NULL, &error);
  if (in == NULL)
    {
      /* Translators: the first %s is the program name, the second one
       * is the URI of the file, the third is the error message.
       */
      g_printerr (_("%s: %s: error opening file: %s\n"),
                  g_get_prgname (), g_file_get_uri (file), error->message);
      g_error_free (error);
      return FALSE;
    }

  parse_res = json_parser_load_from_stream (parser, in, NULL, &error);
  if (!parse_res)
    {
      /* Translators: the first %s is the program name, the second one
       * is the URI of the file, the third is the error message.
       */
      g_printerr (_("%s: %s: error parsing file: %s\n"),
                  g_get_prgname (), g_file_get_uri (file), error->message);
      g_clear_error (&error);
      res = FALSE;
    }

  close_res = g_input_stream_close (in, NULL, &error);
  if (!close_res)
    {
      /* Translators: the first %s is the program name, the second one
       * is the URI of the file, the third is the error message.
       */
      g_printerr (_("%s: %s: error closing: %s\n"),
                  g_get_prgname (), g_file_get_uri (file), error->message);
      g_clear_error (&error);
      res = FALSE;
    }

  return res;
}

int
main (int   argc,
      char *argv[])
{
  GOptionContext *context = NULL;
  GError *error = NULL;
  const char *description;
  const char *summary;
  gchar *param;
  JsonParser *parser;
  gboolean res;
  int i;

  setlocale (LC_ALL, "");

  bindtextdomain (GETTEXT_PACKAGE, JSON_LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  param = g_strdup_printf (("%s..."), _("FILE"));
  /* Translators: this message will appear after the usage string */
  /* and before the list of options.                              */
  summary = _("Validate JSON files.");
  description = _("json-glib-validate validates JSON data at the given URI.");

  context = g_option_context_new (param);
  g_option_context_set_summary (context, summary);
  g_option_context_set_description (context, description);
  g_option_context_add_main_entries (context, entries, GETTEXT_PACKAGE);
  g_option_context_parse (context, &argc, &argv, &error);
  g_option_context_free (context);

  g_free (param);

  if (error != NULL)
    {
      /* Translators: the %s is the program name. This error message
       * means the user is calling json-glib-validate without any
       * argument.
       */
      g_printerr (_("Error parsing commandline options: %s\n"), error->message);
      g_printerr ("\n");
      g_printerr (_("Try \"%s --help\" for more information."), g_get_prgname ());
      g_printerr ("\n");
      g_error_free (error);
      return 1;
    }

  if (files == NULL)
    {
      /* Translators: the %s is the program name. This error message
       * means the user is calling json-glib-validate without any
       * argument.
       */
      g_printerr (_("%s: missing files"), g_get_prgname ());
      g_printerr ("\n");
      g_printerr (_("Try \"%s --help\" for more information."), g_get_prgname ());
      g_printerr ("\n");
      return 1;
    }

  parser = json_parser_new ();
  res = TRUE;
  i = 0;

  do
    {
      GFile *file = g_file_new_for_commandline_arg (files[i]);

      res = validate (parser, file) && res;
      g_object_unref (file);
    }
  while (files[++i] != NULL);

  g_object_unref (parser);

  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}
