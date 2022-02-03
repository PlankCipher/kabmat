#pragma once

#include <string>
#include <unordered_map>

#include "../DataManager/DataManager.h"

using namespace std;

class ArgsParser {
public:
  ArgsParser(int argc, char **argv, DataManager *data_manager);

  unordered_map<string, string> arguments;

private:
  void parse_argument(string argument, string value, DataManager *data_manager);
  void usage();
};
