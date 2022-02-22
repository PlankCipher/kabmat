#pragma once

#include "../../../Config/Config.h"
#include "../../../DataManager/DataManager.h"
#include "../../components/ScrollableWindow/ScrollableWindow.h"

class MainMenu {
public:
  MainMenu(DataManager *data_manager, Config *config);

  void show();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  void setup_window();
  void draw_menu_items(vector<string> shown_boards, WINDOW *scrollable_window);
  void highlight_current();
  void handle_key_press(char key);
  string create_input_window(string title, string content = "");

  DataManager *data_manager;
  Config *config;
  vector<string> boards_names;
  size_t boards_count;
  ScrollableWindow<string> menu_window;
  int highlighted_index;
};
