# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable
TARGET = schedule.o

# Source files
SRC = schedule.c

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)
