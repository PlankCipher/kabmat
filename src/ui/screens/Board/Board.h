#pragma once

#include <ncurses.h>
#include <string>

#include "../../../DataManager/DataManager.h"

using namespace std;

class BoardScreen {
public:
  BoardScreen(string board_name, DataManager *data_manager, bool from_tui);

  void show();

  WINDOW *window;
  int height;
  int width;
  int start_y;
  int start_x;

  Board *board;
  DataManager *data_manager;
  bool from_tui;
};
