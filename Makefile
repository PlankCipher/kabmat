TARGET = kabmat
SRC_DIR = src
BUILD_DIR = bin
CFLAGS = -std=c++17 -Wall -Wextra

# library linker commands (start with -l)
LOADLIBS =

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: all
all:
	for dirname in $$(find $(SRC_DIR) -type d | sed 's/'$(SRC_DIR)'/'$(BUILD_DIR)'/'); do mkdir -p $$dirname; done
	+$(MAKE) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(CFLAGS) -o $@ $(LOADLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ $(LOADLIBS)

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(TARGET)
