# Get a list of all .cpp files in the root directory and its subdirectories
SRCS := $(shell find . -name "*.cpp")

# Set the output file name
OUTPUT :=  main

# Set the compiler and compiler flags
CC := g++
CFLAGS := -std=c++17 -g

# Add DEBUG macro to CFLAGS
ifdef DEBUG
CFLAGS += -DDEBUG -Wall -Wextra
endif

# Build rule
build: $(OUTPUT)

# Linking rule
$(OUTPUT): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(OUTPUT)

test:
	chmod +x test.sh
	./test.sh

# Clean rule
clean:
	@rm -f $(OUTPUT)

