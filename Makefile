CC=g++
CFLAGS=-pedantic-errors -g -lpqxx -lpq
SOURCES_EXECUTABLE1=test.cpp
SOURCES_EXECUTABLE2=select.cpp

all: test.o select.o
    
test.o:
	$(CC) $(SOURCES_EXECUTABLE1) $(CFLAGS) -o $@

select.o: 
	$(CC) $(SOURCES_EXECUTABLE2) $(CFLAGS) -o $@

.PHONY: clean

clean:
	rm -f *.o
