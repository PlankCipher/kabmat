#pragma once

#include <string>

using namespace std;

struct Config {
  bool tui_enabled = true;
  string default_board = "";
  bool move_card_to_column_bottom = false;
};
