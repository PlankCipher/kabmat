#pragma once

#include <ncurses.h>
#include <string>

using namespace std;

class CardWin {
public:
  CardWin(int height, int width, string *content);

  void show(int start_y, int start_x);
  void focus();
  void unfocus();

  int height;
  int width;
  int start_x;
  int start_y;
  WINDOW *window;

private:
  string *content;
};
