#include "MainMenu.h"
#include "../../../helpers/consts.h"
#include "../../components/Footer/Footer.h"
#include "../../helpers/win_center_text/win_center_text.h"

MainMenu::MainMenu(DataManager data_manager) {
  int y_max, x_max;
  getmaxyx(stdscr, y_max, x_max);

  this->height = y_max * 0.30;
  this->width = x_max * 0.20;

  int center_y = (y_max / 2) - (this->height / 2);
  int center_x = (x_max / 2) - (this->width / 2);

  this->window = newwin(this->height, this->width, center_y, center_x);

  // starts at 2 because: one row for
  // border and the other for space
  this->items_start_y = 2;

  this->highlighted_index = 0;
  this->boards_names = data_manager.get_boards_names();
  this->boards_count = this->boards_names.size();
  this->scroll_window_height = this->height - items_start_y - 1;

  if (this->boards_count > 0) {
    this->scroll_window_start = this->boards_names.begin();
    this->scroll_window_end =
        this->scroll_window_start +
        min(this->boards_count, this->scroll_window_height);
  }
}

void MainMenu::show() {
  Footer footer(false, false);
  footer.show();

  // border
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  // menu title
  string menu_title = " Boards ";
  int center_x = win_center_x(this->window, &menu_title);
  mvwprintw(this->window, 0, center_x, "%s", menu_title.c_str());

  // menu items
  this->draw_menu_items();

  refresh();
  wrefresh(this->window);

  char key;
  while ((key = wgetch(this->window))) {
    werase(this->window);

    // border
    wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
    box(this->window, 0, 0);
    wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

    // menu title
    mvwprintw(this->window, 0, center_x, "%s", menu_title.c_str());

    this->handle_key_press(key);
    this->draw_menu_items();
  }
}

void MainMenu::draw_menu_items() {
  if (this->boards_count > 0) {
    // clear any previous highlights
    for (size_t i = 0; i < this->scroll_window_height; ++i) {
      mvwchgat(this->window, i + this->items_start_y, 1, this->width - 2,
               A_NORMAL, 0, NULL);
    }

    // boards list
    vector<string> boards_menu =
        vector<string>(this->scroll_window_start, this->scroll_window_end);

    for (size_t i = 0; i < boards_menu.size(); ++i) {
      int center_x = win_center_x(this->window, &boards_menu[i]);
      mvwprintw(this->window, i + this->items_start_y, center_x, "%s",
                boards_menu[i].c_str());
    }

    // highlight current item
    mvwchgat(this->window, this->highlighted_index + this->items_start_y, 1,
             this->width - 2, A_NORMAL, COLOR_PAIR_FOOTER, NULL);

    refresh();
    wrefresh(this->window);
  } else {
    string create_board_hint = "n to create a new board";
    int center_x = win_center_x(this->window, &create_board_hint);
    mvwprintw(this->window, this->items_start_y, center_x, "%s",
              create_board_hint.c_str());
  }
}

void MainMenu::handle_key_press(char key) {
  switch (key) {
  case 'q':
    endwin();
    exit(0);

    break;
  case 'k':
    if (--this->highlighted_index == SIZE_MAX) {
      this->scroll_window_start =
          max(this->boards_names.begin(), this->scroll_window_start - 1);
      this->scroll_window_end =
          this->scroll_window_start +
          min(this->boards_count, this->scroll_window_height);

      this->highlighted_index = 0;
    }

    break;
  case 'j':
    this->highlighted_index =
        min(this->boards_count - 1, this->highlighted_index + 1);

    if (this->highlighted_index == this->scroll_window_height) {
      this->scroll_window_end =
          min(this->boards_names.end(), this->scroll_window_end + 1);
      this->scroll_window_start =
          this->scroll_window_end -
          min(this->boards_count, this->scroll_window_height);

      this->highlighted_index = this->scroll_window_height - 1;
    }

    break;
  case 'd':
    break;
  case 'n':
    break;
  case 'g':
    break;
  case 'G':
    break;
  case '\n':
    break;
  }
}
