CC = gcc
LD = gcc

CFLAGS = -std=c99 -Iinclude -g
LDFLAGS = `sdl2-config --cflags --libs` -lm -lSDL2_ttf

SRC_FILES=$(wildcard src/*.c)

OBJ_FILES=$(patsubst src/%.c,obj/%.o,$(SRC_FILES))

MAIN_OBJ=obj/main.o

NON_MAIN_OBJ_FILES=$(filter-out $(MAIN_OBJ), $(OBJ_FILES))


all: sir

sir: $(OBJ_FILES)
	$(LD) $(OBJ_FILES) $(LDFLAGS) -o $@ -lm

obj/%.o: src/%.c
	$(CC) -c $(CFLAGS) $< -o $@


