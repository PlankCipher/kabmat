#include "win_center_text.h"
#include "../win_fit_text/win_fit_text.h"

int win_center_x(WINDOW *window, string *text) {
  size_t max_x = getmaxx(window);

  *text = win_fit_text(window, *text);

  int center_x = (max_x / 2) - (text->length() / 2);
  return center_x;
}
