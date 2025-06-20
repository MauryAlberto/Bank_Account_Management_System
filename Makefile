# Makefile for Bank Account Management System

# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Iinclude

# Source and output files
SRC_DIR := src
OBJ_DIR := build
BIN := bank_system

# Find all .cpp files and generate corresponding .o files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(BIN)

# Link object files to create the final binary
$(BIN): $(OBJS)
	@echo "Linking..."
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean all generated files
clean:
	rm -rf $(OBJ_DIR) $(BIN)

# Run the program
run: all
	./$(BIN)

.PHONY: all clean run
