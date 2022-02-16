#pragma once

#include <ncurses.h>
#include <string>
#include <vector>

using namespace std;

class Help {
public:
  Help();

  void show();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  void draw_help_message(vector<string> shown_message_lines,
                         WINDOW *scrollable_window);
};
