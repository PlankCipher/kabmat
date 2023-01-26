#include <algorithm>
#include <iostream>

#include "../helpers/consts.h"
#include "ArgsParser.h"

ArgsParser::ArgsParser(int argc, char **argv, DataManager *data_manager,
                       Config *config) {
  if (argc > 1) {
    for (int i = 1; i < argc; i += 2) {
      string argument = argv[i];
      string value = "";

      vector<string> all_args = {"-h", "--help", "-v", "--version",
                                 "-l", "--list", "-c", "--create",
                                 "-o", "--open", "-d", "--delete",
                                 "-t", "--text", "-b", "--card-at-bottom"};

      if (find(all_args.begin(), all_args.end(), argument) == all_args.end()) {
        fprintf(stderr, "ERROR: Unknown option `%s`\n", argument.c_str());
        exit(1);
      }

      vector<string> no_value_args = {
          "-h",     "--help", "-v",     "--version", "-l",
          "--list", "-t",     "--text", "-b",        "--card-at-bottom"};

      if (find(no_value_args.begin(), no_value_args.end(), argument) ==
          no_value_args.end()) {
        if (i + 1 >= argc) {
          fprintf(stderr, "ERROR: Not enough arguments for option `%s`\n",
                  argument.c_str());
          exit(1);
        }

        value = argv[i + 1];
      } else {
        // decrement the index if no value is needed for
        // argument to actually make it increment by 1
        // to get to the next argument
        i--;
      }

      this->arguments[argument] = value;
      this->parse_argument(argument, value, data_manager, config);
    }
  }
}

void ArgsParser::parse_argument(string argument, string value,
                                DataManager *data_manager, Config *config) {
  if (argument == "-v" || argument == "--version") {
    config->tui_enabled = false;
    cout << NAME << ' ' << VERSION << endl;
  } else if (argument == "-h" || argument == "--help") {
    config->tui_enabled = false;
    this->usage();
  } else if (argument == "-c" || argument == "--create") {
    data_manager->create_board(value);
  } else if (argument == "-l" || argument == "--list") {
    config->tui_enabled = false;
    vector<string> boards_names = data_manager->get_boards_names();
    for (size_t i = 0; i < boards_names.size(); ++i)
      cout << boards_names[i] << endl;
  } else if (argument == "-o" || argument == "--open") {
    data_manager->get_board_if_exists(value);
    config->default_board = value;
  } else if (argument == "-d" || argument == "--delete") {
    data_manager->delete_board(value);
  } else if (argument == "-t" || argument == "--text") {
    config->tui_enabled = false;
  } else if (argument == "-b" || argument == "--card-at-bottom") {
    config->move_card_to_column_bottom = true;
  }
}

void ArgsParser::usage() {
  cout << NAME << ' ' << VERSION << endl;
  cout << "TUI program for managing kanban boards with vim-like keybindings"
       << endl
       << endl;

  cout << "Usage: kabmat [OPTION]..." << endl << endl;

  cout << "Options: " << endl;
  cout << "  -h, --help              print this help message" << endl;
  cout << "  -v, --version           print program version" << endl << endl;

  cout << "  -l, --list              list all boards" << endl;
  cout << "  -c, --create <name>     create a new board with the name <name>"
       << endl;
  cout << "  -o, --open <name>       open board with name <name>" << endl;
  cout << "  -d, --delete <name>     delete board with name <name>" << endl
       << endl;

  cout << "  -t, --text              disable tui" << endl;
  cout << "  -b, --card-at-bottom    when moving cards between columns, put "
          "them at the bottom"
       << endl
       << endl;
  cout << "Consult the man page for more information" << endl;
}
