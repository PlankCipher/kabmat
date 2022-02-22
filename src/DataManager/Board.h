#pragma once

#include "../Config/Config.h"
#include "Column.h"

using namespace std;

class Board {
public:
  Board(string name);

  void add_column(string title);

  bool move_card_to_prev_column(size_t card_index, size_t src_column_index,
                                size_t dist_column_index, Config *config);
  bool move_card_to_next_column(size_t card_index, size_t src_column_index,
                                size_t dist_column_index, Config *config);

  string name;
  vector<Column> columns;
};
