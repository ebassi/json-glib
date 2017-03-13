install_header_subdir = 'json-glib-1.0/json-glib'
install_header_dir = join_paths(json_includedir, install_header_subdir)

configure_file(output: 'config.h', configuration: cdata)

source_h = [
  'json-builder.h',
  'json-generator.h',
  'json-gobject.h',
  'json-gvariant.h',
  'json-parser.h',
  'json-path.h',
  'json-reader.h',
  'json-types.h',
  'json-utils.h',
  'json-version-macros.h'
]

json_glib_enums = gnome.mkenums('json-enum-types',
                                sources: source_h,
                                h_template: 'json-enum-types.h.in',
                                c_template: 'json-enum-types.c.in',
                                install_header: true,
                                install_dir: install_header_dir)

source_c = [
  'json-array.c',
  'json-builder.c',
  'json-debug.c',
  'json-gboxed.c',
  'json-generator.c',
  'json-gobject.c',
  'json-gvariant.c',
  'json-node.c',
  'json-object.c',
  'json-parser.c',
  'json-path.c',
  'json-reader.c',
  'json-scanner.c',
  'json-serializable.c',
  'json-utils.c',
  'json-value.c',
]

version_data = configuration_data()
version_data.set('JSON_MAJOR_VERSION', json_version_major)
version_data.set('JSON_MINOR_VERSION', json_version_minor)
version_data.set('JSON_MICRO_VERSION', json_version_micro)
version_data.set('JSON_VERSION', meson.project_version())

version_h = configure_file(input: 'json-version.h.in',
                           output: 'json-version.h',
                           install_dir: install_header_dir,
                           configuration: version_data)

install_headers(source_h, subdir: install_header_subdir)

localedir = join_paths(get_option('prefix'), get_option('localedir'))
json_c_args = [
  '-DJSON_COMPILATION',
  '-DG_LOG_DOMAIN="Json"',
  '-DJSON_LOCALEDIR="@0@"'.format(localedir)
]
json_lib = library('json-glib-1.0',
                   source_c,
                   version: libversion,
                   soversion: soversion,
                   include_directories: root_dir,
                   dependencies: [ gio_dep, gobject_dep, ],
                   c_args: json_c_args + extra_args,
                   install: true)

pkgg = import('pkgconfig')

pkgg.generate(libraries: [ json_lib ],
              subdirs: '.',
              version: json_version,
              name: 'JSON-GLib',
              filebase: 'json-glib-@0@'.format(apiversion),
              description: 'JSON Parser for GLib.',
              requires: 'glib-2.0 gio-2.0')

json_gen_sources = [ json_glib_enums, version_h ]
if build_gir
    json_gen_sources += gnome.generate_gir(json_lib,
                                           sources: source_c + source_h,
                                           namespace: 'Json',
                                           nsversion: apiversion,
                                           identifier_prefix: 'Json',
                                           symbol_prefix: 'json',
                                           export_packages: 'json-glib-1.0',
                                           includes: [ 'GObject-2.0', 'Gio-2.0', ],
                                           install: true,
                                           extra_args: ['-DJSON_COMPILATION'])
endif

json_glib_dep = declare_dependency(link_with: json_lib,
                                   include_directories: root_dir,
                                   dependencies: [ gobject_dep, gio_dep, ],
                                   sources: json_gen_sources)

tools = [
  [ 'json-glib-validate', [ 'json-glib-validate.c', ] ],
  [ 'json-glib-format', [ 'json-glib-format.c', ] ],
]

foreach t: tools
  bin_name = t[0]
  bin_sources = t[1]

  executable(bin_name, bin_sources, c_args: json_c_args, dependencies: json_glib_dep)
endforeach

subdir('tests')
