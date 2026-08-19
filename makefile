CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Iinclude -I.
SRC := $(wildcard src/*.c) lexer.c
TARGET=shellforge
$(TARGET): $(SRC) 
	$(CC) $(CFLAGS) $(SRC) -lreadline -o $(TARGET)
clean:
	rm -f $(TARGET)
.PHONY: clean

