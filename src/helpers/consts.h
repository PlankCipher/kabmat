#pragma once

#define NAME "kabmat"
#define VERSION "2.7.1"
#define DATA_FILE (string(getenv("HOME")) + "/.local/share/kabmat/data")
#define DATA_BACKUP_FILE                                                       \
  (string(getenv("HOME")) + "/.local/share/kabmat/data_bkp")

#define COLOR_PAIR_FOOTER 1
#define COLOR_PAIR_MODE 2
#define COLOR_PAIR_HEADER 3
#define COLOR_PAIR_BORDER 4
#define COLOR_PAIR_KEY_HINT 5

#define MODE_NORMAL 0
#define MODE_INSERT 1
