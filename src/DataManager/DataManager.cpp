#include <fstream>
#include <iostream>

#include "../helpers/consts.h"
#include "../helpers/remove_trim_spaces/remove_trim_spaces.h"
#include "Board.h"
#include "DataManager.h"

DataManager::DataManager() {
  vector<string> lines = this->parse_data_if_valid();

  for (size_t i = 0; i < lines.size(); ++i) {
    string line = lines[i];

    if (line.find("  ") == 0)
      // since we are parsing the data progressively, the column
      // to which this card belongs is the last column we have
      this->boards.back().columns.back().add_card(line);
    else if (line.find(" ") == 0)
      // since we are parsing the data progressively, the board
      // to which this column belongs is the last board we have
      this->boards.back().add_column(line);
    else
      this->add_board(line);
  }
}

void DataManager::add_board(string name) {
  Board board(remove_trim_spaces(name));
  this->boards.push_back(board);
}

void DataManager::create_board(string name) {
  name = remove_trim_spaces(name);

  if (name.length() < 1) {
    fprintf(stderr, "ERROR: Can't create a board with empty name\n");
    exit(1);
  }

  fstream data_file;
  data_file.open(DATA_FILE, ios::app);

  if (data_file.is_open()) {
    data_file << name << "\n";

    this->boards.push_back(name);

    data_file.close();
  } else {
    fprintf(stderr, "ERROR: Couldn't open data file \"%s\"\n",
            DATA_FILE.c_str());
    exit(1);
  }
}

void DataManager::delete_board(string name) {
  this->get_board_if_exists(name);

  vector<Board> *boards = &this->boards;

  for (vector<Board>::reverse_iterator i = boards->rbegin();
       i != boards->rend(); ++i)
    if ((*i).name == name)
      boards->erase(i.base() - 1);

  this->write_data_to_file();
}

void DataManager::rename_board(string old_name, string new_name) {
  new_name = remove_trim_spaces(new_name);

  if (new_name.length() < 1) {
    fprintf(stderr, "ERROR: Board name can't be empty\n");
    exit(1);
  }

  for (size_t i = 0; i < this->boards.size(); ++i) {
    if (this->boards[i].name == old_name)
      this->boards[i].name = new_name;
  }

  this->write_data_to_file();
}

vector<string> DataManager::parse_data_if_valid() {
  fstream data_file;
  data_file.open(DATA_FILE, ios::in);

  if (data_file.is_open()) {
    string line;
    size_t line_count = 1;
    vector<string> lines;

    bool reading_board = false;
    bool reading_column = false;

    while (getline(data_file, line)) {
      if (line.find("  ") == 0) {
        if (!reading_column) {
          fprintf(stderr, "ERROR: incorrect syntax in data file \"%s\"\n",
                  DATA_FILE.c_str());
          fprintf(stderr, "[line %zu] reading a card without a column\n",
                  line_count);
          exit(1);
        }

      } else if (line.find(" ") == 0) {
        if (!reading_board) {
          fprintf(stderr, "ERROR: incorrect syntax in data file \"%s\"\n",
                  DATA_FILE.c_str());
          fprintf(stderr, "[line %zu] reading a column without a board\n",
                  line_count);
          exit(1);
        }

        reading_column = true;

      } else {
        reading_board = true;
        reading_column = false;
      }

      line_count++;
      lines.push_back(line);
    }

    data_file.close();
    return lines;
  } else {
    fprintf(stderr, "ERROR: Couldn't read data from file \"%s\"\n",
            DATA_FILE.c_str());
    exit(1);
  }
}

void DataManager::write_data_to_file() {
  fstream data_file;
  data_file.open(DATA_FILE, ios::out | ios::trunc);

  if (data_file.is_open()) {
    for (size_t i = 0; i < this->boards.size(); ++i) {
      Board curr_board = this->boards[i];

      data_file << curr_board.name << '\n';

      for (size_t j = 0; j < curr_board.columns.size(); ++j) {
        Column curr_column = curr_board.columns[j];

        data_file << " " << curr_column.title << '\n';

        for (size_t k = 0; k < curr_column.cards.size(); ++k) {
          data_file << "  " << curr_column.cards[k] << '\n';
        }
      }
    }

    data_file.close();
  } else {
    fprintf(stderr, "ERROR: Couldn't open data file \"%s\"\n",
            DATA_FILE.c_str());
    exit(1);
  }
}

Board *DataManager::get_board_if_exists(string name) {
  for (size_t i = 0; i < this->boards.size(); ++i) {
    if (this->boards[i].name == name) {
      return &this->boards[i];
    }
  }

  fprintf(stderr, "ERROR: No board named \"%s\" was found\n", name.c_str());
  exit(1);
}

vector<string> DataManager::get_boards_names() {
  vector<string> boards_names;
  for (size_t i = 0; i < this->boards.size(); ++i)
    boards_names.push_back(this->boards[i].name);
  return boards_names;
}
