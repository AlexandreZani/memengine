test_unittest: test_unittest.o
	gcc test_unittest.o unittest.o -o test_unittest
	./test_unittest

test_unittest.o: test_unittest.c unittest.o
	gcc -c test_unittest.c

unittest.o: unittest.c
	gcc -c unittest.c

test: clean test_unittest

clean:
	rm -rf *.o
