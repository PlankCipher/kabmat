#pragma once

#include <string>

#include "../ScrollableWindow/ScrollableWindow.h"

using namespace std;

class Input {
public:
  Input(int height, int width, int start_y, int start_x, string content = "",
        string title = "", bool focused = false);

  void show(bool grab_input = false, bool just_redraw = false);
  void clean_up();

  void focus();
  void unfocus();

  string get_value();

  bool handle_key_press(char key);

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;
  int mode;

private:
  void draw_content(vector<string> shown_content, WINDOW *scrollable_window);
  void draw_first_line(vector<char> shown_content, WINDOW *scrollable_window);

  bool normal_mode_key_handler(char key);
  bool insert_mode_key_handler(char key);

  void move_cursor_left();
  void move_cursor_right();
  void move_cursor_up();
  void move_cursor_down();
  void move_cursor_to_first_char();
  void move_cursor_to_last_char();
  void move_cursor_to_first_line();
  void move_cursor_to_last_line();
  void change_mode(int mode);

  size_t curr_line_index();

  int cursor_y;
  int cursor_x;

  bool focused;

  bool multi_row;
  string title;

  vector<string> content;
  size_t content_size;
  ScrollableWindow<string> content_window;

  vector<char> first_line_chars;
  size_t first_line_size;
  ScrollableWindow<char> first_line_window;

  vector<bool> line_has_newline;
};
