#include "Input.h"
#include "../../../helpers/consts.h"
#include "../../../helpers/trim_spaces/trim_spaces.h"

Input::Input(int height, int width, int start_y, int start_x, string content,
             string title, bool focused)
    : height{height}, width{width}, start_y{start_y}, start_x{start_x},
      content_window{
          this->height,
          this->width,
          this->start_y,
          this->start_x,
          &this->content,
          &this->content_size,
          bind(&Input::draw_content, this, placeholders::_1, placeholders::_2),
          this->height - 2},
      first_line_window{this->height,
                        this->width,
                        this->start_y,
                        this->start_x,
                        &this->first_line_chars,
                        &this->first_line_size,
                        bind(&Input::draw_first_line, this, placeholders::_1,
                             placeholders::_2),
                        this->width - 3} {
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);
  this->cursor_y = 1;
  this->cursor_x = 1;

  this->multi_row = this->height > 3;

  if (this->multi_row) {
    // construct multi-row content
    this->content = {};
    this->line_has_newline = {};

    if (content.length() == 0) {
      this->content.push_back("");
      this->line_has_newline.push_back(true);
    } else {
      string curr_line = "";
      for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] != '\n')
          curr_line += content[i];

        if (content[i] == '\n' ||
            curr_line.length() == (size_t)this->width - 2 ||
            i == content.length() - 1) {
          this->content.push_back(curr_line);
          this->line_has_newline.push_back(content[i] == '\n');
          curr_line = "";
        }
      }
    }

    this->content_size = this->content.size();
  } else {
    this->first_line_chars = {};
    for (size_t i = 0; i < content.length(); ++i)
      this->first_line_chars.push_back(content[i]);
    this->first_line_size = this->first_line_chars.size();
  }

  this->focused = focused;
  this->title = title;
  this->mode = MODE_NORMAL;
}

void Input::show(bool grab_input, bool just_redraw) {
  // show cursor
  curs_set(1);

  if (this->multi_row) {
    if (just_redraw)
      this->content_window.draw();
    else
      this->content_window.scroll_to_top();
  } else {
    if (just_redraw)
      this->first_line_window.draw();
    else
      this->first_line_window.scroll_to_top();
  }

  if (grab_input) {
    char key;
    bool done = false;
    while (!done && (key = wgetch(this->window))) {
      done = this->handle_key_press(key);
    }

    this->clean_up();
  }
}

void Input::clean_up() {
  curs_set(0);
  werase(this->window);
  wrefresh(this->window);
  this->change_mode(MODE_NORMAL);
}

string Input::get_value() {
  string content = "";

  if (this->multi_row) {
    for (size_t i = 0; i < this->content.size(); ++i) {
      content += this->content[i];

      if (this->content[i].length() < (size_t)this->width - 2 &&
          i < this->content.size() - 1)
        content += '\n';
    }
  } else {
    for (size_t i = 0; i < this->first_line_chars.size(); ++i)
      content += this->first_line_chars[i];
  }

  return trim_spaces(content);
}

void Input::draw_content(vector<string> shown_content,
                         [[maybe_unused]] WINDOW *scrollable_window) {
  werase(this->window);

  if (shown_content.size() > 0) {
    for (size_t i = 0; i < shown_content.size(); ++i)
      mvwprintw(this->window, i + 1, 1, "%s", shown_content[i].c_str());
  }

  if (this->focused)
    this->focus();
  else
    this->unfocus();

  wrefresh(this->window);
}

void Input::draw_first_line(vector<char> shown_content,
                            [[maybe_unused]] WINDOW *scrollable_window) {
  werase(this->window);

  if (this->focused)
    this->focus();
  else
    this->unfocus();

  if (shown_content.size() > 0) {
    string content = "";
    for (size_t i = 0; i < shown_content.size(); ++i)
      content += shown_content[i];

    mvwprintw(this->window, 1, 1, "%s", content.c_str());
  }

  wmove(this->window, this->cursor_y, this->cursor_x);
  wrefresh(this->window);
}

void Input::focus() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  mvwprintw(this->window, 0, 1, "%s", this->title.c_str());
  wmove(this->window, this->cursor_y, this->cursor_x);

  wrefresh(this->window);

  this->focused = true;
}

