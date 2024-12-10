
CC = gcc
CFLAGS = -I. -Wall -Wextra -g
LIBS = -lcjson

all: harness

harness: harness.c
	$(CC) $(CFLAGS) -o harness harness.c $(LIBS)

clean:
	rm -f harness

afl: harness
	afl-fuzz -i seeds -o findings -- ./harness @@