# Detect OS
ifeq ($(OS),Windows_NT)
    EXE_EXT := .exe
    OBJ_DIR := bin\obj
    BIN_DIR := bin
    CC := cl
    CFLAGS := /nologo /W3 /EHsc
    OBJ_EXT := .obj
else
    EXE_EXT :=
    OBJ_DIR := bin/obj
    BIN_DIR := bin
    CC := g++
    CFLAGS := -std=c++17 -Wall -Wextra -O2
    OBJ_EXT := .o
endif

# Source files
SRC := $(shell find src -name "*.cpp")
OBJ := $(patsubst src/%.cpp,$(OBJ_DIR)/%$(OBJ_EXT),$(SRC))

# Default target
all: $(BIN_DIR)/main$(EXE_EXT)

# Create directories if missing
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compile objects
$(OBJ_DIR)/%$(OBJ_EXT): src/%.cpp | $(OBJ_DIR)
ifeq ($(CC),cl)
	cl /c /Fo$@ $< $(CFLAGS)
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

# Link
$(BIN_DIR)/main$(EXE_EXT): $(OBJ) | $(BIN_DIR)
ifeq ($(CC),cl)
	cl $(CFLAGS) $(OBJ) /Fe:$@
else
	$(CC) $(OBJ) -o $@
endif

# Clean
.PHONY: clean
clean:
ifeq ($(CC),cl)
	del /Q $(OBJ_DIR)\*$(OBJ_EXT) $(BIN_DIR)\main$(EXE_EXT)
else
	rm -rf $(OBJ_DIR)/*.o $(BIN_DIR)/main$(EXE_EXT)
endif
