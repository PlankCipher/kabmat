#include "CardWin.h"
#include "../../../helpers/consts.h"
#include "../../helpers/win_fit_text/win_fit_text.h"

CardWin::CardWin(int height, int width, string *content) {
  this->height = height;
  this->width = width;
  this->start_y = 0;
  this->start_x = 0;

  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);
  refresh();

  this->content = content;
}

void CardWin::show(int start_y, int start_x) {
  this->start_y = start_y;
  this->start_x = start_x;

  mvwin(this->window, this->start_y, this->start_x);

  box(this->window, 0, 0);

  string shown_content = win_fit_text(this->window, *this->content);
  mvwprintw(this->window, 1, 1, "%s", shown_content.c_str());
}

void CardWin::focus() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  wrefresh(this->window);
}

void CardWin::unfocus() {
  box(this->window, 0, 0);
  wrefresh(this->window);
}
