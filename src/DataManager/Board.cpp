#include "Board.h"
#include "../helpers/trim_spaces/trim_spaces.h"

Board::Board(string name) {
  this->name = name;
  this->columns = {};
}

void Board::add_column(string title) {
  Column column(trim_spaces(title));
  this->columns.push_back(column);
}

void Board::rename_column(size_t column_index, string new_title) {
  this->columns[column_index].title = trim_spaces(new_title);
}

void Board::delete_column(size_t column_index) {
  this->columns.erase(this->columns.begin() + column_index);
}

bool Board::move_column_left(size_t column_index) {
  if (column_index > 0) {
    swap(this->columns[column_index], this->columns[column_index - 1]);
    return true;
  }
  return false;
}

bool Board::move_column_right(size_t column_index) {
  if (column_index < this->columns.size() - 1) {
    swap(this->columns[column_index], this->columns[column_index + 1]);
    return true;
  }
  return false;
}

bool Board::move_card_to_prev_column(size_t card_index, size_t src_column_index,
                                     size_t dist_column_index, Config *config) {
  if (src_column_index > 0) {
    Card card = this->columns[src_column_index].cards[card_index];
    this->columns[src_column_index].delete_card(card_index);
    this->columns[dist_column_index].add_card(
        card, config->move_card_to_column_bottom);

    return true;
  }

  return false;
}

bool Board::move_card_to_next_column(size_t card_index, size_t src_column_index,
                                     size_t dist_column_index, Config *config) {
  if (src_column_index < this->columns.size() - 1) {
    Card card = this->columns[src_column_index].cards[card_index];
    this->columns[src_column_index].delete_card(card_index);
    this->columns[dist_column_index].add_card(
        card, config->move_card_to_column_bottom);

    return true;
  }

  return false;
}
