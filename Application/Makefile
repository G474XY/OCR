#Makefile

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Wextra
LDLIBS = `pkg-config --libs gtk+-3.0`

SRC = application.c
EXE = main

all:
	${CC} ${CFLAGS} ${SRC} -o ${EXE} ${LDLIBS} 

clean:
	${RM} ${EXE}
