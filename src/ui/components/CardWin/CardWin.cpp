#include "CardWin.h"
#include "../../../helpers/consts.h"
#include "../../helpers/win_fit_text/win_fit_text.h"

CardWin::CardWin(int height, int width, Card *card) {
  this->height = height;
  this->width = width;
  this->start_y = 0;
  this->start_x = 0;

  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);
  refresh();

  this->card = card;
}

void CardWin::show(int start_y, int start_x) {
  this->start_y = start_y;
  this->start_x = start_x;

  mvwin(this->window, this->start_y, this->start_x);

  box(this->window, 0, 0);

  string shown_content = win_fit_text(this->window, this->card->content);
  mvwprintw(this->window, 1, 1, "%s", shown_content.c_str());
}

void CardWin::focus() {
  wattron(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));
  box(this->window, 0, 0);
  wattroff(this->window, COLOR_PAIR(COLOR_PAIR_BORDER));

  // [x/y] on the right
  if (this->card->checklist.size() > 0) {
    size_t done_count = 0;
    size_t total = this->card->checklist.size();

    for (size_t i = 0; i < total; ++i)
      done_count += this->card->checklist[i].done;

    string checklist_overview =
        "[" + to_string(done_count) + "/" + to_string(total) + "]";

    wattron(this->window, COLOR_PAIR(COLOR_PAIR_FOOTER));
    mvwprintw(this->window, 0, this->width - checklist_overview.length() - 2,
              "%s", checklist_overview.c_str());
    wattroff(this->window, COLOR_PAIR(COLOR_PAIR_FOOTER));
  }

  wrefresh(this->window);
}

void CardWin::unfocus() {
  box(this->window, 0, 0);

  // [x/y] on the right
  if (this->card->checklist.size() > 0) {
    size_t done_count = 0;
    size_t total = this->card->checklist.size();

    for (size_t i = 0; i < total; ++i)
      done_count += this->card->checklist[i].done;

    string checklist_overview =
        "[" + to_string(done_count) + "/" + to_string(total) + "]";
    mvwprintw(this->window, 0, this->width - checklist_overview.length() - 2,
              "%s", checklist_overview.c_str());
  }

  wrefresh(this->window);
}
