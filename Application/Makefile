#Makefile

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Wextra
LDLIBS = `pkg-config --libs gtk+-3.0`

SRC = application.c
SRC2 = police.c
EXE = main


application:
	${CC} ${CFLAGS} ${SRC} -o ${EXE} ${LDLIBS} 

police:
	${CC} ${CFLAGS} ${SRC2} -o ${EXE} ${LDLIBS} 

clean:
	${RM} ${EXE}
