#include "Checklist.h"
#include "../../../helpers/consts.h"
#include "../../helpers/win_center_text/win_center_text.h"
#include "../../helpers/win_fit_text/win_fit_text.h"
#include "../Input/Input.h"

Checklist::Checklist(int height, int width, int start_y, int start_x,
                     Card *card, DataManager *data_manager)
    : height{height}, width{width}, start_y{start_y}, start_x{start_x},
      checklist_items_window{this->height,
                             this->width,
                             this->start_y,
                             this->start_x,
                             &this->checklist_items,
                             &this->checklist_items_count,
                             bind(&Checklist::draw_checklist_items, this,
                                  placeholders::_1, placeholders::_2),
                             this->height - 3} {
  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);

  this->card = card;

  this->checklist_items = this->card->checklist;
  this->checklist_items_count = this->checklist_items.size();

  this->highlighted_index = 0;
  this->data_manager = data_manager;
}

void Checklist::setup_window() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  string window_title = " Checklist";

  if (this->checklist_items_count > 0) {
    size_t done_count = 0;
    size_t total = this->checklist_items_count;

    for (size_t i = 0; i < total; ++i)
      done_count += this->card->checklist[i].done;

    // add [x/y] to title
    window_title += " [" + to_string(done_count) + "/" + to_string(total) + "]";

    // progress bar |[====  ] zzz%|
    int done_percent = (done_count / (float)total) * 100;

    string progress_bar = "";
    for (int i = 0; i < int((done_percent / 100.0) * (this->width - 9)); ++i)
      progress_bar += '=';

    mvwprintw(this->window, 1, 1, "[%-*s] %3d%%", this->width - 9,
              progress_bar.c_str(), done_percent);
  }

  window_title += " ";

  mvwprintw(this->window, 0, (this->width / 2) - (window_title.length() / 2),
            "%s", window_title.c_str());

  refresh();
  wrefresh(this->window);
}

void Checklist::show() {
  this->checklist_items_window.scroll_to_top();

  bool done = false;
  char key;
  while (!done && (key = wgetch(this->window))) {
    switch (key) {
    case 'q': {
      // quit
      done = true;

      break;
    }
    case 'k': {
      if (this->checklist_items_count > 0) {
        this->highlight_above();
      }

      break;
    }
    case 'j': {
      if (this->checklist_items_count > 0) {
        this->highlight_below();
      }

      break;
    }
    case 'g': {
      // highlight first item
      if (this->checklist_items_count > 0) {
        this->highlighted_index = 0;
        this->checklist_items_window.scroll_to_top();
      }

      break;
    }
    case 'G': {
      // highlight last board
      if (this->checklist_items_count > 0) {
        this->highlight_last();
      }

      break;
    }
    case 'K': {
      // move highlighted item up
      if (this->checklist_items_count > 0) {
        size_t highlighted_item_index =
            (this->checklist_items_window.window_start -
             this->checklist_items.begin()) +
            this->highlighted_index;
        bool moved = this->data_manager->move_checklist_item_up(
            this->card, highlighted_item_index);

        if (moved) {
          this->checklist_items = this->card->checklist;
          this->checklist_items_window.draw();

          this->highlight_above();
        }
      }

      break;
    }
    case 'J': {
      // move highlighted item down
      if (this->checklist_items_count > 0) {
        size_t highlighted_item_index =
            (this->checklist_items_window.window_start -
             this->checklist_items.begin()) +
            this->highlighted_index;
        bool moved = this->data_manager->move_checklist_item_down(
            this->card, highlighted_item_index);

        if (moved) {
          this->checklist_items = this->card->checklist;
          this->checklist_items_window.draw();

          this->highlight_below();
        }
      }

      break;
    }
    case ' ': {
      // toggle highlighted item (done/not done)
      if (this->checklist_items_count > 0) {
        size_t highlighted_item_index =
            (this->checklist_items_window.window_start -
             this->checklist_items.begin()) +
            this->highlighted_index;
        ChecklistItem highlighted_item =
            this->checklist_items[highlighted_item_index];

        highlighted_item.done = !highlighted_item.done;
        this->data_manager->update_checklist_item(
            this->card, highlighted_item_index, highlighted_item);

        this->checklist_items = this->card->checklist;
        this->checklist_items_window.draw();
      }

      break;
    }
    case 'c': {
      // add an item
      string item_content = this->create_input_window();

      if (item_content.length() > 0) {
        ChecklistItem item = {item_content, false};
        this->data_manager->add_checklist_item(this->card, item);

        this->checklist_items = this->card->checklist;
        this->checklist_items_count = this->checklist_items.size();

        // highlight the just added item
        this->highlighted_index =
            min((size_t)this->checklist_items_window.max_items_in_win - 1,
                this->checklist_items_count - 1);
        this->checklist_items_window.scroll_to_bottom();
      }

      break;
    }
    case 'e': {
      // edit highlighted item's content
      if (this->checklist_items_count > 0) {
        size_t highlighted_item_index =
            (this->checklist_items_window.window_start -
             this->checklist_items.begin()) +
            this->highlighted_index;
        ChecklistItem highlighted_item =
            this->checklist_items[highlighted_item_index];

        string new_content =
            this->create_input_window(highlighted_item.content);

        if (new_content.length() > 0) {
          size_t offset = this->checklist_items_window.window_start -
                          this->checklist_items.begin();

          highlighted_item.content = new_content;
          this->data_manager->update_checklist_item(
              this->card, highlighted_item_index, highlighted_item);

          this->checklist_items = this->card->checklist;
          this->checklist_items_window.scroll_to_offset(offset);
        }
      }

      break;
    }
    case 'd': {
      // delete highlighted item
      if (this->checklist_items_count > 0) {
        size_t highlighted_item_index =
            (this->checklist_items_window.window_start -
             this->checklist_items.begin()) +
            this->highlighted_index;
        size_t prev_offset = this->checklist_items_window.window_start -
                             this->checklist_items.begin();

        this->data_manager->delete_checklist_item(this->card,
                                                  highlighted_item_index);

        this->checklist_items = this->card->checklist;
        this->checklist_items_count = this->checklist_items.size();

        this->highlighted_index = min((size_t)this->highlighted_index,
                                      this->checklist_items_count - 1);
        this->checklist_items_window.scroll_to_offset(prev_offset);
      }

      break;
    }
    }
  }

  werase(this->window);
  wrefresh(this->window);
}

