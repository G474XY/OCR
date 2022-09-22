# Makefile

CC = gcc
CPPFLAGS = $(shell pkg-config SDL2_image --cflags)
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =
LDLIBS = $(shell pkg-config SDL2_image --libs)

SRC = main.c color.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

main: ${OBJ}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

# END

