#pragma once

#include <ncurses.h>

#include "../../../DataManager/Column.h"

class ColumnWin {
public:
  ColumnWin(int height, int width, int start_y, Column *column);

  void show(int start_x);
  void focus();
  void unfocus();

  int height;
  int width;
  int start_y;
  WINDOW *window;

private:
  Column *column;
};
