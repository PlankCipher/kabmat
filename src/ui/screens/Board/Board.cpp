#include "Board.h"
#include "../../components/Footer/Footer.h"
#include "../../components/Header/Header.h"

BoardScreen::BoardScreen(string board_name, DataManager *data_manager,
                         bool from_tui) {
  this->data_manager = data_manager;
  this->board = this->data_manager->get_board_if_exists(board_name);
  this->from_tui = from_tui;

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  this->height = max_y - 2;
  this->width = max_x;

  this->start_y = 1;
  this->start_x = 0;

  this->window =
      newwin(this->height, this->width, this->start_y, this->start_x);
}

void BoardScreen::show() {
  Header header(this->board->name);
  header.show();

  Footer footer(false, true);
  footer.show();

  refresh();
  wrefresh(this->window);

  getch();
}
