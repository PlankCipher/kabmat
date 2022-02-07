#pragma once

#include <string>
#include <vector>

#include "Board.h"

using namespace std;

class DataManager {
public:
  DataManager();

  void add_board(string name);
  void create_board(string name);
  void delete_board(string name);

  void does_board_exist(string name);
  vector<string> get_boards_names();

  vector<Board> boards;

private:
  vector<string> parse_data_if_valid();
  void write_data_to_file();
};
