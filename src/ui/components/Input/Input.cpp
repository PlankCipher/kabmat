#include "Input.h"
#include "../../../helpers/consts.h"
#include "../../../helpers/remove_trim_spaces/remove_trim_spaces.h"

Input::Input(int height, int width, int start_y, int start_x, string content,
             string title) {
  this->content = content;
  this->title = title;

  this->start_y = start_y;
  this->start_x = start_x;
  this->height = height;
  this->width = width;
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);

  this->update_ends();

  curs_set(1);
}

string Input::show() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  mvwprintw(this->window, 0, 1, "%s", this->title.c_str());
  mvwprintw(this->window, 1, 1, "%s", this->content.c_str());

  refresh();
  wrefresh(this->window);

  char key;
  bool done = false;
  while (!done && (key = wgetch(this->window))) {
    werase(this->window);

    wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
    box(this->window, 0, 0);
    wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

    mvwprintw(this->window, 0, 1, "%s", this->title.c_str());

    done = this->handle_key_press(key);

    string shown_content =
        this->content.substr(this->left_end, this->right_end);
    mvwprintw(this->window, 1, 1, "%s", shown_content.c_str());

    refresh();
    wrefresh(this->window);
  }

  curs_set(0);
  werase(this->window);
  refresh();
  wrefresh(this->window);

  return remove_trim_spaces(this->content);
}

bool Input::handle_key_press(char key) {
  bool done = false;

  switch (key) {
  case '\b':
  case 127:
    // BACKSPACE / DELETE
    this->content = this->content.substr(0, this->content.length() - 1);

    break;
  case '\n':
    // ENTER
    done = true;

    break;
  case 27:
    // ESC
    this->content = "";
    done = true;

    break;
  default:
    this->content += key;

    break;
  }

  this->update_ends();
  return done;
}

void Input::update_ends() {
  this->right_end = this->content.length();
  this->left_end =
      this->right_end - min((size_t)this->width - 3, this->content.length());
}