void Input::unfocus() {
  box(this->window, 0, 0);
  mvwprintw(this->window, 0, 1, "%s", this->title.c_str());
  wrefresh(this->window);

  this->change_mode(MODE_NORMAL);

  this->focused = false;
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
    if (this->multi_row) {
      this->content = {};
      this->content_size = 0;
    } else {
      this->first_line_chars = {};
      this->first_line_size = 0;
    }
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
    this->move_cursor_to_first_char();

    break;
  case '$':
    this->move_cursor_to_last_char();

    break;
  case 'k':
    this->move_cursor_up();

    break;
  case 'j':
    this->move_cursor_down();

    break;
  case 'g':
    this->move_cursor_to_first_line();

    break;
  case 'G':
    this->move_cursor_to_last_line();

    break;
  case 'i':
    this->change_mode(MODE_INSERT);

    break;
  case 'a':
    this->change_mode(MODE_INSERT);

    if (this->multi_row) {
      size_t prev_offset =
          this->content_window.window_start - this->content.begin();

      if (this->cursor_x == this->width - 2) {
        if (this->line_has_newline[this->curr_line_index()] ||
            this->curr_line_index() == this->content_size - 1) {
          this->line_has_newline[this->curr_line_index()] = false;

          this->content.insert(
              this->content_window.window_start + this->cursor_y, "");
          this->content_size++;

          this->content_window.scroll_to_offset(prev_offset);

          this->line_has_newline.insert(this->line_has_newline.begin() +
                                            (prev_offset + this->cursor_y),
                                        true);

          this->move_cursor_down();
        } else {
          this->move_cursor_to_first_char();
          this->move_cursor_down();
        }
      } else
        this->move_cursor_right();
    } else
      this->move_cursor_right();

    break;
  case 'I':
    this->move_cursor_to_first_char();
    this->change_mode(MODE_INSERT);

    break;
  case 'A':
    this->move_cursor_to_last_char();
    this->normal_mode_key_handler('a');

    break;
  case 'S':
    if (this->multi_row) {
      if (this->content_size > 0) {
        this->content[this->curr_line_index()] = "";
        this->content_window.draw();
      }
    } else {
      this->first_line_chars = {};
      this->first_line_size = 0;
      this->first_line_window.window_start = this->first_line_chars.begin();
      this->first_line_window.window_end = this->first_line_chars.end();
    }
    this->move_cursor_to_first_char();
    this->change_mode(MODE_INSERT);

    break;
  case 'd':
    if (this->multi_row && this->content_size > 0) {
      size_t prev_offset =
          this->content_window.window_start - this->content.begin();

      this->content.erase(this->content_window.window_start + this->cursor_y -
                          1);
      this->content_size--;
      this->line_has_newline.erase(this->line_has_newline.begin() +
                                   this->curr_line_index());

      if (this->content_size == 0) {
        this->content.push_back("");
        this->content_size++;
        this->line_has_newline.push_back(true);
      }

      this->cursor_y =
          max(min((size_t)this->cursor_y, this->content_size), (size_t)1);
      this->cursor_x =
          content_size == 0
              ? 1
              : max(min((size_t)this->cursor_x,
                        this->content[this->curr_line_index()].length()),
                    (size_t)1);

      this->content_window.scroll_to_offset(prev_offset);
    }

    break;
  }

  return false;
}

