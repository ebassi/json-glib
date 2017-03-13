tests = [
  'array',
  'boxed',
  'builder',
  'generator',
  'gvariant',
  'invalid',
  'node',
  'object',
  'parser',
  'path',
  'reader',
  'serialize-simple',
  'serialize-complex',
  'serialize-full',
]

python3 = find_program('python3')
gen_installed_test = join_paths(meson.current_source_dir(), 'gen-installed-test.py')

test_data = [
  'stream-load.json',
]

foreach d: test_data
  copy = 'from shutil import copyfile; copyfile("@0@", "@1@")'.format(d, join_paths(meson.current_build_dir(), d))
  cp = run_command(python3, '-c', copy)
  if cp.returncode() != 0
    error('Could not copy file: ' + cp.stderr())
  endif
endforeach

installed_test_dir = join_paths(json_libexecdir, 'installed-tests', 'json-glib-1.0')

install_data(test_data, install_dir: installed_test_dir)

foreach t: tests
  data = custom_target('@0@.test'.format(t),
                       output: '@0@.test'.format(t),
                       command: [
                         python3,
                         gen_installed_test,
                         '--testdir=@0@'.format(installed_test_dir),
                         '--testname=@0@.test'.format(t),
                         '--outdir=@OUTDIR@',
                         '--outfile=@0@.test'.format(t),
                       ],
                       install: true,
                       install_dir: join_paths(json_datadir, 'installed-tests', 'json-glib-1.0'))

  exe = executable(t, '@0@.c'.format(t),
                   c_args: json_c_args,
                   install: true,
                   install_dir: installed_test_dir,
                   dependencies: [ json_glib_dep, ])

  test(t, exe, args: [ '--tap', '-k' ])
endforeach
