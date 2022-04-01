#include "ColumnWin.h"
#include "../../../helpers/consts.h"
#include "../../helpers/win_center_text/win_center_text.h"

ColumnWin::ColumnWin(int height, int width, int start_y, Column *column)
    : height{height}, width{width}, start_y{start_y}, start_x{0},
      cards_window{this->height,
                   this->width,
                   this->start_y,
                   this->start_x,
                   &this->cards,
                   &this->cards_count,
                   bind(&ColumnWin::draw_cards, this, placeholders::_1,
                        placeholders::_2),
                   (this->height - 2) / 3} {
  this->window = newwin(height, width, start_y, 0);
  refresh();

  this->column = column;
  this->update_cards();

  this->focused_index = 0;
  this->cards_window_offset = 0;
}

void ColumnWin::update_cards() {
  this->cards = {};
  for (size_t i = 0; i < this->column->cards.size(); ++i)
    this->cards.push_back(CardWin(3, this->width - 2, &this->column->cards[i]));
  this->cards_count = this->cards.size();
}

void ColumnWin::show(int start_x) {
  this->start_x = start_x;

  this->cards_window = ScrollableWindow<CardWin>(
      this->height, this->width, this->start_y, this->start_x, &this->cards,
      &this->cards_count,
      bind(&ColumnWin::draw_cards, this, placeholders::_1, placeholders::_2),
      (this->height - 2) / 3);

  // relocate column window
  mvwin(this->window, this->start_y, this->start_x);

  // border
  box(this->window, 0, 0);

  // column title
  string column_title = " " + this->column->title + " ";
  int center_x = win_center_x(this->window, &column_title);
  mvwprintw(this->window, 0, center_x, "%s", column_title.c_str());

  wrefresh(this->window);

  this->cards_window.scroll_to_offset(this->cards_window_offset);
}

void ColumnWin::draw_cards(vector<CardWin> shown_cards,
                           WINDOW *scrollable_window) {
  if (shown_cards.size() > 0) {
    for (size_t i = 0; i < shown_cards.size(); ++i) {
      // +1 to skip the column window upper border
      int start_y = this->start_y + (i * shown_cards[i].height + 1);
      shown_cards[i].show(start_y, this->start_x + 1);
    }
  } else {
    string create_card_hint = "c to create a new card";
    int center_x = win_center_x(scrollable_window, &create_card_hint);
    mvwprintw(this->window, 1, center_x, "%s", create_card_hint.c_str());
  }

  wrefresh(this->window);
}

void ColumnWin::focus() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  string column_title = " " + this->column->title + " ";
  int center_x = win_center_x(this->window, &column_title);
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_FOOTER));
  mvwprintw(this->window, 0, center_x, "%s", column_title.c_str());
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_FOOTER));

  wrefresh(this->window);

  this->focus_current();
}

void ColumnWin::unfocus() {
  box(this->window, 0, 0);

  string column_title = " " + this->column->title + " ";
  int center_x = win_center_x(this->window, &column_title);
  mvwprintw(this->window, 0, center_x, "%s", column_title.c_str());

  wrefresh(this->window);

  this->setup_cards_window();
  vector<CardWin> shown_cards = this->cards_window.get_current_window();

  for (size_t i = 0; i < shown_cards.size(); ++i)
    shown_cards[i].unfocus();
}

void ColumnWin::focus_current() {
  if (this->cards_count > 0) {
    this->focused_index =
        min((size_t)this->focused_index, this->cards_count - 1);

    this->setup_cards_window();
    vector<CardWin> shown_cards = this->cards_window.get_current_window();

    for (size_t i = 0; i < shown_cards.size(); ++i)
      shown_cards[i].unfocus();

    shown_cards[this->focused_index].focus();
  }
}

void ColumnWin::setup_cards_window() {
  this->cards_window = ScrollableWindow<CardWin>(
      this->height, this->width, this->start_y, this->start_x, &this->cards,
      &this->cards_count,
      bind(&ColumnWin::draw_cards, this, placeholders::_1, placeholders::_2),
      (this->height - 2) / 3);

  this->cards_window.scroll_to_offset(this->cards_window_offset, false);
}

void ColumnWin::focus_prev() {
  if (this->cards_count > 0) {
    this->setup_cards_window();
    if (--this->focused_index == -1) {
      this->focused_index = 0;
      this->cards_window_offset = max(0, (int)this->cards_window_offset - 1);
      this->cards_window.scroll_up();
    }
    this->focus_current();
  }
}

void ColumnWin::focus_next() {
  if (this->cards_count > 0) {
    this->setup_cards_window();
    this->focused_index =
        min(this->cards_count - 1, (size_t)this->focused_index + 1);

    if (this->focused_index == this->cards_window.max_items_in_win) {
      this->focused_index = this->cards_window.max_items_in_win - 1;
      this->cards_window_offset =
          min(this->cards_count - this->cards_window.max_items_in_win,
              this->cards_window_offset + 1);
      this->cards_window.scroll_down();
    }
    this->focus_current();
  }
}

void ColumnWin::focus_first() {
  if (this->cards_count > 0) {
    this->setup_cards_window();
    this->focused_index = 0;
    this->cards_window_offset = 0;
    this->cards_window.scroll_to_top();
    this->focus_current();
  }
}

void ColumnWin::focus_last() {
  if (this->cards_count > 0) {
    this->setup_cards_window();
    this->focused_index = min(this->cards_count - 1,
                              (size_t)this->cards_window.max_items_in_win - 1);
    this->cards_window_offset =
        max((int)this->cards_count - this->cards_window.max_items_in_win, 0);
    this->cards_window.scroll_to_bottom();
    this->focus_current();
  }
}

void ColumnWin::move_focused_card_up(DataManager *data_manager) {
  if (this->cards_count > 0) {
    this->setup_cards_window();

    size_t focused_card_index = this->get_absolute_focused_index();
    bool moved = data_manager->move_card_up(this->column, focused_card_index);

    if (moved) {
      this->update_cards();

      this->cards_window.scroll_to_offset(this->cards_window_offset);

      this->focus_prev();
    }
  }
}

void ColumnWin::move_focused_card_down(DataManager *data_manager) {
  if (this->cards_count > 0) {
    this->setup_cards_window();

    size_t focused_card_index = this->get_absolute_focused_index();
    bool moved = data_manager->move_card_down(this->column, focused_card_index);

    if (moved) {
      this->update_cards();

      this->cards_window.scroll_to_offset(this->cards_window_offset);

      this->focus_next();
    }
  }
}

size_t ColumnWin::get_absolute_focused_index() {
  return this->cards_window_offset + this->focused_index;
}
