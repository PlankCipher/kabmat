#pragma once

#include <ncurses.h>
#include <string>

using namespace std;

class Input {
public:
  Input(int height, int width, int start_y, int start_x, string content = "",
        string title = "");

  string show();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  bool handle_key_press(char key);
  void update_ends();

  string content;
  string title;

  size_t right_end;
  size_t left_end;
};
