# Compiler and flags
CC=gcc
CFLAGS=-Wall -g
LDFLAGS=-lssl -lcrypto  # Linking flags for OpenSSL if TLS is used

# Source files
SOURCES=main.c imap_connection.c email_retrieval.c email_parsing.c command_executor.c error_handling.c tls_support.c utils.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=fetchmail

# Default target
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# To obtain object files
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# Clean
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Additional rules for other prerequisites
.PHONY: all clean
