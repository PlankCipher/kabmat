#include "ArgsParser.h"
#include <algorithm>
#include <string.h>
#include <vector>

using namespace std;

ArgsParser::ArgsParser(int argc, char **argv) {
  if (argc > 1) {
    for (int i = 1; i < argc; i += 2) {
      string argument = argv[i];
      string value = "";

      vector<string> no_value_args = {"-v",        "-h",     "-l",
                                      "--version", "--help", "--list"};

      if (find(no_value_args.begin(), no_value_args.end(), argv[i]) ==
          no_value_args.end()) {
        if (i + 1 >= argc) {
          printf("ERROR: Not enough arguments for option `%s`\n",
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
      this->parse_argument(argument, value);
    }
  }
}

void ArgsParser::parse_argument(string argument, string value) {
  if (argument.compare("-v") == 0 || argument.compare("--version") == 0)
    cout << NAME << ' ' << VERSION << endl;
  else if (argument.compare("-h") == 0 || argument.compare("--help") == 0)
    this->usage();
  else if (argument.compare("-c") == 0 || argument.compare("--create") == 0)
    cout << "CREATING A NEW BOARD WITH THE NAME " << value << endl;
  else if (argument.compare("-l") == 0 || argument.compare("--list") == 0)
    cout << "LISTING ALL BOARDS" << endl;
  else if (argument.compare("-o") == 0 || argument.compare("--open") == 0)
    cout << "OPENING THE BOARD WITH THE NAME " << value << endl;
  else if (argument.compare("-d") == 0 || argument.compare("--delete") == 0)
    cout << "DELETING THE BOARD WITH THE NAME " << value << endl;
}

void ArgsParser::usage() {
  cout << NAME << ' ' << VERSION << endl;
  cout << "TUI program for managing kanban boards with vim-like keybindings"
       << endl
       << endl;

  cout << "Usage: kabmat [OPTION]..." << endl << endl;

  cout << "Options: " << endl;
  cout << "  -h, --help             print this help message" << endl;
  cout << "  -v, --version          print program version" << endl << endl;

  cout << "  -l, --list             list all boards" << endl;
  cout << "  -c, --create <name>    create a new board with the name <name>"
       << endl;
  cout << "  -o, --open <name>      open board with name <name>" << endl;
  cout << "  -d, --delete <name>    delete board with name <name>" << endl;
}
