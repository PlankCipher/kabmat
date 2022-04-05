# Common prefix for installation directories, following GNU conventions.
# See https://www.gnu.org/prep/standards/html_node/Directory-Variables.html for details.

# Installation prefix
PREFIX = /usr/local
DATAROOTDIR = $(PREFIX)/share

# Where to put the executable.
BINDIR = $(PREFIX)/bin
# Where to put the manual pages.
MANDIR = $(DATAROOTDIR)/man

TARGET = kabmat
SRC_DIR = src
BUILD_DIR = bin
DATA_DIR = ~/.local/share/kabmat

CFLAGS = -std=c++17 -Wall -Wextra

# library linker commands (start with -l)
LOADLIBS = -lncurses

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: all
all:
	for dirname in $$(find $(SRC_DIR) -type d | sed 's/'$(SRC_DIR)'/'$(BUILD_DIR)'/'); do mkdir -p $$dirname; done
	mkdir -p $(DATA_DIR)
	touch $(DATA_DIR)/data
	+$(MAKE) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(CFLAGS) -o $@ $(LOADLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ $(LOADLIBS)

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: install
install:
	$(MAKE)
	install -dm755 $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)/man1
	install -Dm755 ./$(TARGET) $(DESTDIR)$(BINDIR)
	install -Dm644 ./doc/kabmat.1 $(DESTDIR)$(MANDIR)/man1
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: uninstall
uninstall:
	rm $(DESTDIR)$(BINDIR)/$(TARGET)
	rm $(DESTDIR)$(MANDIR)/man1/kabmat.1
