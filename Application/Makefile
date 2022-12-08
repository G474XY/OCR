#Makefile

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 sdl2 SDL2_image` -Wall -Wextra
LDLIBS = `pkg-config --libs gtk+-3.0 sdl2 SDL2_image` -lm

SRC = application.c ../solver/solver.c ../solver/saver.c ../imageTreatment/apply_image.c ../imageTreatment/pretraitement.c
SRC2 = police.c
EXE = main

all: application

application:
	${CC} ${CFLAGS} ${SRC} -o ${EXE} ${LDLIBS} 

police:
	${CC} ${CFLAGS} ${SRC2} -o ${EXE} ${LDLIBS} 

clean:
	${RM} ${EXE}
