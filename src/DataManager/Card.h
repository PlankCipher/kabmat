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

  string content;
  string description;
  vector<ChecklistItem> checklist;
};
