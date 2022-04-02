#include "MainMenu.h"
#include "../../../helpers/consts.h"
#include "../../components/ConfirmDialog/ConfirmDialog.h"
#include "../../components/Footer/Footer.h"
#include "../../components/Help/Help.h"
#include "../../components/Input/Input.h"
#include "../../helpers/win_center_text/win_center_text.h"
#include "../Board/Board.h"

MainMenu::MainMenu(DataManager *data_manager, Config *config)
    : height{(int)(getmaxy(stdscr) * 0.3)}, width{(int)(getmaxx(stdscr) * 0.2)},
      start_y{(getmaxy(stdscr) / 2) - (this->height / 2)},
      start_x{(getmaxx(stdscr) / 2) - (this->width / 2)},
      menu_window{this->height - 3,
                  this->width - 2,
                  this->start_y + 2,
                  this->start_x + 1,
                  &this->boards_names,
                  &this->boards_count,
                  bind(&MainMenu::draw_menu_items, this, placeholders::_1,
                       placeholders::_2)} {
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);
  keypad(this->window, true);
  refresh();

  this->data_manager = data_manager;
  this->config = config;
  this->boards_names = this->data_manager->get_boards_names();
  this->boards_count = this->boards_names.size();
  this->highlighted_index = 0;
}

void MainMenu::setup_window() {
  // border
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  // menu title
  string menu_title = " Boards ";
  int center_x = win_center_x(this->window, &menu_title);
  mvwprintw(this->window, 0, center_x, "%s", menu_title.c_str());

  wrefresh(this->window);

  this->menu_window.draw();
}

void MainMenu::show() {
  Footer footer(true, true);
  footer.show();

  this->menu_window.scroll_to_top();

  this->setup_window();

  char key;
  while ((key = wgetch(this->window)))
    this->handle_key_press(key);
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

  wrefresh(this->window);
  wrefresh(scrollable_window);
}

