#include "Input.h"
#include "../../../helpers/consts.h"
#include "../../../helpers/remove_trim_spaces/remove_trim_spaces.h"

Input::Input(int height, int width, int start_y, int start_x, string content,
             string title)
    : height{height}, width{width}, start_y{start_y}, start_x{start_x},
      content_window{
          this->height,
          this->width,
          this->start_y,
          this->start_x,
          &this->content_chars,
          &this->content_size,
          bind(&Input::draw_content, this, placeholders::_1, placeholders::_2),
          this->width - 3} {
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);

  this->content_chars = {};
  for (size_t i = 0; i < content.length(); ++i)
    this->content_chars.push_back(content[i]);
  this->content_size = this->content_chars.size();

  this->cursor_y = 1;
  this->cursor_x = min(this->content_size + 1,
                       (size_t)this->content_window.max_items_in_win + 1);

  this->title = title;

  this->mode = MODE_NORMAL;

  curs_set(1);
}

string Input::show() {
  this->content_window.scroll_to_bottom();

  char key;
  bool done = false;
  while (!done && (key = wgetch(this->window))) {
    done = this->handle_key_press(key);
  }

  // clean up
  curs_set(0);
  werase(this->window);
  wrefresh(this->window);

  string content = "";
  for (size_t i = 0; i < this->content_chars.size(); ++i)
    content += this->content_chars[i];

  return remove_trim_spaces(content);
}

void Input::draw_content(vector<char> shown_content,
                         [[maybe_unused]] WINDOW *scrollable_window) {
  werase(this->window);

  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  mvwprintw(this->window, 0, 1, "%s", this->title.c_str());

  if (shown_content.size() > 0) {
    string content = "";
    for (size_t i = 0; i < shown_content.size(); ++i)
      content += shown_content[i];

    mvwprintw(this->window, 1, 1, "%s", content.c_str());
  }

  wmove(this->window, this->cursor_y, this->cursor_x);

  wrefresh(this->window);
}

bool Input::handle_key_press(char key) {
  switch (this->mode) {
  case MODE_NORMAL:
    return this->normal_mode_key_handler(key);
  case MODE_INSERT:
    return this->insert_mode_key_handler(key);
  }

  return false;
}

bool Input::normal_mode_key_handler(char key) {
  switch (key) {
  case 27: // ESC
  case 'q':
    // cancel
    this->content_chars = {};
    this->content_size = 0;
    return true;

    break;
  case '\n':
    // ENTER
    return true;

    break;
  case 'h':
    this->move_cursor_left();

    break;
  case 'l':
    this->move_cursor_right();

    break;
  case '0':
    this->move_to_start();

    break;
  case '$':
    this->move_to_end();

    break;
  case 'i':
    this->change_mode(MODE_INSERT);

    break;
  case 'a':
    this->move_cursor_right();
    this->change_mode(MODE_INSERT);

    break;
  case 'I':
    this->move_to_start();
    this->change_mode(MODE_INSERT);

    break;
  case 'A':
    this->move_to_end();
    this->change_mode(MODE_INSERT);

    break;
  case 'S':
    this->content_chars = {};
    this->content_size = 0;
    this->content_window.window_start = this->content_chars.begin();
    this->content_window.window_end = this->content_chars.end();
    this->move_to_start();
    this->change_mode(MODE_INSERT);

    break;
  }

  return false;
}

bool Input::insert_mode_key_handler(char key) {
  switch (key) {
  case 27:
    // ESC
    this->change_mode(MODE_NORMAL);

    break;
  case '\n':
    // ENTER
    this->change_mode(MODE_NORMAL);
    return true;

    break;
  case '\b':
  case 127:
    // BACKSPACE / DELETE
    if (this->content_size > 0) {
      vector<char>::iterator del_it =
          this->content_window.window_start + (this->cursor_x - 2);

      if (del_it - this->content_chars.begin() >= 0) {
        size_t prev_offset =
            this->content_window.window_start - this->content_chars.begin();

        // remove character before cursor
        this->content_chars.erase(del_it);
        --this->content_size;

        this->content_window.scroll_to_offset(prev_offset);

        // update cursor position
        if (this->content_window.get_current_window().size() >=
                (size_t)this->content_window.max_items_in_win &&
            prev_offset > 0) {
          this->content_window.scroll_up();
        } else
          this->move_cursor_left();
      }
    }

    break;
  default:
    size_t prev_offset =
        this->content_window.window_start - this->content_chars.begin();

    // insert character into content
    this->content_chars.insert(
        this->content_window.window_start + (this->cursor_x - 1), key);
    ++this->content_size;

    this->content_window.scroll_to_offset(prev_offset);
    this->move_cursor_right();

    break;
  }

  return false;
}

void Input::move_to_start() {
  this->cursor_x = 1;
  this->content_window.scroll_to_top();
}

void Input::move_to_end() {
  this->cursor_x = min(this->content_size + 1,
                       (size_t)this->content_window.max_items_in_win + 1);
  this->content_window.scroll_to_bottom();
}

void Input::move_cursor_left() {
  if (--this->cursor_x < 1) {
    this->cursor_x = 1;
    this->content_window.scroll_up();
  } else
    wmove(this->window, this->cursor_y, this->cursor_x);
}

void Input::move_cursor_right() {
  this->cursor_x = min(this->content_size + 1, (size_t)this->cursor_x + 1);

  if (this->cursor_x == this->content_window.max_items_in_win + 2) {
    this->cursor_x = this->content_window.max_items_in_win + 1;
    this->content_window.scroll_down();
  } else
    wmove(this->window, this->cursor_y, this->cursor_x);
}

void Input::change_mode(int mode) {
  this->mode = mode;

  // update footer
  attron(COLOR_PAIR(COLOR_PAIR_MODE));
  mvprintw(getmaxy(stdscr) - 1, 0,
           this->mode == MODE_NORMAL ? " NORMAL " : " INSERT ");
  attroff(COLOR_PAIR(COLOR_PAIR_MODE));

  wmove(this->window, this->cursor_y, this->cursor_x);
  refresh();
}
