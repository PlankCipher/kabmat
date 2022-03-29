#include "ConfirmDialog.h"
#include "../../../helpers/consts.h"

ConfirmDialog::ConfirmDialog(int height, int width, int start_y, int start_x,
                             string message) {
  this->height = height;
  this->width = width;
  this->start_y = start_y;
  this->start_x = start_x;
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);

  this->message = message;
}

bool ConfirmDialog::show() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  string window_title = " Confirm ";
  mvwprintw(this->window, 0, (this->width / 2) - (window_title.length() / 2),
            "%s", window_title.c_str());
  refresh();
  wrefresh(this->window);

  mvwprintw(this->window, 1, 2, "%s", this->message.c_str());

  // color the "y" in "yes" differently
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_KEY_HINT));
  mvwprintw(this->window, 3, (this->width / 2) - 4, "y");
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_KEY_HINT));
  mvwprintw(this->window, 3, (this->width / 2) - 3, "es");

  // color the "n" in "no" differently
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_KEY_HINT));
  mvwprintw(this->window, 3, (this->width / 2) + 2, "n");
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_KEY_HINT));
  mvwprintw(this->window, 3, (this->width / 2) + 3, "o");

  wrefresh(this->window);

  char key;
  while ((key = wgetch(this->window))) {
    switch (key) {
    case '\n': // Enter
    case 'y':
      // yes
      werase(this->window);
      wrefresh(this->window);
      refresh();
      return true;

      break;
    case 27: // ESC
    case 'n':
      // no
      werase(this->window);
      wrefresh(this->window);
      refresh();
      return false;

      break;
    }
  }

  return false;
}
