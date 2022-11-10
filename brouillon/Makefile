# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -g  `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

<<<<<<< Updated upstream
all: main

SRC = pretraitement.c detect_grid.c main.c
=======
all: pretraitement main rotate

SRC = pretraitement.c main.c rotate.c hough.c
>>>>>>> Stashed changes
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

main: main.o pretraitement.o detect_grid.o

rotate: rotate.o hough.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
