#include <filesystem>
#include <fstream>

#include "../helpers/consts.h"
#include "../helpers/trim_spaces/trim_spaces.h"
#include "DataManager.h"

DataManager::DataManager() {
  fstream data_file;
  data_file.open(DATA_FILE, ios::in);

  if (data_file.is_open()) {
    string line;
    size_t line_count = 1;

    bool reading_board = false;
    bool reading_column = false;
    bool reading_card = false;

    while (getline(data_file, line)) {
      if (line.find("    ") == 0) {
        if (!reading_card) {
          fprintf(stderr, "ERROR: incorrect syntax in data file \"%s\"\n",
                  DATA_FILE.c_str());
          fprintf(stderr, "[line %zu] reading a description without a card\n",
                  line_count);
          exit(1);
        } else {
          // since we are parsing the data progressively, the card
          // to which this description belongs is the last card we have
          line.erase(line.begin(), line.begin() + 4);
          this->boards.back().columns.back().cards.back().description +=
              line + "\n";
        }
      } else if (line.find("   ") == 0) {
        if (!reading_card) {
          fprintf(stderr, "ERROR: incorrect syntax in data file \"%s\"\n",
                  DATA_FILE.c_str());
          fprintf(stderr,
                  "[line %zu] reading a checklist item without a card\n",
                  line_count);
          exit(1);
        } else {
          // since we are parsing the data progressively, the card
          // to which this checklist item belongs is the last card we have
          line = trim_spaces(line);

          ChecklistItem checklist_item;
          checklist_item.done = line[0] == '+';
          line.erase(line.begin(), line.begin() + 1);
          checklist_item.content = line;

          this->boards.back().columns.back().cards.back().add_checklist_item(
              checklist_item);
        }
      } else if (line.find("  ") == 0) {
        if (!reading_column) {
          fprintf(stderr, "ERROR: incorrect syntax in data file \"%s\"\n",
                  DATA_FILE.c_str());
          fprintf(stderr, "[line %zu] reading a card without a column\n",
                  line_count);
          exit(1);
        } else
          // since we are parsing the data progressively, the column
          // to which this card belongs is the last column we have
          this->boards.back().columns.back().add_card(Card(trim_spaces(line)));

        reading_card = true;
      } else if (line.find(" ") == 0) {
        if (!reading_board) {
          fprintf(stderr, "ERROR: incorrect syntax in data file \"%s\"\n",
                  DATA_FILE.c_str());
          fprintf(stderr, "[line %zu] reading a column without a board\n",
                  line_count);
          exit(1);
        } else
          // since we are parsing the data progressively, the board
          // to which this column belongs is the last board we have
          this->boards.back().add_column(line);

        reading_column = true;
      } else {
        reading_board = true;
        reading_column = false;
        reading_card = false;
        this->add_board(line);
      }

      line_count++;
    }

    // sure, let's spin up some loops for a quick and dirty
    // way to remove extra newlines at the end of descriptions
    // of cards because i am too lazy to think of something else
    for (size_t i = 0; i < this->boards.size(); ++i) {
      for (size_t j = 0; j < this->boards[i].columns.size(); ++j) {
        for (size_t k = 0; k < this->boards[i].columns[j].cards.size(); ++k) {
          Card *card = &this->boards[i].columns[j].cards[k];
          if (card->description.length() > 0)
            card->description.erase(card->description.end() - 1);
        }
      }
    }

    data_file.close();
  } else {
    fprintf(stderr, "ERROR: Couldn't read data from file \"%s\"\n",
            DATA_FILE.c_str());
    exit(1);
  }
}

void DataManager::add_board(string name) {
  Board board(trim_spaces(name));
  this->boards.push_back(board);
}

