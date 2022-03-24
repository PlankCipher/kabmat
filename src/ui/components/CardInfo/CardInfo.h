#pragma once

#include <ncurses.h>

#include "../../../DataManager/Card.h"
#include "../Input/Input.h"

class CardInfo {
public:
  CardInfo(int height, int width, int start_y, int start_x, Card *card);

  bool show();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  Card *card;
  Input *focused_input;
  bool focused_content_input;
};
