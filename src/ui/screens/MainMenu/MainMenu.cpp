#include <functional>

#include "../../../helpers/consts.h"
#include "../../components/Footer/Footer.h"
#include "../../components/Input/Input.h"
#include "../../helpers/win_center_text/win_center_text.h"
#include "MainMenu.h"

// I had to
MainMenu::MainMenu(DataManager *data_manager)
    : height{(int)(getmaxy(stdscr) * 0.3)}, width{(int)(getmaxx(stdscr) * 0.2)},
      start_y{(getmaxy(stdscr) / 2) - (this->height / 2)},
      start_x{(getmaxx(stdscr) / 2) - (this->width / 2)},
      boards_names{data_manager->get_boards_names()},
      boards_count{this->boards_names.size()}, menu_window_height{this->height -
                                                                  3},
      menu_window{ScrollableWindow<string>(
          this->menu_window_height, this->width - 2, this->start_y + 2,
          this->start_x + 1, &this->boards_names, &this->boards_count,
          bind(&MainMenu::draw_menu_items, this, placeholders::_1,
               placeholders::_2))} {
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);

  this->highlighted_index = 0;
  this->data_manager = data_manager;
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

  refresh();
  wrefresh(this->window);

  this->menu_window.scroll_to_top();

  char key;
  while ((key = wgetch(this->window))) {
    this->handle_key_press(key);
  }
}

void MainMenu::draw_menu_items(vector<string> shown_boards,
                               WINDOW *scrollable_window) {
  if (shown_boards.size() > 0) {
    // draw boards list
    for (size_t i = 0; i < shown_boards.size(); ++i) {
      int center_x = win_center_x(scrollable_window, &shown_boards[i]);
      mvwprintw(scrollable_window, i, center_x, "%s", shown_boards[i].c_str());
    }

    // highlight current item
    this->highlight_current();
  } else {
    mvwchgat(scrollable_window, 0, 0, getmaxx(scrollable_window), A_NORMAL, 0,
             NULL);
    string create_board_hint = "c to create a new board";
    int center_x = win_center_x(scrollable_window, &create_board_hint);
    mvwprintw(scrollable_window, 0, center_x, "%s", create_board_hint.c_str());
  }

  refresh();
  wrefresh(this->window);
  wrefresh(scrollable_window);
}

void MainMenu::highlight_current() {
  if (this->boards_count > 0) {
    // clear previous highlights
    for (int i = 0; i < this->menu_window_height; ++i)
      mvwchgat(this->menu_window.window, i, 0,
               getmaxx(this->menu_window.window), A_NORMAL, 0, NULL);

    // highlight current
    mvwchgat(this->menu_window.window, this->highlighted_index, 0,
             getmaxx(this->menu_window.window), A_NORMAL, COLOR_PAIR_FOOTER,
             NULL);

    refresh();
    wrefresh(this->window);
    wrefresh(this->menu_window.window);
  }
}

void MainMenu::handle_key_press(char key) {
  switch (key) {
  case 'q': {
    // quit
    endwin();
    exit(0);

    break;
  }
  case 'k': {
    // move up
    if (--this->highlighted_index == -1) {
      this->menu_window.scroll_up();
      this->highlighted_index = 0;
    }
    this->highlight_current();

    break;
  }
  case 'j': {
    // move down
    this->highlighted_index =
        min(this->boards_count - 1, (size_t)this->highlighted_index + 1);

    if (this->highlighted_index == this->menu_window_height) {
      this->menu_window.scroll_down();
      this->highlighted_index = this->menu_window_height - 1;
    }
    this->highlight_current();

    break;
  }
  case 'g': {
    // go to first board
    this->menu_window.scroll_to_top();
    this->highlighted_index = 0;
    this->highlight_current();

    break;
  }
  case 'G': {
    // go to last board
    this->menu_window.scroll_to_bottom();
    this->highlighted_index =
        min((size_t)this->menu_window_height - 1, this->boards_count - 1);
    this->highlight_current();

    break;
  }
  case 'd': {
    // delete board
    if (this->boards_count > 0) {
      string board_to_delete =
          *(this->menu_window.window_start + this->highlighted_index);
      this->data_manager->delete_board(board_to_delete);

      this->boards_names = data_manager->get_boards_names();
      this->boards_count = this->boards_names.size();

      this->menu_window.scroll_to_top();
      this->highlighted_index =
          min((size_t)this->highlighted_index, this->boards_count - 1);
      this->highlight_current();
    }

    break;
  }
  case 'c': {
    // create board
    string board_name = this->create_input_window(" New Board Name ");

    if (board_name.length() > 0) {
      this->data_manager->create_board(board_name);

      this->boards_names = data_manager->get_boards_names();
      this->boards_count = this->boards_names.size();

      // highlight the just created board
      this->menu_window.scroll_to_bottom();
      this->highlighted_index =
          min((size_t)this->menu_window_height - 1, this->boards_count - 1);
      this->highlight_current();
    }

    break;
  }
  case 'r': {
    // rename board
    string old_name =
        *(this->menu_window.window_start + this->highlighted_index);
    string new_name = this->create_input_window(" Rename Board ", old_name);

    if (new_name.length() > 0) {
      size_t offset =
          this->menu_window.window_start - this->boards_names.begin();

      this->data_manager->rename_board(old_name, new_name);

      this->boards_names = this->data_manager->get_boards_names();
      this->menu_window.scroll_to_offset(offset);
    }

    break;
  }
  case '\n': {
    // enter board

    break;
  }
  }
}

string MainMenu::create_input_window(string title, string content) {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  int height = 1;
  int width = max_x * 0.4;

  int start_y = max_y / 2;
  int start_x = (max_x / 2) - (width / 2);

  WINDOW *outer_box = newwin(height + 2, width + 2, start_y - 1, start_x - 1);

  wattron(outer_box, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(outer_box, 0, 0);
  wattroff(outer_box, COLOR_PAIR(COLOR_PAIR_BORDER));

  mvwprintw(outer_box, 0, 1, "%s", title.c_str());

  refresh();
  wrefresh(outer_box);

  Input input_bar(height, width, start_y, start_x, content);
  string input = input_bar.show();

  werase(outer_box);
  wrefresh(outer_box);

  // restoring previous look of menu
  // after erasing the input window
  // border
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  // menu title
  string menu_title = " Boards ";
  int center_x = win_center_x(this->window, &menu_title);
  mvwprintw(this->window, 0, center_x, "%s", menu_title.c_str());

  refresh();
  wrefresh(this->window);

  this->menu_window.draw();

  return input;
}
