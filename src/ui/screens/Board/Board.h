#pragma once

#include "../../../Config/Config.h"
#include "../../../DataManager/DataManager.h"
#include "../../components/ColumnWin/ColumnWin.h"
#include "../../components/ScrollableWindow/ScrollableWindow.h"

class BoardScreen {
public:
  BoardScreen(string board_name, DataManager *data_manager, Config *config,
              bool from_tui);

  void show();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  void draw_columns(vector<ColumnWin> shown_columns, WINDOW *scrollable_window);
  bool handle_key_press(char key);
  void focus_current();

  void setup_columns();
  string create_input_window(string title, string content = "");

  Board *board;
  DataManager *data_manager;
  Config *config;
  vector<ColumnWin> columns;
  size_t columns_count;
  ScrollableWindow<ColumnWin> columns_window;
  int focused_index;

  bool from_tui;
};
