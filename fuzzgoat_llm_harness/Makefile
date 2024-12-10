
CC = afl-gcc
CFLAGS = -I.  # Assuming the library's header files are in the current directory
LIBS = -L. -lsimple_json  # Assuming the library is named libsimple_json.a or libsimple_json.so

harness: harness.c
	$(CC) $(CFLAGS) -o harness harness.c $(LIBS)

afl: harness
	afl-fuzz -i seeds -o findings -- ./harness @@

clean:
	rm -f harness