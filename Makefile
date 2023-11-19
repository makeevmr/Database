CC=g++
CFLAGS=-pedantic-errors -g -lpqxx -lpq
SOURCES_EXECUTABLE1=src/test.cpp src/ParseQuery/ParseQuery.cpp
# SOURCES_EXECUTABLE2=select.cpp

all: test.o

test.o:
	$(CC) $(SOURCES_EXECUTABLE1) $(CFLAGS) -o $@

# select.o: 
# 	$(CC) $(SOURCES_EXECUTABLE2) $(CFLAGS) -o $@

.PHONY: clean

clean:
	rm -f *.o
