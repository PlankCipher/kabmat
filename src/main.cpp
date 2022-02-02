#include "ArgsParser/ArgsParser.h"
#include "DataManager/DataManager.h"

int main(int argc, char **argv) {
  DataManager data_manager;
  ArgsParser args_parser(argc, argv);

  return 0;
}
