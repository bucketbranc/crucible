# Compiler and flags
CC = gcc
CFLAGS = -Wall -c
LDFLAGS = -lncursesw
EXEC = main

# Default target
all: $(EXEC)

# Linking the object file to create the executable
main: main.o
	$(CC) main.o -o $(EXEC) $(LDFLAGS)

# Compiling the source code into an object file
main.o: main.c
	$(CC) $(CFLAGS) main.c

# Rule to run the executable
run: all
	./$(EXEC)

# Clean up build artifacts
clean:
	rm -f *.o $(EXEC)