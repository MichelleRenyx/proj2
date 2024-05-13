CC = gcc
CFLAGS = -Wall -g -I/usr/include/openssl
LDFLAGS = -lssl -lcrypto

# 添加所有的源文件
SOURCES = main.c connection.c commands.c utility.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = fetchmail

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
