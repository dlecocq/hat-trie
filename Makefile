CPP	 = g++
CPPOPTS = -O9 -Wall -Iinclude -I/Users/dan/playground/hat-trie-chris/hat-trie/src

all: test bench

bench: include/*.hpp bench.cpp
	$(CPP) $(CPPOPTS) bench.cpp -o bench

clean:
	rm -rdf *.o test bench *.dSYM

test: include/*.hpp test.cpp
	$(CPP) $(CPPOPTS) -Ideps/Catch/single_include test.cpp -o test
	# Now invoke
	#./test
