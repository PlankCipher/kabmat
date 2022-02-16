#include <ncurses.h>

#include "../../../helpers/consts.h"
#include "Header.h"

Header::Header(string board_name) { this->board_name = board_name; }

void Header::show() {
  int max_x = getmaxx(stdscr);

  // centered board name
  int center_x = (max_x / 2) - (this->board_name.length() / 2);
  mvprintw(0, center_x, "%s", this->board_name.c_str());

  mvchgat(0, 0, max_x, A_NORMAL, COLOR_PAIR_HEADER, NULL);

  refresh();
}
