#include "Card.h"

#include "../helpers/trim_spaces/trim_spaces.h"

Card::Card(string content) {
  this->content = trim_spaces(content);
  this->description = "";
  this->checklist = {};
}

void Card::add_checklist_item(ChecklistItem item) {
  this->checklist.push_back(item);
}

void Card::update_checklist_item(size_t item_index, ChecklistItem item) {
  this->checklist[item_index] = item;
}

void Card::delete_checklist_item(size_t item_index) {
  this->checklist.erase(this->checklist.begin() + item_index);
}

bool Card::move_checklist_item_up(size_t item_index) {
  if (item_index > 0) {
    swap(this->checklist[item_index], this->checklist[item_index - 1]);
    return true;
  }
  return false;
}

bool Card::move_checklist_item_down(size_t item_index) {
  if (item_index < this->checklist.size() - 1) {
    swap(this->checklist[item_index], this->checklist[item_index + 1]);
    return true;
  }
  return false;
}
