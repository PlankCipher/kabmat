#include "Board.h"
#include "../../components/Footer/Footer.h"
#include "../../components/Header/Header.h"
#include "../../components/Help/Help.h"
#include "../../helpers/win_center_text/win_center_text.h"

BoardScreen::BoardScreen(string board_name, DataManager *data_manager,
                         bool from_tui)
    : height{getmaxy(stdscr) - 4}, width{getmaxx(stdscr) - 2}, start_y{2},
      start_x{1}, columns_window{ScrollableWindow<ColumnWin>(
                      this->height, this->width, this->start_y, this->start_x,
                      &this->columns, &this->columns_count,
                      bind(&BoardScreen::draw_columns, this, placeholders::_1,
                           placeholders::_2),
                      3)} {
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);
  refresh();

  this->data_manager = data_manager;
  this->board = this->data_manager->get_board_if_exists(board_name);

  for (size_t i = 0; i < this->board->columns.size(); ++i) {
    int width = (this->width - 4) / 3;
    this->columns.push_back(ColumnWin(this->height, width, this->start_y,
                                      &this->board->columns[i]));
  }
  this->columns_count = this->columns.size();
  this->focused_index = 0;

  this->from_tui = from_tui;
}

void BoardScreen::show() {
  Header header(this->board->name);
  header.show();

  Footer footer(false, true);
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
    if (this->from_tui)
      return true;

    endwin();
    exit(0);

    break;
  }
  case '?': {
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
  }

  return false;
}
