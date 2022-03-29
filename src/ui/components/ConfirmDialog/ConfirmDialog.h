#pragma once

#include <ncurses.h>
#include <string>

using namespace std;

class ConfirmDialog {
public:
  ConfirmDialog(int height, int width, int start_y, int start_x,
                string message);

  bool show();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

  string message;
};
