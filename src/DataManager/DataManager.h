#pragma once

#include "Board.h"

class DataManager {
public:
  DataManager();

  void add_board(string name);
  void create_board(string name);
  void rename_board(string old_name, string new_name);
  void delete_board(string name);

  bool move_board_up(size_t board_index);
  bool move_board_down(size_t board_index);

  Board *get_board_if_exists(string name);
  vector<string> get_boards_names();

  void create_column(Board *board, string title);
  void rename_column(Board *board, size_t column_index, string new_title);
  void delete_column(Board *board, size_t column_index);

  bool move_column_left(Board *board, size_t column_index);
  bool move_column_right(Board *board, size_t column_index);

  void add_card(Column *column, Card card);
  void update_card(Column *column, size_t card_index, Card card);
  void delete_card(Column *column, size_t card_index);

  bool move_card_up(Column *column, size_t card_index);
  bool move_card_down(Column *column, size_t card_index);

  bool move_card_to_prev_column(Board *board, size_t card_index,
                                size_t src_index, size_t dist_index,
                                Config *config);
  bool move_card_to_next_column(Board *board, size_t card_index,
                                size_t src_index, size_t dist_index,
                                Config *config);

  void add_checklist_item(Card *card, ChecklistItem item);
  void update_checklist_item(Card *card, size_t item_index, ChecklistItem item);
  void delete_checklist_item(Card *card, size_t item_index);

  bool move_checklist_item_up(Card *card, size_t item_index);
  bool move_checklist_item_down(Card *card, size_t item_index);

  vector<Board> boards;

private:
  void write_data_to_file();
};
