#pragma once

#include <ncurses.h>
#include <string>

using namespace std;

class Input {
public:
  Input(int start_y, int start_x, int height, int width, string content = "");

  string show();

  string content;

private:
  bool handle_key_press(char key);
  void update_ends();

  int start_y;
  int start_x;
  int height;
  int width;
  WINDOW *window;

  size_t right_end;
  size_t left_end;
};
