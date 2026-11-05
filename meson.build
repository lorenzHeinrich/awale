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
  include_directories : inc,
)

moves_test = executable(
  'moves-test',
  ['tests/awale/moves_test.c', 'src/awale/moves.c', 'src/awale/board.c'],
  include_directories : inc,
)

awale_test = executable(
  'awale-test',
  [
    'tests/awale/awale_test.c',
    'src/awale/awale.c',
    'src/awale/moves.c',
    'src/awale/board.c',
  ],
  include_directories : inc,
)

test('board', board_test)


foreach case_name : [
  'sow_and_catch',
  'check_sow_ownership_and_stones',
  'check_sow_feeding',
  'capture_single',
  'capture_chain',
  'capture_blockers',
  'capture_stops_at_own_side',
  'capture_lands_on_own_side',
  'grand_slam_chain',
  'sowing_wraparound',
  'sowing_skip_origin',
  'sowing_multiple_laps',
  'sowing_capture_after_skip',
  'rejected_moves',
]
  test('moves/' + case_name, moves_test, args : [case_name])
endforeach

foreach case_name : [
  'player_for_side',
  'move',
  'move_grand_slam',
  'new_game',
  'rejected_moves',
  'finished_game',
  'capture_scores',
  'winning_score',
  'no_feed_winner',
  'no_feed_draw',
  'empty_side_can_be_fed',
]
  test('awale/' + case_name, awale_test, args : [case_name])
endforeach
