CC=gcc
CFLAGS=-Wall -Werror -I/usr/include/openssl
LDFLAGS=-lssl -lcrypto

EXE=fetchmail
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

format:
	clang-format -i -style=file src/*.c src/*.h

clean:
	rm -f *.o $(EXE)

.PHONY: all clean format
