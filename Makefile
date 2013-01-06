CFLAGS=-Wall -std=gnu99 -ggdb -D_THREAD_SAFE -pthread
CC=gcc $(CFLAGS)

# cache
cache.o: cache.c cache.h arenas.o hash.o lru.o
	$(CC) -c cache.c

cache_test: test_cache.c unittest.o cache.o
	$(CC) test_cache.c cache.o unittest.o arenas.o hash.o lru.o -o cache_test
	./cache_test

# cache_entry
cache_entry.o: cache_entry.c cache_entry.h
	$(CC) -c cache_entry.c

cache_entry_test: test_cache_entry.c unittest.o cache_entry.o
	$(CC) test_cache_entry.c cache_entry.o unittest.o -o cache_entry_test
	./cache_entry_test

# slabs
slabs.o: slabs.c slabs.h
	$(CC) -c slabs.c

slabs_test: test_slabs.c unittest.o slabs.o
	$(CC) test_slabs.c slabs.o unittest.o -o slabs_test
	./slabs_test

# arenas
arenas.o: arenas.c arenas.h
	$(CC) -c arenas.c

arenas_test: test_arenas.c unittest.o arenas.o
	$(CC) test_arenas.c arenas.o unittest.o -o arenas_test
	./arenas_test

# hash
hash.o: hash.c hash.h cache_entry.o
	$(CC) -c hash.c

hash_test: unittest.o test_hash.c hash.o
	$(CC) test_hash.c hash.o unittest.o cache_entry.o -o hash_test
	./hash_test

# lru
lru.o: lru.c lru.h cache_entry.o
	$(CC) -c lru.c

lru_test: unittest.o test_lru.c lru.o
	$(CC) test_lru.c lru.o cache_entry.o unittest.o -o lru_test
	./lru_test

# unittest
unittest.o: unittest.c unittest.h
	$(CC) -c unittest.c

unittest_test: test_unittest.c unittest.o
	$(CC) test_unittest.c unittest.o -o unittest_test
	./unittest_test

test: clean unittest_test hash_test slabs_test cache_entry_test arenas_test lru_test cache_test

.PRECIOUS: unittest_test hash_test slabs_test cache_entry_test arenas_test lru_test cache_test

lib: hash.o

clean:
	rm -rf *.o
	rm -f *_test
