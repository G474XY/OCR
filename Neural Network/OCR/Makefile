CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
LDLIBS =

SRC = XorNN.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

xor: ${OBJ}

spam:
	./a.out
	./a.out
	./a.out
	./a.out
	./a.out
	./a.out
	./a.out
	./a.out
	./a.out
	./a.out

clean:
	${RM} ${OBJ}
	${RM} ${DEP}
	${RM} xor