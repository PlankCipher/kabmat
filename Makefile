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
	@echo -e "\033[1mCreating directories for program...\033[m"
	for dirname in $$(find $(SRC_DIR) -type d | sed 's/'$(SRC_DIR)'/'$(BUILD_DIR)'/'); do mkdir -p $$dirname; done
	@echo -e "\033[1mCreating data files in ~/.local/share...\033[m"
	mkdir -p $(DATA_DIR)
	touch $(DATA_DIR)/data
	@echo -e "\033[1mCompiling program (This may take several seconds)...\033[m"
	$(MAKE) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(CFLAGS) -o $@ $(LOADLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ $(LOADLIBS)

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: install
install:
ifeq ($(USER), root)
	@echo -e "\033[1mThe Installing Starts!\033[m"
	$(MAKE)
	@echo -e "\n\033[1mMoving binaries to the /usr/local/...\033[m"
	install -dm755 $(DESTDIR)$(BINDIR) $(DESTDIR)$(MANDIR)/man1
	install -Dm755 ./$(TARGET) $(DESTDIR)$(BINDIR)
	install -Dm644 ./doc/kabmat.1 $(DESTDIR)$(MANDIR)/man1
	$(MAKE) clean
	@echo -e "\n\033[1mEverything is done. Type "kabmat" to start.\033[m"
else
	@echo "You need be root to install."
endif

.PHONY: uninstall
uninstall:
ifeq ($(USER), root)
	@echo -e "\033[1mUninstalling...\033[m"
	rm $(DESTDIR)$(BINDIR)/$(TARGET)
	rm $(DESTDIR)$(MANDIR)/man1/kabmat.1
else
	@echo "You need be root to uninstall."
endif
