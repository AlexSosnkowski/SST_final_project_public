
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -larchive

all: harness

harness: harness.c
	$(CC) $(CFLAGS) -o harness harness.c $(LDFLAGS)

clean:
	rm -f harness

afl: harness
	afl-fuzz -i seeds -o findings -- ./harness @@