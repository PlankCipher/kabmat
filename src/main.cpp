#include "ArgsParser/ArgsParser.h"
#include "helpers/consts.h"
#include "ui/screens/Board/Board.h"
#include "ui/screens/MainMenu/MainMenu.h"

int main(int argc, char **argv) {
  DataManager data_manager;
  Config config;
  ArgsParser args_parser(argc, argv, &data_manager, &config);

  if (config.tui_enabled) {
    initscr();

    if (!has_colors()) {
      endwin();
      fprintf(stderr, "Your terminal does not support color\n");
      exit(1);
    }

    // ncurses setup
    noecho();
    curs_set(0);
    start_color();
    use_default_colors();

    init_pair(COLOR_PAIR_FOOTER, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR_MODE, COLOR_BLACK, COLOR_BLUE);
    init_pair(COLOR_PAIR_HEADER, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_PAIR_BORDER, COLOR_WHITE, -1);
    init_pair(COLOR_PAIR_KEY_HINT, COLOR_WHITE, -1);

    if (config.default_board == "") {
      MainMenu main_menu(&data_manager, &config);
      main_menu.show();
    } else {
      BoardScreen board_screen(config.default_board, &data_manager, &config,
                               false);
      board_screen.show();
    }

    endwin();
  }

  return 0;
}
