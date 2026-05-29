# Compiler and flags
CC      := gcc
DEBUG_FLAGS = -g -O0 -fsanitize=address
RELEASE_FLAGS = -O2
CFLAGS  := -Wall -Wextra -std=c11 
LDLIBS = -lm

# Directories
BUILD_DIR := build
BIN_DIR   := bin

# Final executable name
TARGET := $(BIN_DIR)/main

# Source and object files
SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS))

# Default target
all: release

release: CFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

# Link executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ $(LDLIBS) -o $@

# Compile .c -> build/.o
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if they do not exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Cleanup
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Convenience rebuild target
rebuild: clean all

.PHONY: all clean rebuild