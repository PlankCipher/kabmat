#include "ColumnWin.h"
#include "../../../helpers/consts.h"
#include "../../helpers/win_center_text/win_center_text.h"

ColumnWin::ColumnWin(int height, int width, int start_y, Column *column) {
  this->height = height;
  this->width = width;
  this->start_y = start_y;

  this->window = newwin(height, width, start_y, 0);
  refresh();

  this->column = column;
}

void ColumnWin::show(int start_x) {
  mvwin(this->window, this->start_y, start_x);

  box(this->window, 0, 0);

  string column_title = " " + this->column->title + " ";
  int center_x = win_center_x(this->window, &column_title);
  mvwprintw(this->window, 0, center_x, "%s", column_title.c_str());

  wrefresh(this->window);
}

void ColumnWin::focus() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  string column_title = " " + this->column->title + " ";
  int center_x = win_center_x(this->window, &column_title);
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_FOOTER));
  mvwprintw(this->window, 0, center_x, "%s", column_title.c_str());
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_FOOTER));

  wrefresh(this->window);
}

void ColumnWin::unfocus() {
  box(this->window, 0, 0);

  string column_title = " " + this->column->title + " ";
  int center_x = win_center_x(this->window, &column_title);
  mvwprintw(this->window, 0, center_x, "%s", column_title.c_str());

  wrefresh(this->window);
}
