CC = gcc
CFLAGS = -Wall -Wextra -02
LDFLAGS =
LDLIBS =

SRC = XorNN.c NNsaveV2.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

xor: ${OBJ}

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} xor