#include <ncurses.h>
#include <string>

#include "../../../helpers/consts.h"
#include "Footer.h"

using namespace std;

Footer::Footer(bool show_mode, bool show_help_hint) {
  this->show_mode = show_mode;
  this->show_help_hint = show_help_hint;
  this->mode = MODE_NORMAL;
}

void Footer::show() {
  int y_max, x_max;
  getmaxyx(stdscr, y_max, x_max);

  // mode
  if (this->show_mode) {
    attron(COLOR_PAIR(COLOR_PAIR_MODE));
    mvprintw(getmaxy(stdscr) - 1, 0,
             this->mode == MODE_NORMAL ? " NORMAL " : " INSERT ");
    attroff(COLOR_PAIR(COLOR_PAIR_MODE));
  }

  // centered program info
  string info = string(NAME) + " " + string(VERSION);
  int center_x = (x_max / 2) - (info.length() / 2);
  mvprintw(y_max - 1, center_x, "%s", info.c_str());

  // help hint
  if (this->show_help_hint)
    mvprintw(y_max - 1, x_max - 11, "? for help");

  mvchgat(y_max - 1, 8, x_max, A_NORMAL, COLOR_PAIR_FOOTER, NULL);

  refresh();
}