bool Input::insert_mode_key_handler(char key) {
  switch (key) {
  case 27:
    // ESC
    this->move_cursor_left();
    this->change_mode(MODE_NORMAL);

    break;
  case '\n':
    // ENTER
    if (this->multi_row) {
      size_t prev_offset =
          this->content_window.window_start - this->content.begin();

      string content_on_newline = this->content[this->curr_line_index()].substr(
          this->cursor_x - 1, this->content[this->curr_line_index()].length() -
                                  (this->cursor_x - 1));

      this->content[this->curr_line_index()].erase(
          this->cursor_x - 1, this->content[this->curr_line_index()].length() -
                                  (this->cursor_x - 1));

      this->line_has_newline[this->curr_line_index()] = true;

      this->content.insert(this->content_window.window_start + this->cursor_y,
                           content_on_newline);
      this->content_size++;

      this->content_window.scroll_to_offset(prev_offset);

      this->line_has_newline.insert(
          this->line_has_newline.begin() + (this->curr_line_index() + 1), true);

      this->move_cursor_to_first_char();
      this->move_cursor_down();
    } else {
      this->change_mode(MODE_NORMAL);
      return true;
    }

    break;
  case '\b':
  case 127:
    // Backspace / Delete

    if (this->multi_row) {
      if (this->content_size > 0) {
        size_t prev_offset =
            this->content_window.window_start - this->content.begin();

        size_t n_of_del_chars = 0;
        bool moved_up = false;

        if (this->cursor_x - 1 > 0) {
          this->content[this->curr_line_index()].erase(
              this->content[this->curr_line_index()].begin() + this->cursor_x -
              2);
          n_of_del_chars = 1;
          this->move_cursor_left();
        } else {
          if (this->curr_line_index() > 0) {
            if (this->line_has_newline[this->curr_line_index() - 1]) {
              this->line_has_newline[this->curr_line_index() - 1] = false;

              n_of_del_chars =
                  (this->width - 2) -
                  this->content[this->curr_line_index() - 1].length();

              if (n_of_del_chars > 0) {
                this->move_cursor_up();
                this->move_cursor_to_last_char();
                moved_up = true;
              }
            } else {
              this->content[this->curr_line_index() - 1].erase(
                  this->content[this->curr_line_index() - 1].begin() +
                  (this->content[this->curr_line_index() - 1].length() - 1));
              n_of_del_chars = 1;

              this->move_cursor_up();
              this->move_cursor_to_last_char();
              moved_up = true;
            }
          }
        }

        if (n_of_del_chars > 0) {
          for (size_t i = this->curr_line_index(); i < this->content_size - 1;
               ++i) {
            n_of_del_chars = (this->width - 2) - this->content[i].length();

            if (this->line_has_newline[i])
              break;

            string extra_string =
                this->content[i + 1].substr(0, n_of_del_chars);

            this->content[i] += extra_string;

            this->content[i + 1].erase(
                this->content[i + 1].begin(),
                this->content[i + 1].begin() +
                    (min(this->content[i + 1].length(), n_of_del_chars)));

            if (this->content[i + 1].length() == 0) {
              this->line_has_newline[i] = this->line_has_newline[i + 1];

              this->content.erase(this->content.begin() + i + 1);
              this->content_size--;

              this->line_has_newline.erase(this->line_has_newline.begin() + i +
                                           1);
            }
          }

          if (this->content_window.get_current_window().size() >=
                  (size_t)this->content_window.max_items_in_win &&
              prev_offset > 0 && moved_up) {
            this->content_window.scroll_up();
            this->move_cursor_down();
          }

          this->content_window.draw();
        }
      }
    } else {
      if (this->first_line_size > 0) {
        vector<char>::iterator del_it =
            this->first_line_window.window_start + (this->cursor_x - 2);

        if (del_it - this->first_line_chars.begin() >= 0) {
          size_t prev_offset = this->first_line_window.window_start -
                               this->first_line_chars.begin();

          // remove character before cursor
          this->first_line_chars.erase(del_it);
          --this->first_line_size;

          this->first_line_window.scroll_to_offset(prev_offset);

          // update cursor position
          if (this->first_line_window.get_current_window().size() >=
                  (size_t)this->first_line_window.max_items_in_win &&
              prev_offset > 0) {
            this->first_line_window.scroll_up();
          } else
            this->move_cursor_left();
        }
      }
    }

    break;
  default:
    if (this->multi_row) {
      size_t prev_offset =
          this->content_window.window_start - this->content.begin();

      this->content[this->curr_line_index()].insert(
          this->content[this->curr_line_index()].begin() + (this->cursor_x - 1),
          key);

      this->content_window.scroll_to_offset(prev_offset);

      // handling content overflow
      for (size_t i = this->curr_line_index(); i < this->content.size(); ++i) {
        if (this->content[i].length() > (size_t)this->width - 2) {
          char extra_char = this->content[i][this->content[i].length() - 1];
          this->content[i].pop_back();

          if (i == this->content_size - 1) {
            this->content.push_back({extra_char});
            this->content_size++;
            this->line_has_newline.push_back(false);
          } else {
            if (!this->line_has_newline[i]) {
              this->content[i + 1].insert(this->content[i + 1].begin(),
                                          extra_char);
            } else {
              this->content.insert(this->content.begin() + i + 1, {extra_char});
              this->content_size++;
              this->line_has_newline[i] = false;
              this->line_has_newline.insert(
                  this->line_has_newline.begin() + i + 1, true);
            }
          }
        }
      }

      this->content_window.scroll_to_offset(prev_offset);

      if (this->cursor_x == this->width - 2) {
        if (this->line_has_newline[this->curr_line_index()] ||
            this->curr_line_index() == this->content_size - 1) {
          this->line_has_newline[this->curr_line_index()] = false;
          this->content.insert(
              this->content.begin() + this->curr_line_index() + 1, "");
          this->content_size++;

          this->content_window.scroll_to_offset(prev_offset);

          this->line_has_newline.insert(this->line_has_newline.begin() +
                                            (this->curr_line_index() + 1),
                                        true);

          this->move_cursor_down();
        } else {
          this->move_cursor_to_first_char();
          this->move_cursor_down();
        }
      } else
        this->move_cursor_right();
    } else {
      size_t prev_offset =
          this->first_line_window.window_start - this->first_line_chars.begin();

      // insert character
      this->first_line_chars.insert(
          this->first_line_window.window_start + (this->cursor_x - 1), key);
      ++this->first_line_size;

      this->first_line_window.scroll_to_offset(prev_offset);
      this->move_cursor_right();
    }

    break;
  }

  return false;
}

