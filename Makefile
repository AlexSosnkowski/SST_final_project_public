
CC = afl-clang-fast
CFLAGS = -I.  # Include the current directory for the JSON library header
LDFLAGS = -L. -ljson  # Link against the locally compiled JSON library

all: harness

harness: harness.c
	$(CC) $(CFLAGS) -o harness harness.c $(LDFLAGS)

afl: harness
	afl-fuzz -i seeds -o findings -- ./harness @@

clean:
	rm -f harness