void DataManager::create_board(string name) {
  name = trim_spaces(name);

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

void DataManager::rename_board(string old_name, string new_name) {
  new_name = trim_spaces(new_name);

  if (new_name.length() < 1) {
    fprintf(stderr, "ERROR: Board name can't be empty\n");
    exit(1);
  }

  (*this->get_board_if_exists(old_name)).name = new_name;

  this->write_data_to_file();
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

bool DataManager::move_board_up(size_t board_index) {
  bool moved = false;

  if (board_index > 0) {
    swap(this->boards[board_index], this->boards[board_index - 1]);
    moved = true;
  }

  this->write_data_to_file();

  return moved;
}

bool DataManager::move_board_down(size_t board_index) {
  bool moved = false;

  if (board_index < this->boards.size() - 1) {
    swap(this->boards[board_index], this->boards[board_index + 1]);
    moved = true;
  }

  this->write_data_to_file();

  return moved;
}

Board *DataManager::get_board_if_exists(string name) {
  for (size_t i = 0; i < this->boards.size(); ++i)
    if (this->boards[i].name == name)
      return &this->boards[i];

  fprintf(stderr, "ERROR: No board named \"%s\" was found\n", name.c_str());
  exit(1);
}

vector<string> DataManager::get_boards_names() {
  vector<string> boards_names;
  for (size_t i = 0; i < this->boards.size(); ++i)
    boards_names.push_back(this->boards[i].name);

  return boards_names;
}

void DataManager::create_column(Board *board, string title) {
  board->add_column(title);
  this->write_data_to_file();
}

void DataManager::rename_column(Board *board, size_t column_index,
                                string new_title) {
  board->rename_column(column_index, new_title);
  this->write_data_to_file();
}

void DataManager::delete_column(Board *board, size_t column_index) {
  board->delete_column(column_index);
  this->write_data_to_file();
}

bool DataManager::move_column_left(Board *board, size_t column_index) {
  bool moved = board->move_column_left(column_index);
  if (moved)
    this->write_data_to_file();
  return moved;
}

bool DataManager::move_column_right(Board *board, size_t column_index) {
  bool moved = board->move_column_right(column_index);
  if (moved)
    this->write_data_to_file();
  return moved;
}

void DataManager::add_card(Column *column, Card card) {
  column->add_card(card);
  this->write_data_to_file();
}

void DataManager::update_card(Column *column, size_t card_index, Card card) {
  column->update_card(card_index, card);
  this->write_data_to_file();
}

void DataManager::delete_card(Column *column, size_t card_index) {
  column->delete_card(card_index);
  this->write_data_to_file();
}

bool DataManager::move_card_up(Column *column, size_t card_index) {
  bool moved = column->move_card_up(card_index);
  if (moved)
    this->write_data_to_file();
  return moved;
}

bool DataManager::move_card_down(Column *column, size_t card_index) {
  bool moved = column->move_card_down(card_index);
  if (moved)
    this->write_data_to_file();
  return moved;
}

bool DataManager::move_card_to_prev_column(Board *board, size_t card_index,
                                           size_t src_index, size_t dist_index,
                                           Config *config) {
  bool moved = board->move_card_to_prev_column(card_index, src_index,
                                               dist_index, config);
  if (moved)
    this->write_data_to_file();
  return moved;
}

bool DataManager::move_card_to_next_column(Board *board, size_t card_index,
                                           size_t src_index, size_t dist_index,
                                           Config *config) {
  bool moved = board->move_card_to_next_column(card_index, src_index,
                                               dist_index, config);
  if (moved)
    this->write_data_to_file();
  return moved;
}

void DataManager::add_checklist_item(Card *card, ChecklistItem item) {
  card->add_checklist_item(item);
  this->write_data_to_file();
}

void DataManager::update_checklist_item(Card *card, size_t item_index,
                                        ChecklistItem item) {
  card->update_checklist_item(item_index, item);
  this->write_data_to_file();
}

void DataManager::delete_checklist_item(Card *card, size_t item_index) {
  card->delete_checklist_item(item_index);
  this->write_data_to_file();
}

bool DataManager::move_checklist_item_up(Card *card, size_t item_index) {
  bool moved = card->move_checklist_item_up(item_index);
  if (moved)
    this->write_data_to_file();
  return moved;
}

bool DataManager::move_checklist_item_down(Card *card, size_t item_index) {
  bool moved = card->move_checklist_item_down(item_index);
  if (moved)
    this->write_data_to_file();
  return moved;
}

void DataManager::write_data_to_file() {
  // copy data to backup file before writing
  filesystem::copy_file(DATA_FILE, DATA_BACKUP_FILE,
                        filesystem::copy_options::overwrite_existing);

  // write data
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
          Card curr_card = curr_column.cards[k];

          data_file << "  " << curr_card.content << '\n';

          for (size_t l = 0; l < curr_card.checklist.size(); ++l)
            data_file << "   " << (curr_card.checklist[l].done ? '+' : '-')
                      << curr_card.checklist[l].content << '\n';

          for (size_t l = 0; l < curr_card.description.length(); ++l)
            if (curr_card.description[l] == '\n')
              curr_card.description.insert(
                  curr_card.description.begin() + l + 1, 4, ' ');

          if (curr_card.description.length() > 0)
            data_file << "    " << curr_card.description << '\n';
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
