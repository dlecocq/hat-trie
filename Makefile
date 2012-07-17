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
	# Splitting the data up into the various inputs
	for i in {1,2,4,6,8,{1,2,3,4,5,6,7,8,9}{0,5}}00 ; do
		head -n $i data/distinct_1 > data/distinct_$i
	done
	for i in {1,2,3,4,5,6,7,8,9}{0,5}000 ; do
		head -n $i data/distinct_1 > data/distinct_$i
	done

bench: include/*.hpp bench.cpp data/distinct_1
	$(CPP) $(CPPOPTS) bench.cpp -o bench
	./bench ./data/distinct_{1,2,4,6,8,{1,2,3,4,5,6,7,8,9}{0,5}}00 ./data/distinct_{1,2,3,4,5,6,7,8,9}{0,5}000

test: include/*.hpp test.cpp
	$(CPP) $(CPPOPTS) -Ideps/Catch/single_include test.cpp -o test

clean:
	rm -rdf *.o test bench *.dSYM
