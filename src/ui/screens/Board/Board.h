#pragma once

#include "../../components/ColumnWin/ColumnWin.h"

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
  void setup_columns();

  bool handle_key_press(char key);
  void focus_current();

  string create_input_window(string title, string content = "",
                             bool focused = false);
  bool create_card_info_window(Card *card);
  bool create_confirm_dialog(string message);

  Board *board;
  DataManager *data_manager;
  Config *config;
  bool from_tui;

  vector<ColumnWin> columns;
  size_t columns_count;
  ScrollableWindow<ColumnWin> columns_window;
  int focused_index;
};
