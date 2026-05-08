# Detect OS
ifeq ($(OS),Windows_NT)
    OUTPUT := main.exe
else
    OUTPUT := main
endif

# Recursive wildcard to collect source files using relative paths.
# This avoids issues with spaces in absolute paths on Windows.
rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
SRCS := $(call rwildcard,src/,*.cpp)
SRCS := $(filter-out \
    src/libs/value/pyBool.cpp \
    src/libs/value/pyClass.cpp \
    src/libs/value/pyFloat.cpp \
    src/libs/value/pyInstance.cpp \
    src/libs/value/pyInt.cpp \
    src/libs/value/pyNone.cpp \
    src/libs/value/pyStr.cpp, \
    $(SRCS))

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


