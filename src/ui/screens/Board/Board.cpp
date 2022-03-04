#include "Board.h"
#include "../../components/Footer/Footer.h"
#include "../../components/Header/Header.h"
#include "../../components/Help/Help.h"
#include "../../components/Input/Input.h"
#include "../../helpers/win_center_text/win_center_text.h"

BoardScreen::BoardScreen(string board_name, DataManager *data_manager,
                         Config *config, bool from_tui)
    : height{getmaxy(stdscr) - 4}, width{getmaxx(stdscr) - 2}, start_y{2},
      start_x{1}, columns_window{ScrollableWindow<ColumnWin>(
                      this->height, this->width, this->start_y, this->start_x,
                      &this->columns, &this->columns_count,
                      bind(&BoardScreen::draw_columns, this, placeholders::_1,
                           placeholders::_2),
                      3)} {
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);
  keypad(this->window, true);
  refresh();

  this->data_manager = data_manager;
  this->config = config;
  this->board = this->data_manager->get_board_if_exists(board_name);

  this->setup_columns();
  this->focused_index = 0;

  this->from_tui = from_tui;
}

void BoardScreen::setup_columns() {
  this->columns = {};
  for (size_t i = 0; i < this->board->columns.size(); ++i) {
    // -4 because of 2 spaces between
    // each pair of shown columns
    int width = (this->width - 4) / 3;
    this->columns.push_back(ColumnWin(this->height, width, this->start_y,
                                      &this->board->columns[i]));
  }
  this->columns_count = this->columns.size();
}

void BoardScreen::show() {
  Header header(this->board->name);
  header.show();

  Footer footer(true, true);
  footer.show();

  this->columns_window.scroll_to_top();

  char key;
  bool done = false;
  while (!done && (key = wgetch(this->window)))
    done = this->handle_key_press(key);
}

void BoardScreen::draw_columns(vector<ColumnWin> shown_columns,
                               WINDOW *scrollable_window) {
  wrefresh(this->window);

  if (shown_columns.size() > 0) {
    wrefresh(scrollable_window);

    for (size_t i = 0; i < shown_columns.size(); ++i) {
      int start_x = (i * (shown_columns[i].width + 2)) + this->start_x;
      (*(this->columns_window.window_start + i)).start_x = start_x;
      shown_columns[i].show(start_x);
    }

    this->focus_current();
  } else {
    string create_column_hint = "C to create a new column";
    int center_x = win_center_x(scrollable_window, &create_column_hint);
    mvwprintw(scrollable_window, 0, center_x, "%s", create_column_hint.c_str());

    wrefresh(scrollable_window);
  }
}

void BoardScreen::focus_current() {
  vector<ColumnWin> shown_columns = this->columns_window.get_current_window();
  for (size_t i = 0; i < shown_columns.size(); ++i)
    shown_columns[i].unfocus();

  shown_columns[this->focused_index].focus();
}

