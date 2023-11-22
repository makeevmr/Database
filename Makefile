CC=g++
CFLAGS=-c -pedantic-errors -Wall -Wextra -std=c++20 
LPQFLAGS=-lpqxx -lpq
SOURCES=src/main.cpp src/ParseQuery/ParseQuery.cpp src/ViewTable/ViewTable.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=DataBase.o

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LPQFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)
