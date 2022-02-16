#pragma once

#include "Column.h"

using namespace std;

class Board {
public:
  Board(string name);

  void add_column(string title);

  string name;
  vector<Column> columns;
};
