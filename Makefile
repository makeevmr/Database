# Compilation flags
CC=g++
OPT=-O2
CFLAGS=-c -pedantic-errors -Wall -Wextra -std=c++17 $(OPT)
LDFLAGS=-lpqxx -lpq

# Files
BUILD_DIR=obj
SOURCES=src/main.cpp src/ParseQuery/ParseQuery.cpp src/ViewTable/ViewTable.cpp src/LoadDB/LoadDB.cpp src/JsonHandle/JsonHandle.cpp
OBJECTS=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
TARGET=DataBase

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	@find obj -type f -delete
	@rm $(TARGET)

