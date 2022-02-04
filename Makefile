TARGET = kabmat
SRC_DIR = src
BUILD_DIR = bin
INSTALL_DIR = /usr/bin/
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
	sudo cp ./$(TARGET) $(INSTALL_DIR)
