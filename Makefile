CC=g++
CFLAGS=-pedantic-errors -g -lpqxx -lpq
SOURCES_EXECUTABLE1=src/main.cpp src/ParseQuery/ParseQuery.cpp src/ViewTable/ViewTable.cpp

all: main.o

main.o:
	$(CC) $(SOURCES_EXECUTABLE1) $(CFLAGS) -o $@

.PHONY: clean

clean:
	rm -f *.o
