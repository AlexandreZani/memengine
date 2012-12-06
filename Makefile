CFLAGS=-Wall -std=gnu99
CC=gcc $(CFLAGS)

# hash
hash.o: hash.c hash.h
	$(CC) -c hash.c

test_hash.o: test_hash.c
	$(CC) -c test_hash.c

test_hash: unittest.o test_hash.o hash.o
	$(CC) test_hash.o hash.o unittest.o -o test_hash
	./test_hash

# unittest
unittest.o: unittest.c unittest.h
	$(CC) -c unittest.c

test_unittest.o: unittest.o test_unittest.c
	$(CC) -c test_unittest.c

test_unittest: test_unittest.o
	$(CC) test_unittest.o unittest.o -o test_unittest
	./test_unittest

test: clean test_unittest test_hash

lib: hash.o

clean:
	rm -rf *.o
	rm -f test_unittest
	rm -f test_hash
