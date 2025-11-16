# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Executable name (Linux: bin/main, Windows: bin/main.exe)
TARGET := bin/main
ifeq ($(OS),Windows_NT)
    TARGET := bin/main.exe
endif

# Find all .cpp files recursively
SRC := $(shell find src -name '*.cpp')

# Convert each src/*.cpp → bin/obj/src/*.o
OBJ := $(SRC:src/%.cpp=bin/obj/%.o)

# Default rule
all: $(TARGET)

# Link step
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Build rule for each .o
bin/obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Cleanup
.PHONY: clean
clean:
	rm -rf bin/obj
	rm -f bin/main bin/main.exe
