
# 定义编译器
CC = gcc

# 定义编译选项
CFLAGS = -Wall

# 定义最终生成的可执行文件名
EXECUTABLE = fetchmail

# 定义需要编译的源文件
SOURCES = main.c connection.c commands.c

# 通过替换 .c 为 .o 从 SOURCES 变量中得到对象文件列表
OBJECTS = $(SOURCES:.c=.o)

# 默认目标
all: $(EXECUTABLE)

# 链接目标，依赖于所有的对象文件
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

# 从每个 .c 文件生成对应的 .o 文件
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

format:
	clang-format -i -style=file *.c *.h

# 清理目标，用于删除所有编译生成的文件
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# 声明伪目标
.PHONY: all clean
