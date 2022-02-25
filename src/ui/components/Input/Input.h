#pragma once

#include <ncurses.h>
#include <string>

#include "../ScrollableWindow/ScrollableWindow.h"

using namespace std;

class Input {
public:
  Input(int height, int width, int start_y, int start_x, string content = "",
        string title = "");

  string show();

  bool handle_key_press(char key);

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  void draw_content(vector<char> shown_content, WINDOW *scrollable_window);

  bool normal_mode_key_handler(char key);
  bool insert_mode_key_handler(char key);

  void move_to_start();
  void move_to_end();
  void move_cursor_left();
  void move_cursor_right();
  void change_mode(int mode);

  int cursor_y;
  int cursor_x;

  int mode;

  string title;

  vector<char> content_chars;
  size_t content_size;
  ScrollableWindow<char> content_window;
};