bool BoardScreen::handle_key_press(char key) {
  switch (key) {
  case 'q': {
    // quit
    if (this->from_tui)
      return true;

    endwin();
    exit(0);

    break;
  }
  case '?': {
    // help
    Help help_window;
    help_window.show();

    this->focus_current();

    break;
  }
  case 'h': {
    // focus left column
    if (this->columns_count > 0) {
      if (--this->focused_index == -1) {
        this->focused_index = 0;
        this->columns_window.scroll_up();
      } else
        this->focus_current();
    }

    break;
  }
  case 'l': {
    // focus right column
    if (this->columns_count > 0) {
      this->focused_index =
          min(this->columns_count - 1, (size_t)this->focused_index + 1);

      if (this->focused_index == this->columns_window.max_items_in_win) {
        this->focused_index = this->columns_window.max_items_in_win - 1;
        this->columns_window.scroll_down();
      } else
        this->focus_current();
    }

    break;
  }
  case 'k': {
    // focus above card
    if (this->columns_count > 0) {
      vector<ColumnWin> shown_columns =
          this->columns_window.get_current_window();
      shown_columns[this->focused_index].focus_prev();
      *(this->columns_window.window_start + this->focused_index) =
          shown_columns[this->focused_index];
    }

    break;
  }
  case 'j': {
    // focus below card
    if (this->columns_count > 0) {
      vector<ColumnWin> shown_columns =
          this->columns_window.get_current_window();
      shown_columns[this->focused_index].focus_next();
      *(this->columns_window.window_start + this->focused_index) =
          shown_columns[this->focused_index];
    }

    break;
  }
  case 'g': {
    // focus first card
    if (this->columns_count > 0) {
      vector<ColumnWin> shown_columns =
          this->columns_window.get_current_window();
      shown_columns[this->focused_index].focus_first();
      *(this->columns_window.window_start + this->focused_index) =
          shown_columns[this->focused_index];
    }

    break;
  }
  case 'G': {
    // focus last card
    if (this->columns_count > 0) {
      vector<ColumnWin> shown_columns =
          this->columns_window.get_current_window();
      shown_columns[this->focused_index].focus_last();
      *(this->columns_window.window_start + this->focused_index) =
          shown_columns[this->focused_index];
    }

    break;
  }
  case 'H': {
    // move card to left/prev column
    if (this->columns_count > 0) {
      // scroll offset + current focused index
      size_t src_index =
          (this->columns_window.window_start - this->columns.begin()) +
          this->focused_index;

      ColumnWin *src_column = &(this->columns[src_index]);

      if (src_column->cards_count > 0) {
        size_t dist_index = src_index - 1;
        size_t card_index = src_column->get_absolute_focused_index();

        bool moved = this->data_manager->move_card_to_prev_column(
            this->board, card_index, src_index, dist_index, this->config);

        if (moved) {
          src_column->update_cards();
          src_column->show(src_column->start_x);

          if (card_index == src_column->cards_count)
            src_column->focus_last();

          this->columns[dist_index].update_cards();
          this->columns[dist_index].show(this->columns[dist_index].start_x);

          this->focus_current();
        }
      }
    }

    break;
  }
  case 'L': {
    // move card to right/next column
    if (this->columns_count > 0) {
      // scroll offset + current focused index
      size_t src_index =
          (this->columns_window.window_start - this->columns.begin()) +
          this->focused_index;

      ColumnWin *src_column = &(this->columns[src_index]);

      if (src_column->cards_count > 0) {
        size_t dist_index = src_index + 1;
        size_t card_index = src_column->get_absolute_focused_index();

        bool moved = this->data_manager->move_card_to_next_column(
            this->board, card_index, src_index, dist_index, this->config);

        if (moved) {
          src_column->update_cards();
          src_column->show(src_column->start_x);

          if (card_index == src_column->cards_count)
            src_column->focus_last();

          this->columns[dist_index].update_cards();
          this->columns[dist_index].show(this->columns[dist_index].start_x);

          this->focus_current();
        }
      }
    }

    break;
  }
  case 'K': {
    // move card up in column
    if (this->columns_count > 0) {
      vector<ColumnWin> shown_columns =
          this->columns_window.get_current_window();
      shown_columns[this->focused_index].move_focused_card_up(
          this->data_manager);
      *(this->columns_window.window_start + this->focused_index) =
          shown_columns[this->focused_index];
    }

    break;
  }
  case 'J': {
    // move card down in column
    if (this->columns_count > 0) {
      vector<ColumnWin> shown_columns =
          this->columns_window.get_current_window();
      shown_columns[this->focused_index].move_focused_card_down(
          this->data_manager);
      *(this->columns_window.window_start + this->focused_index) =
          shown_columns[this->focused_index];
    }

    break;
  }
  case ('h' & 0x1f): {
    // move column to the left
    if (this->columns_count > 0) {
      size_t col_to_mov_index =
          (this->columns_window.window_start - this->columns.begin()) +
          this->focused_index;
      bool moved =
          this->data_manager->move_column_left(this->board, col_to_mov_index);

      if (moved) {
        this->setup_columns();

        // focus left column
        if (--this->focused_index == -1) {
          this->focused_index = 0;
          this->columns_window.scroll_up();
        } else
          this->columns_window.draw();
      }
    }

    break;
  }
  case ('l' & 0x1f): {
    // move column to the right
    if (this->columns_count > 0) {
      size_t col_to_mov_index =
          (this->columns_window.window_start - this->columns.begin()) +
          this->focused_index;
      bool moved =
          this->data_manager->move_column_right(this->board, col_to_mov_index);

      if (moved) {
        this->setup_columns();

        // focus right column
        this->focused_index =
            min(this->columns_count - 1, (size_t)this->focused_index + 1);

        if (this->focused_index == this->columns_window.max_items_in_win) {
          this->focused_index = this->columns_window.max_items_in_win - 1;
          this->columns_window.scroll_down();
        } else
          this->columns_window.draw();
      }
    }

    break;
  }
  case 'C': {
    // create a column
    string column_title = this->create_input_window(" New Column Name ");

    if (column_title.length() > 0) {
      this->data_manager->create_column(this->board, column_title);

      this->columns = {};
      for (size_t i = 0; i < this->board->columns.size(); ++i) {
        // -4 because of 2 spaces between
        // each pair of shown columns
        int width = (this->width - 4) / 3;
        this->columns.push_back(ColumnWin(this->height, width, this->start_y,
                                          &this->board->columns[i]));
      }
      this->columns_count = this->columns.size();

      // focus the just created column
      this->focused_index =
          min((size_t)this->columns_window.max_items_in_win - 1,
              this->columns_count - 1);
      this->columns_window.scroll_to_bottom();
    }

    break;
  }
  case 'E':
  case 'r': {
    // edit title of highlighted column
    if (this->columns_count > 0) {
      size_t col_to_rename_index =
          (this->columns_window.window_start - this->columns.begin()) +
          this->focused_index;
      string new_title = this->create_input_window(
          " Rename Column ", this->board->columns[col_to_rename_index].title);

      if (new_title.length() > 0) {
        this->data_manager->rename_column(this->board, col_to_rename_index,
                                          new_title);
        this->columns_window.draw();
      }
    }

    break;
  }
  case 'D': {
    // delete highlighted column
    if (this->columns_count > 0) {
      size_t col_to_del_index =
          (this->columns_window.window_start - this->columns.begin()) +
          this->focused_index;
      this->data_manager->delete_column(this->board, col_to_del_index);

      this->columns = {};
      for (size_t i = 0; i < this->board->columns.size(); ++i) {
        // -4 because of 2 spaces between
        // each pair of shown columns
        int width = (this->width - 4) / 3;
        this->columns.push_back(ColumnWin(this->height, width, this->start_y,
                                          &this->board->columns[i]));
      }
      this->columns_count = this->columns.size();

      this->focused_index =
          min((size_t)this->focused_index, this->columns_count - 1);
      this->columns_window.scroll_to_top();
    }

    break;
  }
  }

  return false;
}

string BoardScreen::create_input_window(string title, string content) {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  int height = 3;
  int width = max_x * 0.4;

  int start_y = (max_y / 2) - (height / 2);
  int start_x = (max_x / 2) - (width / 2);

  Input input_bar(height, width, start_y, start_x, content, title);
  string input = input_bar.show();

  this->columns_window.draw();

  return input;
}
