#include "ArgsParser/ArgsParser.h"
#include "Config/Config.h"
#include "DataManager/DataManager.h"

int main(int argc, char **argv) {
  DataManager data_manager;
  Config config;
  ArgsParser args_parser(argc, argv, &data_manager, &config);

  return 0;
}
