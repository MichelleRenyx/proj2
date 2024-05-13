# CC = gcc
# CFLAGS = -Wall -g -I/usr/include/openssl
# LDFLAGS = -lssl -lcrypto

# # 添加所有的源文件
# SOURCES = main.c connection.c commands.c utility.c
# OBJECTS = $(SOURCES:.c=.o)
# EXECUTABLE = fetchmail

# all: $(EXECUTABLE)

# $(EXECUTABLE): $(OBJECTS)
# 	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# %.o: %.c
# 	$(CC) -c $(CFLAGS) $< -o $@

# clean:
# 	rm -f $(OBJECTS) $(EXECUTABLE)

# .PHONY: all clean

CC = gcc
CFLAGS = -Wall -g -I/usr/include/openssl
LDFLAGS = -lssl -lcrypto

# Sources and objects
SOURCES = src/*.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = $(wildcard *.h)
EXECUTABLE = fetchmail

# Main target
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile each source file to an object
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Phony targets
.PHONY: all clean

# Format source files
format:
	clang-format -style=file -i $(SOURCES) $(HEADERS)
