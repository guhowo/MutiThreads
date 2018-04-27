all:sema
CC=g++ -std=c++11
FLAG=-g -W
LIBS=-pthread -lboost_thread

sema:main.o
	$(CC) $(FLAG) -o sema main.cpp $(LIBS)

main.o:main.cpp
	$(CC) $(FLAG) -c main.cpp $(LIBS)

.PHONY:clean

clean:
	-rm -f *.o sema
