#pragma once

#include <string>

using namespace std;

class Header {
public:
  Header(string board_name);

  void show();

private:
  string board_name;
};
