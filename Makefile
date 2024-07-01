CC = gcc
WIN_CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -Wextra -g `sdl2-config --cflags`
LIBS = `sdl2-config --libs` -lSDL2_ttf -lm

SRC = sdliquid.c
OBJ = $(SRC:.c=.o)
EXEC = sdliquid
WIN_EXEC = sdliquid.exe

all: $(EXEC)

win: $(WIN_EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIBS)

$(WIN_EXEC): win_objs
	$(WIN_CC) $(OBJ) -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

win_objs: $(SRC)
	$(WIN_CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(EXEC) $(WIN_EXE)

.PHONY: clean all
