#include "Board.h"
#include "../helpers/remove_trim_spaces/remove_trim_spaces.h"

Board::Board(string name) {
  this->name = name;
  this->columns = {};
}

void Board::add_column(string title) {
  Column column(remove_trim_spaces(title));
  this->columns.push_back(column);
}
