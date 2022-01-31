#pragma once

#include <iostream>
#include <unordered_map>

#define NAME "kabmat"
#define VERSION "1.0.0"

using namespace std;

class ArgsParser {
public:
  ArgsParser(int argc, char **argv);

  unordered_map<string, string> arguments;

private:
  void parse_argument(string argument, string value);
  void usage();
};
