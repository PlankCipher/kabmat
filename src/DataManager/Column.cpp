#include <algorithm>

#include "Column.h"

Column::Column(string title) {
  this->title = title;
  this->cards = {};
}

void Column::add_card(Card card, bool put_at_bottom) {
  if (put_at_bottom)
    this->cards.push_back(card);
  else
    this->cards.insert(this->cards.begin(), card);
}

void Column::update_card(size_t card_index, Card card) {
  this->cards[card_index] = card;
}

void Column::delete_card(size_t card_index) {
  this->cards.erase(this->cards.begin() + card_index);
}

bool Column::move_card_up(size_t card_index) {
  if (card_index > 0) {
    swap(this->cards[card_index], this->cards[card_index - 1]);
    return true;
  }
  return false;
}

bool Column::move_card_down(size_t card_index) {
  if (card_index < this->cards.size() - 1) {
    swap(this->cards[card_index], this->cards[card_index + 1]);
    return true;
  }
  return false;
}
