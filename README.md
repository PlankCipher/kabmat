# kabmat

TUI program for managing kanban boards with vim-like keybindings

![kabmat](./assets/kabmat.gif)

![kabmat](./assets/kabmat.png)

## Dependencies

- `ncurses` for the UI
- `make` and `g++`/`gcc` for compiling and installing

## Installation

1. get the source code from latest release or clone this repo => `git clone https://github.com/PlankCipher/kabmat.git`
2. cd into the source code or cloned repo => `cd kabmat`
3. run `make` to build the program
4. run `sudo make install` to install it

## Usage

```
kabmat 2.7.1
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

Consult the man page for more information
```

## Keybindings in `kabmat`

I tried making the keybindings as intuitive and expected as possible, and for the most part they are. For example, generally `c` creates an entity, `d` deletes, `k` moves up, `j` move down, etc...

### Help Window:

| Key | Function             |
| --- | -------------------- |
| k   | scroll up one line   |
| j   | scroll down one line |
| q   | close the window     |

### Main Menu:

| Key       | Function                               |
| --------- | -------------------------------------- |
| q         | quit                                   |
| ?         | show help window                       |
| k         | highlight the above board name         |
| j         | highlight the below board name         |
| g         | highlight the first board name         |
| G         | highlight the last board name          |
| K         | move highlighted board up              |
| J         | move highlighted board down            |
| d         | delete the currently highlighted board |
| r         | rename the currently highlighted board |
| c         | create a new board and highlight it    |
| \<Enter\> | open the currently highlighted board   |

### Input Field:

#### Normal mode:

| Key        | Function                                                         |
| ---------- | ---------------------------------------------------------------- |
| \<Esc\>, q | cancel and close the input field                                 |
| \<Enter\>  | submit and close the input field                                 |
| h          | move cursor one character to the left                            |
| l          | move cursor one character to the right                           |
| 0          | move cursor to the start of the line                             |
| $          | move cursor to the end of the line                               |
| k          | move cursor up one line (in multi-row input only)                |
| j          | move cursor down one line (in multi-row input only)              |
| g          | move cursor to the first line (in multi-row input only)          |
| G          | move cursor to the last line (in multi-row input only)           |
| i          | change mode to insert                                            |
| a          | move cursor one character to the right and change mode to insert |
| I          | move cursor to the start of the line and change mode to insert   |
| A          | move cursor to the end of the line and change mode to insert     |
| S          | delete everything on the line and change mode to insert          |
| d          | delete line under cursor (in multi-row input only)               |

#### Insert mode:

| Key                      | Function                                                                |
| ------------------------ | ----------------------------------------------------------------------- |
| \<Esc\>                  | change mode to normal                                                   |
| \<Enter\>                | submit and close the input field (or add a new line in multi-row input) |
| \<Backspace\>/\<Delete\> | delete the character before the cursor                                  |
| any other key            | inserted before the cursor                                              |

### Confirmation Window:

| Key          | Function             |
| ------------ | -------------------- |
| \<Enter\>, y | confirm action (yes) |
| \<Esc\>, n   | cancel action (no)   |

### Board Screen:

| Key              | Function                                                   |
| ---------------- | ---------------------------------------------------------- |
| q                | quit to where the board was opened from (main menu or cli) |
| ?                | show help window                                           |
| h                | focus the left column                                      |
| l                | focus the right column                                     |
| k                | focus the above card                                       |
| j                | focus the below card                                       |
| g                | focus the first card                                       |
| G                | focus the last card                                        |
| H                | move focused card to the left column                       |
| L                | move focused card to the right column                      |
| K                | move focused card up                                       |
| J                | move focused card down                                     |
| \<C-h\>, \<C-p\> | move focused column to the left                            |
| \<C-l\>, \<C-n\> | move focused column to the right                           |
| C                | create a new column                                        |
| E                | edit title of focused column                               |
| D                | delete focused column                                      |
| c                | create a new card in focused column                        |
| e                | edit focused card                                          |
| d                | delete focused card                                        |

### Card Info Window:

| Key           | Function                                      |
| ------------- | --------------------------------------------- |
| q             | cancel and close (if in normal mode)          |
| \<Enter\>     | submit and close (if in normal mode)          |
| \<Tab\>       | switch focused input (content or description) |
| c             | open checklist items window                   |
| any other key | handled by the focused input                  |

### Checklist Window:

| Key       | Function                                |
| --------- | --------------------------------------- |
| q         | close the window                        |
| k         | highlight the item above                |
| j         | highlight the item below                |
| g         | highlight the first item                |
| G         | highlight the last item                 |
| K         | move highlighted item up                |
| J         | move highlighted item down              |
| c         | add a new item to the list              |
| e         | edit content of highlighted item        |
| \<Space\> | toggle highlighted item (done/not done) |
| d         | delete highlighted item                 |

## Window Manager Integration

If you want to launch it sort of like an application you can make a keybinding to launch your terminal with `kabmat` running instead of the shell. For example, here is how I do it with `st` and `dwm`.

```bash
st -e sh -c kabmat
```

```c
static Key keys[] = {
    /* modifier                                key                        function        argument */
    ....
    { (Mod4Mask|Mod1Mask),                     XK_t,                      spawn,          SHCMD("st -e sh -c kabmat") },
    ....
};
```
