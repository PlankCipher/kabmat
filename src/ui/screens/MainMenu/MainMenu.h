#pragma once

#include <ncurses.h>

#include "../../../DataManager/DataManager.h"

class MainMenu {
public:
  MainMenu(DataManager *data_manager);

  void show();

  WINDOW *window;
  int height;
  int width;

private:
  void draw_menu_items();
  void handle_key_press(char key);
  string create_input_window(string title, string content = "");

  int items_start_y;
  size_t highlighted_index;
  vector<string> boards_names;
  size_t boards_count;
  size_t scroll_window_height;
  vector<string>::iterator scroll_window_start;
  vector<string>::iterator scroll_window_end;

  DataManager *data_manager;
};