void Input::move_cursor_left() {
  if (--this->cursor_x < 1) {
    this->cursor_x = 1;
    if (!this->multi_row)
      this->first_line_window.scroll_up();
  } else {
    wmove(this->window, this->cursor_y, this->cursor_x);
    wrefresh(this->window);
  }
}

void Input::move_cursor_right() {
  if ((this->content_size > 0 && this->multi_row) ||
      (this->first_line_size > 0 && !this->multi_row)) {
    if (this->multi_row)
      this->cursor_x = max(min(this->content[this->curr_line_index()].length() +
                                   (this->mode == MODE_INSERT ? 1 : 0),
                               (size_t)this->cursor_x + 1),
                           (size_t)1);
    else
      this->cursor_x =
          min(this->first_line_size + 1, (size_t)this->cursor_x + 1);

    if (this->cursor_x == this->first_line_window.max_items_in_win + 2) {
      this->cursor_x = this->first_line_window.max_items_in_win + 1;
      if (!this->multi_row)
        this->first_line_window.scroll_down();
    } else {
      wmove(this->window, this->cursor_y, this->cursor_x);
      wrefresh(this->window);
    }
  }
}

void Input::move_cursor_up() {
  if (this->multi_row && this->content_size > 0) {
    if (--this->cursor_y == 0) {
      this->cursor_y = 1;
      this->content_window.scroll_up();
    }
    this->cursor_x = max(min(this->content[this->curr_line_index()].length(),
                             (size_t)this->cursor_x),
                         (size_t)1);
    wmove(this->window, this->cursor_y, this->cursor_x);
    wrefresh(this->window);
  }
}

void Input::move_cursor_down() {
  if (this->multi_row && this->content_size > 0) {
    this->cursor_y =
        max(min(this->content_size, (size_t)this->cursor_y + 1), (size_t)1);

    if (this->cursor_y == this->content_window.max_items_in_win + 1) {
      this->cursor_y = this->content_window.max_items_in_win;
      this->content_window.scroll_down();
    }
    this->cursor_x = max(min(this->content[this->curr_line_index()].length(),
                             (size_t)this->cursor_x),
                         (size_t)1);
    wmove(this->window, this->cursor_y, this->cursor_x);
    wrefresh(this->window);
  }
}

void Input::move_cursor_to_first_char() {
  this->cursor_x = 1;
  wmove(this->window, this->cursor_y, this->cursor_x);
  wrefresh(this->window);

  if (!this->multi_row)
    this->first_line_window.scroll_to_top();
}

void Input::move_cursor_to_last_char() {
  if ((this->content_size > 0 && this->multi_row) ||
      (this->first_line_size > 0 && !this->multi_row)) {
    if (this->multi_row) {
      this->cursor_x = max(this->content[this->curr_line_index()].length() +
                               (this->mode == MODE_INSERT ? 1 : 0),
                           (size_t)1);
      wmove(this->window, this->cursor_y, this->cursor_x);
      wrefresh(this->window);
    } else {
      this->cursor_x =
          min(this->first_line_size + 1,
              (size_t)this->first_line_window.max_items_in_win + 1);
      this->first_line_window.scroll_to_bottom();
    }
  }
}

void Input::move_cursor_to_first_line() {
  if (this->multi_row && this->content_size > 0) {
    this->cursor_y = 1;
    this->cursor_x =
        max(min(this->content[0].length(), (size_t)this->cursor_x), (size_t)1);
    this->content_window.scroll_to_top();
  }
}

void Input::move_cursor_to_last_line() {
  if (this->multi_row && this->content_size > 0) {
    this->cursor_y = max(
        min(this->content_size, (size_t)this->content_window.max_items_in_win),
        (size_t)1);
    this->cursor_x = max(min(this->content[this->content.size() - 1].length(),
                             (size_t)this->cursor_x),
                         (size_t)1);
    this->content_window.scroll_to_bottom();
  }
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
  wrefresh(this->window);
}

size_t Input::curr_line_index() {
  return this->content_window.window_start - this->content.begin() +
         this->cursor_y - 1;
}
