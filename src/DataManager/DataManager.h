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
  void rename_board(string old_name, string new_name);

  Board *get_board_if_exists(string name);
  vector<string> get_boards_names();

  vector<Board> boards;

private:
  vector<string> parse_data_if_valid();
  void write_data_to_file();
};
