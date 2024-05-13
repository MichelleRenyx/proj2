CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lssl -lcrypto

all: main

main: main.o connection.o commands.o
	$(CC) -o main main.o connection.o commands.o $(LDFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS)

connection.o: connection.c
	$(CC) -c connection.c $(CFLAGS)

commands.o: commands.c
	$(CC) -c commands.c $(CFLAGS)

clean:
	rm -f *.o main

.PHONY: all clean
