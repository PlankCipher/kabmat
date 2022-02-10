#include <ncurses.h>
#include <string>

#include "../../../helpers/consts.h"
#include "Footer.h"

using namespace std;

Footer::Footer(bool show_mode, bool show_help_hint) {
  this->show_mode = show_mode;
  this->show_help_hint = show_help_hint;
}

void Footer::show() {
  int y_max, x_max;
  getmaxyx(stdscr, y_max, x_max);

  // mode
  if (this->show_mode)
    // TODO: use real mode
    mvprintw(y_max - 1, 1, "NORMAL");

  // centered program info
  string info = string(NAME) + " " + string(VERSION);
  int center_x = (x_max / 2) - (info.length() / 2);
  mvprintw(y_max - 1, center_x, "%s", info.c_str());

  // help hint
  if (this->show_help_hint)
    mvprintw(y_max - 1, x_max - 11, "h for help");

  mvchgat(y_max - 1, 0, x_max, A_NORMAL, COLOR_PAIR_FOOTER, NULL);
}
