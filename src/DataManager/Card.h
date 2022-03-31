#pragma once

#include <string>
#include <vector>

using namespace std;

struct ChecklistItem {
  string content;
  bool done = false;
};

class Card {
public:
  Card(string content);

  void add_checklist_item(ChecklistItem item);
  void update_checklist_item(size_t item_index, ChecklistItem item);
  void delete_checklist_item(size_t item_index);

  bool move_checklist_item_up(size_t item_index);
  bool move_checklist_item_down(size_t item_index);

  string content;
  string description;
  vector<ChecklistItem> checklist;
};
