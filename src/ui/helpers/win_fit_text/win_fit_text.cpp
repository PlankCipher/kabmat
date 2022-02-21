#include "win_fit_text.h"

string win_fit_text(WINDOW *window, string text) {
  size_t max_x = getmaxx(window);

  if (text.length() > max_x - 2)
    text = text.substr(0, max_x - 4) + "..";

  return text;
}
