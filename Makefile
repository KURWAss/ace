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

ifdef DISABLED
run: $(TARGET)
	@command -v Xephyr >/dev/null 2>&1 || { echo "Xephyr not found. Install it first (e.g. sudo pacman -S xorg-server-xephyr)."; exit 1; }
	-pkill Xephyr 2>/dev/null
	-pkill -f "DISPLAY=:1 ./$(TARGET)" 2>/dev/null
	sleep 0.5
	Xephyr :1 -screen 1280x800 & \
	sleep 1; \
	DISPLAY=:1 ./$(TARGET) & \
	echo "Xephyr and ace are running on DISPLAY=:1"
endif

help:
	@echo "Usage:"
	@echo "  make          - Build ace"
	# @echo "  make run      - Build ace and run it in Xephyr"
	@echo "  make clean    - Remove object files"

.PHONY: clean help run
