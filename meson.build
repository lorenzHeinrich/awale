project(
  'awale',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
]

sources = [
  'src/awale.c',
]

exe = executable(
  'awale',
  sources,
  dependencies : dependencies,
  install : true,
)

test('basic', exe)
