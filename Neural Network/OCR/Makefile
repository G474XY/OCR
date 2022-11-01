CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =

SRC = XorNN.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

xor: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} xor