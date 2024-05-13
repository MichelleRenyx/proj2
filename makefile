CC=gcc
CFLAGS=-Wall -Werror -I/usr/include/openssl
LDFLAGS=-lssl -lcrypto

EXE=fetchmail
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

format:
	clang-format -i -style=file src/*.c src/*.h

clean:
	rm -f src/*.o $(EXE)

.PHONY: all clean format
