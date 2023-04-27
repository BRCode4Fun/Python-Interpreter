# Get a list of all .cpp files in the root directory and its subdirectories
SRCS := $(shell find . -name "*.cpp")

# Set the output file name
OUTPUT := main

# Set the compiler and compiler flags
CC := g++
CFLAGS := -Wall -Wextra -std=c++20

# Add DEBUG macro to CFLAGS
ifdef DEBUG
CFLAGS += -DDEBUG
endif

# Build rule
all: $(OUTPUT)

# Linking rule
$(OUTPUT): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUTPUT)

# Clean rule
clean:
	rm -f $(OUTPUT)

