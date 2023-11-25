# Compilation flags
CC=g++
OPT=-O2
CFLAGS=-c -pedantic-errors -Wall -Wextra -std=c++17 $(OPT)
LDFLAGS=-lpqxx -lpq

# Files for DataBase
BUILD_DIR=obj
SOURCES_DB=src/main.cpp src/Authorization/Authorization.cpp src/ParseQuery/ParseQuery.cpp src/ViewTable/ViewTable.cpp src/LoadDB/LoadDB.cpp src/JsonHandle/JsonHandle.cpp
OBJECTS_DB=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES_DB))
TARGET_DB=DataBase

# Files for AddFirstUser
SOURCES_AFU=src/AddFirstUser.cpp src/LoadDB/LoadDB.cpp src/JsonHandle/JsonHandle.cpp
OBJECTS_AFU=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES_AFU))
TARGET_AFU=AddFirstUser

all: $(TARGET_DB) $(TARGET_AFU)

$(TARGET_DB): $(OBJECTS_DB)
	$(CC) $(OBJECTS_DB) $(LDFLAGS) -o $@

$(TARGET_AFU): $(OBJECTS_AFU)
	$(CC) $(OBJECTS_AFU) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	@find obj -type f -delete
	@rm $(TARGET_DB)
	@rm $(TARGET_AFU)

