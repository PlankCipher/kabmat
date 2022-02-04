#include <ncurses.h>

#include "../../components/Footer/Footer.h"
#include "MainMenu.h"

MainMenu::MainMenu() { this->highlighted_index = 0; }

void MainMenu::show() {
  Footer footer(false, false);
  footer.show();

  getch();
}
