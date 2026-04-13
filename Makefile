CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lm

SRC = lexer.c parser.c symtable.c functions.c eval.c main.c
OBJ = $(SRC:.c=.o)
TARGET = eval_math

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
