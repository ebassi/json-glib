project('json-glib', 'c',
  version: '1.2.3',
  meson_version: '>= 0.37.1',
  default_options: [ 'warning_level=1',
                      'buildtype=debugoptimized' ])


# Versionning
json_version = meson.project_version()
version_arr = json_version.split('.')
json_version_major = version_arr[0]
json_version_minor = version_arr[1]
json_version_micro = version_arr[2]

apiversion = '1.0'
soversion = 0

if json_version_micro.to_int().is_odd()
  json_interface_age = 0
else
  json_interface_age = json_version_micro
endif

# maintaining compatibility with the previous libtool versioning
# current = minor * 100 + micro - interface
# revision = interface
soversion = 0
current = json_version_minor.to_int() * 100 + json_version_micro.to_int() - json_interface_age
revision = json_interface_age
libversion = '@0@.@1@.@2@'.format(soversion, current, revision)

# Dependencies
glib_req_version = '>= 2.37.6'
gobject_dep = dependency('gobject-2.0', version: glib_req_version)
gio_dep = dependency('gio-2.0', version: glib_req_version)

# Configurations
cc = meson.get_compiler('c')
extra_args = []

cdata = configuration_data()
check_headers = [
  ['unistd.h', 'HAVE_UNISTD_H'],
]

foreach h: check_headers
  if cc.has_header(h.get(0))
    cdata.set(h.get(1), 1)
  endif
endforeach
cdata.set_quoted('GETTEXT_PACKAGE', 'json-glib-1.0')

if get_option('default_library') != 'static'
  if host_machine.system() == 'windows'
    cdata.set('DLL_EXPORT', true)
    if cc.get_id() == 'msvc'
      cdata.set('_JSON_EXTERN', '__declspec(dllexport) extern')
    else
      cdata.set('_JSON_EXTERN', '__attribute__((visibility("default"))) __declspec(dllexport) extern')
      extra_args += ['-fvisibility=hidden']
    endif
  else
    cdata.set('_JSON_EXTERN', '__attribute__((visibility("default")))')
    extra_args += ['-fvisibility=hidden']
  endif
endif

configure_file(output: 'config.h', configuration: cdata)

root_dir = include_directories('.')
gnome = import('gnome')
gir = find_program('g-ir-scanner', required: false)
build_gir = gir.found() and not meson.is_cross_build() and not get_option('disable_introspection')

subdir('json-glib')
