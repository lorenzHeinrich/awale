project(
  'awale',
  'c',
  meson_version: '>= 1.3.0',
  version: '0.1',
  default_options: ['warning_level=3'],
)

dependencies = [
  dependency('readline'),
]

add_project_arguments(
  '-DLOG_LEVEL=LOG_LEVEL_' + get_option('log_level'),
  '-ffile-prefix-map=../=',
  language: 'c',
)

common = files(
  'src/socket.c',
  'src/utils.c',
)

server = files(
  'src/awale/awale.c',
  'src/awale/board.c',
  'src/awale/moves.c',
  'src/awale_server.c',
  'src/commands/arg_parser.c',
  'src/commands/command.c',
  'src/commands/leave_command.c',
  'src/commands/move_command.c',
  'src/commands/register_command.c',
)

executable('awale_server', common, server)
executable('awale_client', common, 'src/awale_client.c', dependencies: dependencies)

#executable(
#'awale_cli',
#common,
#'src/awale_cli.c',
#dependencies : dependencies,
#)

inc = include_directories('src')

board_test = executable(
  'board-test',
  ['tests/awale/board_test.c', 'src/awale/board.c'],
  include_directories: inc,
)

moves_test = executable(
  'moves-test',
  ['tests/awale/moves_test.c', 'src/awale/moves.c', 'src/awale/board.c'],
  include_directories: inc,
)

awale_test = executable(
  'awale-test',
  [
    'tests/awale/awale_test.c',
    'src/awale/awale.c',
    'src/awale/moves.c',
    'src/awale/board.c',
  ],
  include_directories: inc,
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
  test('moves/' + case_name, moves_test, args: [case_name])
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
  test('awale/' + case_name, awale_test, args: [case_name])
endforeach

command_test = executable(
  'command-test',
  [
    'tests/command_test.c',
    'src/commands/command.c',
    'src/commands/arg_parser.c',
    'src/utils.c',
  ],
  include_directories: inc,
)

foreach case_name : [
  'parse_cmd',
  'parse_args',
  'parse_cmd_unknown',
  'parse_cmd_wrong_argc',
  'parse_cmd_too_many_tokens',
  'parse_cmd_empty',
  'parse_args_non_int',
  'parse_args_int_out_of_range',
  'parse_args_empty_str',
]
  test('command/' + case_name, command_test, args: [case_name])
endforeach
