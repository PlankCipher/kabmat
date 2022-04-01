#pragma once

#include "../../../DataManager/DataManager.h"
#include "../CardWin/CardWin.h"
#include "../ScrollableWindow/ScrollableWindow.h"

class ColumnWin {
public:
  ColumnWin(int height, int width, int start_y, Column *column);

  void show(int start_x);

  void update_cards();

  void focus();
  void unfocus();
  void focus_current();
  void focus_prev();
  void focus_next();
  void focus_first();
  void focus_last();

  void move_focused_card_up(DataManager *data_manager);
  void move_focused_card_down(DataManager *data_manager);

  size_t get_absolute_focused_index();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

  size_t cards_count;
  int focused_index;

  size_t cards_window_offset;

  Column *column;

private:
  void draw_cards(vector<CardWin> shown_cards, WINDOW *scrollable_window);
  void setup_cards_window();

  vector<CardWin> cards;
  ScrollableWindow<CardWin> cards_window;
};
