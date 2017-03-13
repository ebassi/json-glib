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

foreach t: tests
  exe = executable(t, '@0@.c'.format(t),
                   c_args: json_c_args,
                   dependencies: [ json_glib_dep, ])
  test(t, exe)
endforeach
