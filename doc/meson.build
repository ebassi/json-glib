subdir('xml')

private_headers = [
  'config.h',

  'json-debug.h',
  'json-enum-types.h',
  'json-glib.h',
  'json-gobject-private.h',
  'json-marshal.h',
  'json-private.h',
  'json-scanner.h',
  'json-types-private.h',
]

glib_prefix = dependency('glib-2.0').get_pkgconfig_variable('prefix')
glib_docpath = join_paths(glib_prefix, 'share', 'gtk-doc', 'html')
docpath = join_paths(json_datadir, 'gtk-doc', 'html')

gnome.gtkdoc('json-glib',
             main_xml: 'json-glib-docs.xml',
             src_dir: [ join_paths(meson.source_root(), 'json-glib'), join_paths(meson.build_root(), 'json-glib'), ],
             dependencies: json_glib_dep,
             gobject_typesfile: 'json-glib.types',
             scan_args: [
               '--rebuild-types',
               '--ignore-decorators=_JSON_EXTERN',
               '--ignore-headers=' + ' '.join(private_headers),
             ],
             fixxref_args: [
               '--html-dir=@0@'.format(docpath),
               '--extra-dir=@0@'.format(join_paths(glib_docpath, 'glib')),
               '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gobject')),
               '--extra-dir=@0@'.format(join_paths(glib_docpath, 'gio')),
             ],
             install: true)

xsltproc = find_program('xsltproc', required: false)
if get_option('enable-man') and xsltproc.found()
  xlstproc_flags = [
    '--nonet',
    '--stringparam', 'man.output.quietly', '1',
    '--stringparam', 'funcsynopsis.style', 'ansi',
    '--stringparam', 'man.th.extra1.suppress', '1',
    '--stringparam', 'man.authors.section.enabled', '1',
    '--stringparam', 'man.copyright.section.enabled', '1',
  ]

  man_files = [
    'json-glib-format',
    'json-glib-validate',
  ]

  foreach m: man_files
    custom_target(m + ' man page',
                  input: '@0@.xml'.format(m),
                  output: '@0@.1'.format(m),
                  command: [
                    xsltproc,
                    xlstproc_flags,
                    '-o', '@OUTPUT@',
                    'http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl',
                    '@INPUT@',
                  ],
                  install: true,
                  install_dir: join_paths(json_mandir, 'man1'))
  endforeach
endif
