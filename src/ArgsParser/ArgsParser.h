#pragma once

#include <unordered_map>

#include "../DataManager/DataManager.h"

class ArgsParser {
public:
  ArgsParser(int argc, char **argv, DataManager *data_manager, Config *config);

private:
  void parse_argument(string argument, string value, DataManager *data_manager,
                      Config *config);
  void usage();

  unordered_map<string, string> arguments;
};
