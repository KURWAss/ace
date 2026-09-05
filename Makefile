CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude
LDLIBS := -lX11

SRC_DIR := src
SOURCES := $(SRC_DIR)/main.cpp $(SRC_DIR)/window_manager.cpp $(SRC_DIR)/config.cpp $(SRC_DIR)/ewmh.cpp $(SRC_DIR)/process.cpp $(SRC_DIR)/workspaces.cpp
OBJECTS := $(SOURCES:.cpp=.o)

TARGET := ace

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDLIBS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

help:
	@echo "Usage:"
	@echo "  make          - Build ace"
	@echo "  make clean    - Remove object files"

.PHONY: clean help
