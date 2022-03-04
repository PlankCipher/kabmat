#pragma once

#include "../Config/Config.h"
#include "Column.h"

using namespace std;

class Board {
public:
  Board(string name);

  void add_column(string title);
  void rename_column(size_t column_index, string new_title);
  void delete_column(size_t column_index);

  bool move_column_left(size_t column_index);
  bool move_column_right(size_t column_index);

  bool move_card_to_prev_column(size_t card_index, size_t src_column_index,
                                size_t dist_column_index, Config *config);
  bool move_card_to_next_column(size_t card_index, size_t src_column_index,
                                size_t dist_column_index, Config *config);

  string name;
  vector<Column> columns;
};
