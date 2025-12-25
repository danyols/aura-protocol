CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g

SRC_DIR = src
BUILD_DIR = build

TARGET = aura_client

SRCS = $(SRC_DIR)/main.cpp

# building the client
$(TARGET): $(SRCS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(BUILD_DIR)/$(TARGET)

run: $(TARGET)
	./$(BUILD_DIR)/$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: run valgrind clean