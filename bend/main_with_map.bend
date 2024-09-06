empty = 0
white = 1
black = 2
board_size = 8

# TODO: Why can't I just do this?
# I get the error: Non-exhaustive pattern matching rule. Default case of number type not covered.
# last_row_index = board_size - 1
# last_position_index = (board_size * board_size) - 1
# white_starting_position = last_position_index - board_size + 1
last_row_index = 7
last_position_index = 63
white_starting_position = 56

# Board Map positions
#    y0 y1 y2 y3 y4 y5 y6 y7
# x0  0  1  2  3  4  5  6  7
# x1  8  9 10 11 12 13 14 15
# x2 16 17 18 19 20 21 22 23
# x3 24 25 26 27 28 29 30 31
# x4 32 33 34 35 36 37 38 38
# x5 40 41 42 43 44 45 46 47
# x6 48 49 50 51 52 53 54 55
# x7 56 57 58 59 60 61 62 63
def get_position_index(x, y):
  return x*board_size + y

def get_initial_position_value(position):
  if position < board_size:
    return black
  elif position >= white_starting_position:
    return white
  else:
    return empty

def get_initial_board():
  bend position = 0, board = {}:
    when position <= last_position_index:
      board[position] = get_initial_position_value(position)
      return fork(position + 1, board)
    else:
      return board

def get_pieces(board, turn):
  bend position = 0, board:
    when position <= last_position_index:
      if (board[position] == turn):
        pieces = List/Cons(position, fork(position + 1, board)) 
      else:
        pieces = fork(position + 1, board)
    else:
      pieces = List/Nil
  return pieces

def valid_position(position):
  if position < 0:
    return 0
  elif position > last_position_index:
    return 0
  else:
    return 1

def get_row(position):
  return to_i24(position / board_size)

def get_left_position(row, position):
  row_start = get_position_index(row, 0)
  left = position - 1
  if left < row_start:
    return -1
  else:
    return left

def get_right_position(row, position):
  row_end = get_position_index(row, last_row_index)
  right = position + 1
  if right > row_end:
    return -1
  else:
    return right

def get_nearby_positions(position):
  top = position - board_size
  top_row = get_row(top)
  top_left = get_left_position(top_row, top)
  top_right = get_right_position(top_row, top)

  row = get_row(position)
  left = get_left_position(row, position)
  right = get_right_position(row, position)
  
  bottom = position + board_size
  bottom_row = get_row(bottom)
  bottom_left = get_left_position(bottom_row, bottom)
  bottom_right = get_right_position(bottom_row, bottom)

  positions = [
    top_left, top, top_right,
    left, right,
    bottom_left, bottom, bottom_right
  ]
  return [p for p in positions if valid_position(p)]

def empty_position(board, position):
  if board[position] == empty:
    return 1
  else:
    return 0

def get_possible_moves_for_piece(board, piece):
  nearby_positions = get_nearby_positions(piece)
  return [p for p in nearby_positions if empty_position(board, p)]

def get_possible_moves(board, turn):
  pieces = get_pieces(board, turn)
  pieces_and_possible_moves = List/Nil

  fold pieces with pieces_and_possible_moves:
    case List/Cons:
      piece = pieces.head
      possible_moves = get_possible_moves_for_piece(board, piece)
      return pieces.tail(List/Cons((piece, possible_moves), pieces_and_possible_moves))
    case List/Nil:
      return pieces_and_possible_moves

def main():
  board = get_initial_board()
  possible_moves = get_possible_moves(board, white)
  return possible_moves
