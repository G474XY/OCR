#Makefile

CC = gcc
CPPFLAGS = -lm
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS = -lm

SRC = NNsave.c NNtraining.c allocfree.c NN.c
EXE = main

all:
	${CC} ${CFLAGS} ${SRC} -o ${EXE} ${LDLIBS}

clean:
	${RM} ${EXE}