void Checklist::draw_checklist_items(
    vector<ChecklistItem> shown_items,
    [[maybe_unused]] WINDOW *scrollable_window) {
  werase(this->window);
  this->setup_window();

  if (shown_items.size() > 0) {
    // draw items
    for (size_t i = 0; i < shown_items.size(); ++i) {
      // the [x] or [ ] before item content
      string item = "[";
      item += shown_items[i].done ? 'x' : ' ';
      item += "] ";

      // actual item content
      item += shown_items[i].content;

      mvwprintw(this->window, i + 2, 1, "%s",
                win_fit_text(this->window, item).c_str());
    }

    this->highlight_current();
  } else {
    string create_board_hint = "c to add a checklist item";
    int center_x = win_center_x(this->window, &create_board_hint);
    mvwprintw(this->window, 1, center_x, "%s", create_board_hint.c_str());
  }

  wrefresh(this->window);
}

void Checklist::highlight_current() {
  if (this->checklist_items_count > 0) {
    // clear previous highlights
    for (int i = 0; i < this->checklist_items_window.max_items_in_win; ++i)
      mvwchgat(this->window, i + 2, 1, this->width - 2, A_NORMAL, 0, NULL);

    // highlight current
    mvwchgat(this->window, this->highlighted_index + 2, 1, this->width - 2,
             A_NORMAL, COLOR_PAIR_FOOTER, NULL);

    wrefresh(this->window);
  }
}

void Checklist::highlight_above() {
  if (--this->highlighted_index == -1) {
    this->highlighted_index = 0;
    this->checklist_items_window.scroll_up();
  } else
    this->highlight_current();
}

void Checklist::highlight_below() {
  this->highlighted_index =
      min(this->checklist_items_count - 1, (size_t)this->highlighted_index + 1);

  if (this->highlighted_index ==
      this->checklist_items_window.max_items_in_win) {
    this->highlighted_index = this->checklist_items_window.max_items_in_win - 1;
    this->checklist_items_window.scroll_down();
  } else
    this->highlight_current();
}

void Checklist::highlight_last() {
  this->highlighted_index =
      min((size_t)this->checklist_items_window.max_items_in_win - 1,
          this->checklist_items_count - 1);
  this->checklist_items_window.scroll_to_bottom();
}

string Checklist::create_input_window(string content) {
  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  int height = 3;
  int width = max_x * 0.4;
  int start_y = (max_y / 2) - (height / 2);
  int start_x = (max_x / 2) - (width / 2);

  Input input_bar(height, width, start_y, start_x, content, " Checklist Item ",
                  true);
  input_bar.show(true);
  string input = input_bar.get_value();

  this->checklist_items_window.draw();

  return input;
}
