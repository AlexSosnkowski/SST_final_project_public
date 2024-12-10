CC = afl-clang
DEPS=harness.c
CFLAGS = -I./libarchive-3.7.4/libarchive
LDFLAGS = -L./libarchive-3.7.4/.libs -larchive

all: $(DEPS)
	$(CC) -o harness $(CFLAGS) $^ $(LDFLAGS)

afl: harness
	afl-fuzz -i in -o out -- ./harness @@

clean:
	rm -f $(TARGET)
