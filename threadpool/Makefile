CC=g++
CFLAGS = -g -Wall
pool:main.o threadpool.o
	$(CC) $(CFLAGS) -o main.o threadpool.o

threadpool.o:ThreadPool.cpp
	$(CC) $(CFLAGS) -c ThreadPool.cpp

main.o:main.cpp
	$(cc) $(CFLAGS) -c main.cpp

.PHONY:clean

clean:
	    -rm -f *.o  pool
