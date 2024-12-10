LIBXML_PATH = ./libxml2-2.9.14
INCLUDE_PATH = $(LIBXML_PATH)/include/
# INCLUDE_PATH = /usr/local/include/
LIB_PATH = $(LIBXML_PATH)/

CC = afl-gcc
CFLAGS = -I$(INCLUDE_PATH) -g -O0 -fprofile-arcs -ftest-coverage
LDFLAGS = -L$(LIB_PATH) -lxml2 -fprofile-arcs -ftest-coverage

all: fuzz_harness

fuzz_harness: harness.c
	$(CC) $(CFLAGS) -o fuzz_harness harness.c $(LDFLAGS)

clean:
	rm -f fuzz_harness *.gcda *.gcno



