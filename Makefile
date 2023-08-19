# ---------------------------------------------------------------------------
# Wickit v1.0.0
# 
# Build output = ./build/wickit.exe
# Build artifacts located in ./build/artifacts/
# Max Derbenwick 2023
# ---------------------------------------------------------------------------

# Define compiler and arguments
CXX			= g++
CXXFLAGS	= -std=c++17 -Wall -Isrc -g

# Define source, build, and artifact directories
SRC_DIR		= src
BUILD_DIR	= build
OBJ_DIR		= $(BUILD_DIR)/artifacts

# Locate all source files
SRCS = $(shell find src -name '*.cpp')

# Locate all object files (build artifacts)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Locate target executable
TARGET = $(BUILD_DIR)/wickit

# All = build target
all: $(TARGET) lalrgen

# Build target by linking object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build each object file from source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Make LALRGEN sub-project
lalrgen:
	make -C lalrgen all

# Phony target to clean build artifacts
.PHONY: clean lalrgen

# Clean up by deleteing build (and artifact) directory
clean:
	make -C lalrgen clean
	rm -rf $(BUILD_DIR)

# ---------------------------------------------------------------------------
# Max Derbenwick 2023
# ---------------------------------------------------------------------------
