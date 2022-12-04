#Makefile

CC = gcc
CPPFLAGS = -lm
CFLAGS = -Wall -Wextra -g
LDFLAGS =
LDLIBS = -lm

TRN = NNtraining.c allocfree.c
SRC = NNsave.c NN.c ${TRN}
EXE = main

all:
	${CC} ${CFLAGS} ${SRC} -o ${EXE} ${LDLIBS}

clean:
	${RM} ${EXE}
