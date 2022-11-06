# Makefile

CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =

SRC = XorNN.c NNsaveV2.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

XorNN: ${OBJ}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} test

# END
