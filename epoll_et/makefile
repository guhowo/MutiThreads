CFLGS = -g -W
OBJS = main.o
CC = gcc
LIB=-lpthread

epthread : ${OBJS}
	$(CC) ${CFLGS} -o $@ ${OBJS} $(LIB)

${OBJS} : main.c
	$(CC) ${CFLGS} -c main.c

.PHONY:clean

clean:
	-rm -f ${OBJS} test
