#pragma once

#include <string>
#include <vector>

using namespace std;

class Column {
public:
  Column(string title);

  void add_card(string content);

  string title;
  vector<string> cards;
};
