#pragma once

#include <ncurses.h>

#include "../../../DataManager/Column.h"
#include "../CardWin/CardWin.h"
#include "../ScrollableWindow/ScrollableWindow.h"

class ColumnWin {
public:
  ColumnWin(int height, int width, int start_y, Column *column);

  void show(int start_x);

  void focus();
  void unfocus();
  void focus_current();
  void focus_prev();
  void focus_next();
  void focus_first();
  void focus_last();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  void draw_cards(vector<CardWin> shown_cards, WINDOW *scrollable_window);
  void setup_cards_window();

  Column *column;

  vector<CardWin> cards;
  size_t cards_count;
  ScrollableWindow<CardWin> cards_window;

  int focused_index;
  size_t cards_window_offset;
};
