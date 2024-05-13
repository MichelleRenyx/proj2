# CC=gcc
# CFLAGS=-Wall -Werror
# LDFLAGS=-Wall -Werror

# EXE=fetchmail
# SOURCES=$(wildcard *.c)
# OBJECTS=$(SOURCES:.c=.o)

# all: $(EXE)

# $(EXE): $(OBJECTS)
# 	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# format:
# 	clang-format -i -style=file src/*.c src/*.h

# clean:
# 	rm -f *.o $(EXE)

# .PHONY: all clean format
EXE=fetchmail

$(EXE): main.c
	cc -Wall -o $(EXE) $<

format:
	clang-format -style=file -i *.c

# clean:
# 	rm -f *.o $(EXE)

# all: $(EXE)

# .PHONY: all clean format