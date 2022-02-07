#include "win_center_text.h"

int win_center_x(WINDOW *window, string *text) {
  size_t max_x = getmaxx(window);

  if (text->length() > max_x - 2)
    *text = text->substr(0, max_x - 4) + "..";

  int center_x = (max_x / 2) - (text->length() / 2);
  return center_x;
}
