project(
  'awale',
  'c',
  meson_version : '>= 1.3.0',
  version : '0.1',
  default_options : ['warning_level=3'],
)

dependencies = [
  dependency('readline'),
]

sources = files(
  'src/awale/awale.c',
  'src/awale/board.c',
  'src/awale/moves.c',
  'src/main.c',
)


exe = executable(
  'awale',
  sources,
  dependencies : dependencies,
  install : true,
)

inc = include_directories('src')

board_test = executable(
  'board-test',
  ['tests/awale/board_test.c', 'src/awale/board.c'],
  include_directories: inc,
)

test('test', board_test)
