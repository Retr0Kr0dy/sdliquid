CC = gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_ttf -lm

SRC = sdliquid.c
OBJ = $(SRC:.c=.o)
EXEC = sdliquid

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: clean all