void MainMenu::highlight_current() {
  if (this->boards_count > 0) {
    // clear previous highlights
    for (int i = 0; i < this->menu_window.max_items_in_win; ++i)
      mvwchgat(this->menu_window.window, i, 0,
               getmaxx(this->menu_window.window), A_NORMAL, 0, NULL);

    // highlight current
    mvwchgat(this->menu_window.window, this->highlighted_index, 0,
             getmaxx(this->menu_window.window), A_NORMAL, COLOR_PAIR_FOOTER,
             NULL);

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
  case '?': {
    Help help_window;
    help_window.show();

    this->setup_window();

    break;
  }
  case 'k': {
    this->highlight_above();
    break;
  }
  case 'j': {
    this->highlight_below();

    break;
  }
  case 'g': {
    // highlight first board
    if (this->boards_count > 0) {
      this->highlighted_index = 0;
      this->menu_window.scroll_to_top();
    }

    break;
  }
  case 'G': {
    // highlight last board
    if (this->boards_count > 0) {
      this->highlighted_index =
          min((size_t)this->menu_window.max_items_in_win - 1,
              this->boards_count - 1);
      this->menu_window.scroll_to_bottom();
    }

    break;
  }
  case 'K': {
    // move highlighted board up
    if (this->boards_count > 0) {
      size_t highlighted_board_index =
          (this->menu_window.window_start - this->boards_names.begin()) +
          this->highlighted_index;
      size_t prev_offset =
          this->menu_window.window_start - this->boards_names.begin();

      bool moved = this->data_manager->move_board_up(highlighted_board_index);

      if (moved) {
        this->boards_names = this->data_manager->get_boards_names();
        this->menu_window.scroll_to_offset(prev_offset);
        this->highlight_above();
      }
    }

    break;
  }
  case 'J': {
    // move highlighted board down
    if (this->boards_count > 0) {
      size_t highlighted_board_index =
          (this->menu_window.window_start - this->boards_names.begin()) +
          this->highlighted_index;
      size_t prev_offset =
          this->menu_window.window_start - this->boards_names.begin();

      bool moved = this->data_manager->move_board_down(highlighted_board_index);

      if (moved) {
        this->boards_names = this->data_manager->get_boards_names();
        this->menu_window.scroll_to_offset(prev_offset);
        this->highlight_below();
      }
    }

    break;
  }
  case 'd': {
    // delete highlighted board
    if (this->boards_count > 0) {
      string board_to_delete =
          *(this->menu_window.window_start + this->highlighted_index);

      string message_board_name = board_to_delete.length() > 11
                                      ? board_to_delete.substr(0, 9) + ".."
                                      : board_to_delete;
      string message = "Delete board \"" + message_board_name + "\"?";
      int height = 5;
      int width = message.length() + 4;
      int start_y = (getmaxy(stdscr) / 2) - (height / 2);
      int start_x = (getmaxx(stdscr) / 2) - (width / 2);

      ConfirmDialog confirm_dialog(height, width, start_y, start_x, message);
      bool confirmed = confirm_dialog.show();

      this->setup_window();

      if (confirmed) {
        size_t prev_offset =
            this->menu_window.window_start - this->boards_names.begin();

        this->data_manager->delete_board(board_to_delete);

        this->boards_names = this->data_manager->get_boards_names();
        this->boards_count = this->boards_names.size();

        this->highlighted_index =
            min((size_t)this->highlighted_index, this->boards_count - 1);
        this->menu_window.scroll_to_offset(prev_offset);
      }
    }

    break;
  }
  case 'c': {
    // create board
    string board_name = this->create_input_window(" New Board Name ", "", true);

    if (board_name.length() > 0) {
      this->data_manager->create_board(board_name);

      this->boards_names = data_manager->get_boards_names();
      this->boards_count = this->boards_names.size();

      // highlight the just created board
      this->highlighted_index =
          min((size_t)this->menu_window.max_items_in_win - 1,
              this->boards_count - 1);
      this->menu_window.scroll_to_bottom();
    }

    break;
  }
  case 'r': {
    // rename highlighted board
    if (this->boards_count > 0) {
      string old_name =
          *(this->menu_window.window_start + this->highlighted_index);
      string new_name =
          this->create_input_window(" Rename Board ", old_name, true);

      if (new_name.length() > 0) {
        size_t offset =
            this->menu_window.window_start - this->boards_names.begin();

        this->data_manager->rename_board(old_name, new_name);

        this->boards_names = this->data_manager->get_boards_names();
        this->menu_window.scroll_to_offset(offset);
      }
    }

    break;
  }
  case '\n': {
    // enter highlighted board
    if (this->boards_count > 0) {
      string board_to_open =
          *(this->menu_window.window_start + this->highlighted_index);

      BoardScreen board_screen(board_to_open, this->data_manager, this->config,
                               true);
      board_screen.show();

      // restoring previous look of menu window
      erase();
      refresh();

      Footer footer(true, true);
      footer.show();

      this->setup_window();
    }
    break;
  }
  }
}

void MainMenu::highlight_above() {
  if (this->boards_count > 0) {
    if (--this->highlighted_index == -1) {
      this->highlighted_index = 0;
      this->menu_window.scroll_up();
    } else
      this->highlight_current();
  }
}

void MainMenu::highlight_below() {
  if (this->boards_count > 0) {
    this->highlighted_index =
        min(this->boards_count - 1, (size_t)this->highlighted_index + 1);

    if (this->highlighted_index == this->menu_window.max_items_in_win) {
      this->highlighted_index = this->menu_window.max_items_in_win - 1;
      this->menu_window.scroll_down();
    } else
      this->highlight_current();
  }
}

string MainMenu::create_input_window(string title, string content,
                                     bool focused) {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  int height = 3;
  int width = max_x * 0.4;
  int start_y = (max_y / 2) - (height / 2);
  int start_x = (max_x / 2) - (width / 2);

  Input input_bar(height, width, start_y, start_x, content, title, focused);
  input_bar.show(true);
  string input = input_bar.get_value();

  this->setup_window();

  return input;
}
