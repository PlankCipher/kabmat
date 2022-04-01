#pragma once

#include "../../../DataManager/DataManager.h"
#include "../ScrollableWindow/ScrollableWindow.h"

class Checklist {
public:
  Checklist(int height, int width, int start_y, int start_x, Card *card,
            DataManager *data_manager);

  void show();

  int height;
  int width;
  int start_y;
  int start_x;
  WINDOW *window;

private:
  void draw_checklist_items(vector<ChecklistItem> shown_items,
                            WINDOW *scrollable_window);
  void setup_window();
  string create_input_window(string content = "");

  void highlight_above();
  void highlight_current();
  void highlight_below();
  void highlight_last();

  int highlighted_index;

  Card *card;
  DataManager *data_manager;

  vector<ChecklistItem> checklist_items;
  size_t checklist_items_count;
  ScrollableWindow<ChecklistItem> checklist_items_window;
};
