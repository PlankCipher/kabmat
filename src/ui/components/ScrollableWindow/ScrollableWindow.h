#pragma once

#include <functional>
#include <ncurses.h>
#include <vector>

using namespace std;

template <typename T> class ScrollableWindow {
public:
  ScrollableWindow(int height, int width, int start_y, int start_x,
                   vector<T> *items, size_t *items_count,
                   function<void(vector<T>, WINDOW *)> draw_callback,
                   int max_items_in_win = 0) {
    this->height = height;
    this->width = width;
    this->start_y = start_y;
    this->start_x = start_x;

    this->window =
        newwin(this->height, this->width, this->start_y, this->start_x);

    this->items = items;
    this->items_count = items_count;
    this->max_items_in_win =
        max_items_in_win == 0 ? this->height : max_items_in_win;
    this->draw_callback = draw_callback;
  }

  void draw() {
    werase(this->window);

    if (*this->items_count > 0) {
      vector<T> shown_items = vector<T>(this->window_start, this->window_end);
      this->draw_callback(shown_items, this->window);
    } else
      this->draw_callback({}, this->window);
  }

  void scroll_up(bool redraw = true) {
    if (*this->items_count > 0) {
      this->window_start = max(this->items->begin(), this->window_start - 1);
      this->window_end =
          this->window_start +
          min(*this->items_count, (size_t)this->max_items_in_win);
    }

    if (redraw)
      this->draw();
  }

  void scroll_down(bool redraw = true) {
    if (*this->items_count > 0) {
      this->window_end = min(this->items->end(), this->window_end + 1);
      this->window_start =
          this->window_end -
          min(*this->items_count, (size_t)this->max_items_in_win);
    }

    if (redraw)
      this->draw();
  }

  void scroll_to_top(bool redraw = true) {
    if (*this->items_count > 0) {
      this->window_start = this->items->begin();
      this->window_end =
          this->window_start +
          min(*this->items_count, (size_t)this->max_items_in_win);
    }

    if (redraw)
      this->draw();
  }

  void scroll_to_bottom(bool redraw = true) {
    if (*this->items_count > 0) {
      this->window_end = this->items->end();
      this->window_start =
          this->window_end -
          min(*this->items_count, (size_t)this->max_items_in_win);
    }

    if (redraw)
      this->draw();
  }

  void scroll_to_offset(size_t offset, bool redraw = true) {
    if (*this->items_count > 0) {
      this->window_start =
          this->items->begin() +
          min(offset, *this->items_count - this->max_items_in_win);
      this->window_end =
          this->window_start +
          min(*this->items_count, (size_t)this->max_items_in_win);
    }

    if (redraw)
      this->draw();
  }

  vector<T> get_current_window() {
    return vector<T>(this->window_start, this->window_end);
  }

  vector<T> *items;
  size_t *items_count;
  int max_items_in_win;

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

  typename vector<T>::iterator window_start;
  typename vector<T>::iterator window_end;

private:
  function<void(vector<T>, WINDOW *)> draw_callback;
};
