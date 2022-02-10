#pragma once

#include <ncurses.h>
#include <string>

using namespace std;

class Input {
public:
  Input(int height, int width, int start_y, int start_x, string content = "",
        string title = "");

  string show();

  string content;
  string title;

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
