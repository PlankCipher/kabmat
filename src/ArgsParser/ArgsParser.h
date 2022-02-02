#pragma once

#include <string>
#include <unordered_map>

using namespace std;

class ArgsParser {
public:
  ArgsParser(int argc, char **argv);

  unordered_map<string, string> arguments;

private:
  void parse_argument(string argument, string value);
  void usage();
};
