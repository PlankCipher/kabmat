#pragma once

#include "Board.h"

class DataManager {
public:
  DataManager();

  void add_board(string name);

  void create_board(string name);
  void rename_board(string old_name, string new_name);
  void delete_board(string name);

  void create_column(Board *board, string title);
  void rename_column(Board *board, size_t column_index, string new_title);
  void delete_column(Board *board, size_t column_index);

  bool move_column_left(Board *board, size_t column_index);
  bool move_column_right(Board *board, size_t column_index);

  bool move_card_up(Column *column, size_t card_index);
  bool move_card_down(Column *column, size_t card_index);

  bool move_card_to_prev_column(Board *board, size_t card_index,
                                size_t src_index, size_t dist_index,
                                Config *config);
  bool move_card_to_next_column(Board *board, size_t card_index,
                                size_t src_index, size_t dist_index,
                                Config *config);

  Board *get_board_if_exists(string name);
  vector<string> get_boards_names();

  vector<Board> boards;

private:
  void write_data_to_file();
};
