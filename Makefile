# Detect OS
ifeq ($(OS),Windows_NT)
    # Windows settings
    SRCS := $(shell powershell -Command "Get-ChildItem -Recurse -Filter *.cpp | Select-Object -ExpandProperty FullName" | tr '\r\n' ' ')
    OUTPUT := main.exe
else
    # Unix-like settings (Linux, macOS, etc.)
    SRCS := $(shell find . -name "*.cpp")
    OUTPUT := main
endif

# Set the compiler and compiler flags
CC := g++
CFLAGS := -std=c++20

# Add DEBUG macro to CFLAGS
ifdef DEBUG
CFLAGS += -DDEBUG -Wall -Wextra -g
CFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-reorder
endif

# Build rule
build: $(OUTPUT)

# Linking rule
$(OUTPUT):
	$(CC) $(CFLAGS) $(SRCS) -o $(OUTPUT)

# Test rule with optional TEST variable
# Usage: make test TEST=<test_name>
test:
	chmod +x test.sh
	./test.sh $(TEST)

# Clean rule
clean:
ifeq ($(OS),Windows_NT)
	@powershell -Command "Remove-Item -Force -ErrorAction SilentlyContinue $(OUTPUT)"
else
	@rm -f $(OUTPUT)
endif


