# Makefile

CC = gcc
CPPFLAGS =
CFLAGS = -Wall -Wextra -g  -fsanitize=leak `pkg-config --cflags sdl2 SDL2_image`
LDFLAGS =
LDLIBS = `pkg-config --libs sdl2 SDL2_image` -lm

all: main

SRC = main.c
all: main

SRC = main.c rotate.c hough.c
OBJ = ${SRC:.c=.o}
EXE = ${SRC:.c=}

main: main.o rotate.o hough.o

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${EXE}

# END
