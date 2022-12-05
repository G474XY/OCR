# Makefile

CC = gcc
CPPFLAGS = -lm
CFLAGS = -Wall -Wextra -g -o2 -fsanitize=address
LDFLAGS =
LDLIBS = -lm

SRC = main.c NN.c NNsave.c NNtraining.c allocfree.c
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
