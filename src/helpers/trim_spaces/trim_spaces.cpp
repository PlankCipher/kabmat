#include "trim_spaces.h"

string trim_spaces(string str) {
  size_t first_non_space = str.find_first_not_of(' ');

  // if can't find a non space character
  // then the whole string is spaces
  if (first_non_space == string::npos) {
    return "";
  }

  size_t last_non_space = str.find_last_not_of(' ');

  // trimming spaces from left and right by getting the
  // substring between first and last non space characters
  str = str.substr(first_non_space, (last_non_space - first_non_space + 1));

  return str;
}
