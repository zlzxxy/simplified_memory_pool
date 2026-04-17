CXX := g++
CXXFLAGS := -std=c++11 -pthread -Iinclude
SRC_DIR := src
INC_DIR := include
TEST_DIR := test
BUILD_DIR := build

TARGET := MemoryPool.o
TEST_TARGET := test_memory_pool

SRC := $(SRC_DIR)/MemoryPool.cpp
OBJ := $(BUILD_DIR)/$(TARGET)
TEST_SRC := $(TEST_DIR)/test_memory_pool.cpp

all: $(OBJ)

$(OBJ): $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

test: $(OBJ)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) $(OBJ) -o $(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TEST_TARGET)