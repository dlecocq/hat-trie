CPP	 = g++
CPPOPTS = -O3 -Wall -Iinclude -I/Users/dan/playground/hat-trie-chris/hat-trie/src

all: test bench

# Retrieve some of the test data from Dr. Askitis
data/distinct_1:
	# Create a directory
	mkdir -p data
	# Downloading the dataset
	curl http://www.naskitis.com/distinct_1.bz2 -o data/distinct_1.bz2
	# Unpacking the data
	bunzip2 data/distinct_1.bz2

bench: include/*.hpp bench.cpp data/distinct_1
	$(CPP) $(CPPOPTS) bench.cpp -o bench
	./bench ./data/distinct_1 | tee -a bench.dat

test: include/*.hpp test.cpp
	$(CPP) $(CPPOPTS) -Ideps/Catch/single_include test.cpp -o test

clean:
	rm -rdf *.o test bench *.dSYM
