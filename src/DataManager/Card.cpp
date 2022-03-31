#include "Card.h"

#include "../helpers/remove_trim_spaces/remove_trim_spaces.h"

Card::Card(string content) {
  this->content = remove_trim_spaces(content);
  this->description = "";
  this->checklist = {};
}

void Card::add_checklist_item(ChecklistItem item) {
  this->checklist.push_back(item);
}
