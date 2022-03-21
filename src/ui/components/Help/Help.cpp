#include "Help.h"
#include "../../../helpers/consts.h"
#include "../ScrollableWindow/ScrollableWindow.h"

Help::Help() {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  this->height = max_y * 0.5;
  this->width = max_x * 0.6;

  this->start_y = (max_y / 2) - (height / 2);
  this->start_x = (max_x / 2) - (width / 2);

  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);
}

void Help::show() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  string window_title = " Help ";
  mvwprintw(this->window, 0, (this->width / 2) - (window_title.length() / 2),
            "%s", window_title.c_str());

  vector<string> message_lines = {
      "Help Window keybindings:",
      "k   scroll up one line",
      "j   scroll down one line",
      "q   close the window",
      "",
      "",
      "Main Menu keybindings:",
      "q       quit",
      "?       show this help window",
      "k       highlight the above board name",
      "j       highlight the below board name",
      "g       highlight the first board name",
      "G       highlight the last board name",
      "d       delete the currently highlighted board",
      "r       rename the currently highlighted board",
      "c       create a new board and highlight it",
      "Enter   open the currently highlighted board",
      "",
      "",
      "Input Field keybindings:",
      "  Normal mode:",
      "  Esc, q   cancel and close the input field",
      "  Enter    submit and close the input field",
      "  h        move cursor one character to the left",
      "  l        move cursor one character to the right",
      "  0        move cursor to the start of the line",
      "  $        move cursor to the end of the line",
      "  k        move cursor up one line (in multi-row input only)",
      "  j        move cursor down one line (in multi-row input only)",
      "  g        move cursor to the first line (in multi-row input only)",
      "  G        move cursor to the last line (in multi-row input only)",
      "  i        change mode to insert",
      "  a        move cursor one character to the right and change mode to "
      "insert",
      "  I        move cursor to the start of the line and change mode to "
      "insert",
      "  A        move cursor to the end of the line and change mode to insert",
      "  S        delete everything on the line and change mode to insert",
      "  d        delete line under cursor (in multi-row input only)",
      "",
      "  Insert mode:",
      "  Esc                change mode to normal",
      "  Enter              submit and close the input field (or add a new "
      "line in multi-row input)",
      "  Backspace/Delete   delete the character before the cursor",
      "  (Any other key is inserted before the cursor)",
  };
  size_t message_lines_count = message_lines.size();

  ScrollableWindow<string> help_window(
      this->height - 2, this->width - 2, this->start_y + 1, this->start_x + 1,
      &message_lines, &message_lines_count,
      bind(&Help::draw_help_message, this, placeholders::_1, placeholders::_2));

  help_window.scroll_to_top();

  char key;
  bool done = false;
  while (!done && (key = wgetch(this->window))) {
    switch (key) {
    case 'q':
      done = true;

      break;
    case 'k':
      help_window.scroll_up();

      break;
    case 'j':
      help_window.scroll_down();

      break;
    }
  }

  werase(this->window);
  wrefresh(this->window);
}

void Help::draw_help_message(vector<string> shown_message_lines,
                             WINDOW *scrollable_window) {
  for (size_t i = 0; i < shown_message_lines.size(); ++i)
    mvwprintw(scrollable_window, i, 0, "%s", shown_message_lines[i].c_str());

  wrefresh(this->window);
  wrefresh(scrollable_window);
}
