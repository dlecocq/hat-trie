CPP	 = g++
CPPOPTS = -O3 -Wall -g -Iinclude

all: driver

driver: include/*.hpp src/driver.cpp
	$(CPP) $(CPPOPTS) src/driver.cpp -o build/driver

clean:
	rm -rdf *.o driver *.dSYM

test: include/*.hpp test.cpp
	$(CPP) $(CPPOPTS) -Ideps/Catch/single_include test.cpp -o test
	# Now invoke
	./test
