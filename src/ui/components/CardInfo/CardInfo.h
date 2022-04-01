#pragma once

#include "../../../DataManager/DataManager.h"
#include "../Input/Input.h"

class CardInfo {
public:
  CardInfo(int height, int width, int start_y, int start_x, Card *card);

  bool show(DataManager *data_manager);

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  void setup_window(Input *content_input, Input *description_input,
                    bool just_redraw);

  Card *card;
  Input *focused_input;
  bool focused_content_input;
};
