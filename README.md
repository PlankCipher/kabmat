# kabmat (WIP)

TUI program for managing kanban boards with vim-like keybindings

## Installation

After the command below runs successfully, there should be a `kabmat` executable file in `/usr/bin/` (sudo privileges required to copy it to there).

```sh
$ make install
```

## Usage

```
kabmat 2.7.0
TUI program for managing kanban boards with vim-like keybindings

Usage: kabmat [OPTION]...

Options:
  -h, --help              print this help message
  -v, --version           print program version

  -l, --list              list all boards
  -c, --create <name>     create a new board with the name <name>
  -o, --open <name>       open board with name <name>
  -d, --delete <name>     delete board with name <name>

  -t, --text              disable tui
  -b, --card-at-bottom    when moving cards between columns, put them at the bottom
```
