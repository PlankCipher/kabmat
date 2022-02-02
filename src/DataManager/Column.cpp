#include "Column.h"
#include "../helpers/remove_trim_spaces/remove_trim_spaces.h"

Column::Column(string title) {
  this->title = title;
  this->cards = {};
}

void Column::add_card(string content) {
  this->cards.push_back(remove_trim_spaces(content));
}
