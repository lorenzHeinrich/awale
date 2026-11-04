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

inc = include_directories('src')

board_test = executable(
  'board-test',
  ['tests/board_test.c', 'src/board.c'],
  include_directories: inc,
)

test('test', board_test)
