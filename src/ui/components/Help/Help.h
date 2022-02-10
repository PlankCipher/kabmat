#pragma once

#include <ncurses.h>
#include <string>
#include <vector>

using namespace std;

class Help {
public:
  Help();

  void show();

private:
  void draw_help_message(vector<string> shown_message_lines,
                         WINDOW *scrollable_window);

  WINDOW *window;
  int height;
  int width;
  int start_y;
  int start_x;
};
