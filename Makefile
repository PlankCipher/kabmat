TARGET = kabmat
SRC_DIR = src
BUILD_DIR = bin
CFLAGS = -std=c++17 -Wall -Wextra

# library linker commands (start with -l)
LOADLIBS =

# finds all objects that corrispond to .cpp files (system agnostic)
OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))

.PHONY: all
all:
	mkdir -p $(BUILD_DIR)
	+$(MAKE) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(CFLAGS) -o $@ $(LOADLIBS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ $(LOADLIBS)

.PHONY: clean
clean:
	-rm -rf $(BUILD_DIR) $(TARGET)
