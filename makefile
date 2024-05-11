CC = gcc
CFLAGS = -Wall -g -I/home/linuxbrew/.linuxbrew/opt/openssl@3/include
LDFLAGS = -L/home/linuxbrew/.linuxbrew/opt/openssl@3/lib -lssl -lcrypto

SOURCES = main.c imap_connection.c email_retrieval.c email_parsing.c command_executor.c error_handling.c tls_support.c utils.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = outputfile

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
