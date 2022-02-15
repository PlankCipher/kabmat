#pragma once

#include <ncurses.h>
#include <string>

#include "../../../DataManager/DataManager.h"
#include "../../components/ColumnWin/ColumnWin.h"
#include "../../components/ScrollableWindow/ScrollableWindow.h"

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

  ScrollableWindow<ColumnWin> columns_window;
  vector<ColumnWin> columns;
  size_t columns_count;
  int focused_index;

private:
  void draw_columns(vector<ColumnWin> shown_columns, WINDOW *scrollable_window);
  bool handle_key_press(char key);
  void focus_current();
};
