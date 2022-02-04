#pragma once

#include <cstddef>

class MainMenu {
public:
  MainMenu();

  size_t highlighted_index;
  void show();
};
