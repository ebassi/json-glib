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

cp = run_command(python3, '-c',
    'from shutil import copyfile; copyfile("@0@", "@1@")'.format(
        'stream-load.json',
        meson.current_build_dir() + '/stream-load.json'))

if cp.returncode() != 0
  error('Could not copy file: ' + cp.stderr())
endif

foreach t: tests
    exe = executable(t, '@0@.c'.format(t),
        c_args: json_c_args,
        dependencies: [json_glib_dep],)
    test(t, exe)
endforeach
