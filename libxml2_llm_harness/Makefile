
CC = afl-clang-fast
CFLAGS = -I/usr/include/libxml2
LDFLAGS = -lxml2
TARGET = harness

all: $(TARGET)

$(TARGET): harness.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

afl: $(TARGET)
	afl-fuzz -i seeds -o findings -- ./${TARGET} @@