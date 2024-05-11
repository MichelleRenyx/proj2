CC = gcc
CFLAGS = -Wall -g -I/home/linuxbrew/.linuxbrew/opt/openssl@3/include
LDFLAGS = -L/home/linuxbrew/.linuxbrew/opt/openssl@3/lib -lssl -lcrypto
DEPFLAGS = -MMD -MP

SOURCES = main.c imap_connection.c email_retrieval.c email_parsing.c command_executor.c error_handling.c tls_support.c utils.c
OBJECTS = $(SOURCES:.c=.o)
DEPS = $(OBJECTS:.o=.d)  # 依赖文件
EXECUTABLE = outputfile

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# 编译规则，包括生成依赖性文件
%.o: %.c
	$(CC) -c $(CFLAGS) $(DEPFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(DEPS)

# 包括自动生成的依赖性文件
-include $(DEPS)

.PHONY: all clean
