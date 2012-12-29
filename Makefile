CFLAGS=-Wall -std=gnu99 -ggdb
CC=gcc $(CFLAGS)

# cache_entry
cache_entry.o: cache_entry.c cache_entry.h
	$(CC) -c cache_entry.c

test_cache_entry: test_cache_entry.c unittest.o cache_entry.o
	$(CC) test_cache_entry.c cache_entry.o unittest.o -o test_cache_entry
	./test_cache_entry

# slabs
slabs.o: slabs.c slabs.h
	$(CC) -c slabs.c

test_slabs: test_slabs.c unittest.o slabs.o
	$(CC) test_slabs.c slabs.o unittest.o -o test_slabs
	./test_slabs

# hash
hash.o: hash.c hash.h cache_entry.o
	$(CC) -c hash.c

test_hash: unittest.o test_hash.c hash.o
	$(CC) test_hash.c hash.o unittest.o cache_entry.o -o test_hash
	./test_hash

# unittest
unittest.o: unittest.c unittest.h
	$(CC) -c unittest.c

test_unittest: test_unittest.c unittest.o
	$(CC) test_unittest.c unittest.o -o test_unittest
	./test_unittest

test: clean test_unittest test_hash test_slabs test_cache_entry

.PRECIOUS: test_unittest test_hash test_slabs test_cache_entry

lib: hash.o

clean:
	rm -rf *.o
	rm -f test_unittest
	rm -f test_hash
	rm -f test_slabs
	rm -f test_cache_entry
