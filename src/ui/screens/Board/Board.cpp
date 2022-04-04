#include "Board.h"
#include "../../components/CardInfo/CardInfo.h"
#include "../../components/ConfirmDialog/ConfirmDialog.h"
#include "../../components/Footer/Footer.h"
#include "../../components/Header/Header.h"
#include "../../components/Help/Help.h"
#include "../../helpers/win_center_text/win_center_text.h"

BoardScreen::BoardScreen(string board_name, DataManager *data_manager,
                         Config *config, bool from_tui)
    : height{getmaxy(stdscr) - 4}, width{getmaxx(stdscr) - 2}, start_y{2},
      start_x{1}, columns_window{this->height,
                                 this->width,
                                 this->start_y,
                                 this->start_x,
                                 &this->columns,
                                 &this->columns_count,
                                 bind(&BoardScreen::draw_columns, this,
                                      placeholders::_1, placeholders::_2),
                                 3} {
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
      // +2 for spaces between columns
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
    // move focused card to left/prev column
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
    // move focused card to right/next column
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
    // move focused card up in column
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
    // move focused card down in column
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
  case ('p' & 0x1f):   // ctrl + p
  case ('h' & 0x1f): { // ctrl + h
    // move focused column to the left
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
  case ('n' & 0x1f):   // ctrl + n
  case ('l' & 0x1f): { // ctrl + l
    // move focused column to the right
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
    string column_title =
        this->create_input_window(" New Column Name ", "", true);

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
  case 'E': {
    // edit title of focused column
    if (this->columns_count > 0) {
      size_t col_to_rename_index =
          (this->columns_window.window_start - this->columns.begin()) +
          this->focused_index;
      string new_title = this->create_input_window(
          " Rename Column ", this->board->columns[col_to_rename_index].title,
          true);

      if (new_title.length() > 0) {
        this->data_manager->rename_column(this->board, col_to_rename_index,
                                          new_title);
        this->columns_window.draw();
      }
    }

    break;
  }
  case 'D': {
    // delete focused column
    if (this->columns_count > 0) {
      size_t col_to_del_index =
          (this->columns_window.window_start - this->columns.begin()) +
          this->focused_index;
      string col_to_del_title = this->columns[col_to_del_index].column->title;

      string message_col_title = col_to_del_title.length() > 11
                                     ? col_to_del_title.substr(0, 9) + ".."
                                     : col_to_del_title;
      string message = "Delete column \"" + message_col_title + "\"?";
      bool confirmed = this->create_confirm_dialog(message);

      if (confirmed) {
        size_t prev_offset =
            this->columns_window.window_start - this->columns.begin();
        this->data_manager->delete_column(this->board, col_to_del_index);

        this->setup_columns();

        this->focused_index =
            min((size_t)this->focused_index, this->columns_count - 1);
        this->columns_window.scroll_to_offset(prev_offset);
      }
    }

    break;
  }
  case 'c': {
    // create a card in focused column
    if (this->columns_count > 0) {
      Card card = Card("");
      bool canceled = this->create_card_info_window(&card);

      if (!canceled && card.content != "") {
        Column *column =
            this->columns[this->columns_window.window_start -
                          this->columns.begin() + this->focused_index]
                .column;
        this->data_manager->add_card(column, card);

        this->setup_columns();

        // focus the just created card
        vector<ColumnWin> shown_columns =
            this->columns_window.get_current_window();
        shown_columns[this->focused_index].focus_last();
        *(this->columns_window.window_start + this->focused_index) =
            shown_columns[this->focused_index];

        this->columns_window.draw();
      }
    }

    break;
  }
  case 'e': {
    // edit focused card
    if (this->columns_count > 0) {
      size_t focused_col_index = this->columns_window.window_start -
                                 this->columns.begin() + this->focused_index;
      Column *column = this->columns[focused_col_index].column;

      if (column->cards.size() > 0) {
        size_t focused_card_index =
            this->columns[focused_col_index].get_absolute_focused_index();

        Card *card = &(column->cards[focused_card_index]);

        bool canceled = this->create_card_info_window(card);

        if (!canceled && card->content != "") {
          size_t prev_offset =
              this->columns[focused_col_index].cards_window_offset;
          size_t prev_focused_index =
              this->columns[focused_col_index].focused_index;

          this->data_manager->update_card(column, focused_card_index, *card);

          this->setup_columns();
          this->columns[focused_col_index].cards_window_offset = prev_offset;
          this->columns[focused_col_index].focused_index = prev_focused_index;

          this->columns_window.draw();
        }
      }
    }

    break;
  }
  case 'd': {
    // delete focused card
    if (this->columns_count > 0) {
      size_t focused_col_index = this->columns_window.window_start -
                                 this->columns.begin() + this->focused_index;
      Column *column = this->columns[focused_col_index].column;

      if (column->cards.size() > 0) {
        size_t card_to_del_index =
            this->columns[focused_col_index].get_absolute_focused_index();
        string card_to_del_content = this->columns[focused_col_index]
                                         .column->cards[card_to_del_index]
                                         .content;

        string message_card_content =
            card_to_del_content.length() > 11
                ? card_to_del_content.substr(0, 9) + ".."
                : card_to_del_content;
        string message = "Delete card \"" + message_card_content + "\"?";
        bool confirmed = this->create_confirm_dialog(message);

        if (confirmed) {
          size_t prev_offset =
              this->columns[focused_col_index].cards_window_offset;
          size_t prev_focused_index =
              this->columns[focused_col_index].focused_index;

          this->data_manager->delete_card(column, card_to_del_index);

          this->setup_columns();
          this->columns[focused_col_index].cards_window_offset = prev_offset;
          this->columns[focused_col_index].focused_index =
              min(prev_focused_index,
                  this->columns[focused_col_index].cards_count - 1);

          this->columns_window.draw();
        }
      }
    }

    break;
  }
  }

  return false;
}

string BoardScreen::create_input_window(string title, string content,
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

  this->columns_window.draw();

  return input;
}

bool BoardScreen::create_card_info_window(Card *card) {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  int height = max_y * 0.4;
  int width = max_x * 0.5;
  int start_y = (max_y / 2) - (height / 2);
  int start_x = (max_x / 2) - (width / 2);

  CardInfo card_info_window = CardInfo(height, width, start_y, start_x, card);
  bool canceled = card_info_window.show(this->data_manager);

  this->columns_window.draw();

  return canceled;
}

bool BoardScreen::create_confirm_dialog(string message) {
  int height = 5;
  int width = message.length() + 4;
  int start_y = (getmaxy(stdscr) / 2) - (height / 2);
  int start_x = (getmaxx(stdscr) / 2) - (width / 2);

  ConfirmDialog confirm_dialog(height, width, start_y, start_x, message);
  bool confirmed = confirm_dialog.show();

  this->columns_window.draw();

  return confirmed;
}
