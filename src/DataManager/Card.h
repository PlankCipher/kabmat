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

  string content;
  string description;
  vector<ChecklistItem> checklist;
};
