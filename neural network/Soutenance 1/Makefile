# Makefile

CC = gcc
CPPFLAGS = -lm
CFLAGS = -Wall -Wextra -o2
LDFLAGS =
LDLIBS = -lm

SRC = main.c XorNN.c NNsave.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: 
	${CC} ${CFLAGS} ${SRC} -o main ${LDLIBS}

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} main

# END
