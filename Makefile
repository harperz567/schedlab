# SchedLab build file
# Uses > as the recipe prefix instead of a tab
.RECIPEPREFIX = >

CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -g

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
HDR = $(wildcard src/*.h)
TARGET = schedlab

all: $(TARGET)

$(TARGET): $(OBJ)
> $(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c $(HDR)
> $(CC) $(CFLAGS) -c $< -o $@

clean:
> rm -f $(OBJ) $(TARGET)

.PHONY: all clean
