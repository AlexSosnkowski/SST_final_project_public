
CC = afl-gcc
CFLAGS = -Wall -I/usr/local/include
LDFLAGS = -L/usr/local/lib -larchive

harness: harness.c
	$(CC) $(CFLAGS) -o harness harness.c $(LDFLAGS)

clean:
	rm -f harness

afl: harness
	afl-fuzz -i seeds -o findings -- ./harness @@