#pragma once

#include <string>
#include <unordered_map>

#include "../Config/Config.h"
#include "../DataManager/DataManager.h"

using namespace std;

class ArgsParser {
public:
  ArgsParser(int argc, char **argv, DataManager *data_manager, Config *config);

  unordered_map<string, string> arguments;

private:
  void parse_argument(string argument, string value, DataManager *data_manager,
                      Config *config);
  void usage();
};
