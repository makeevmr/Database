# Compilation flags
CC=g++
OPT=-O2
CFLAGS=-c -pedantic-errors -Wall -Wextra -std=c++17 $(OPT)
LDFLAGS=-lpqxx -lpq

# Files for ServerDataBase
BUILD_DIR=obj
SOURCES_SDB=src/ServerDataBase.cpp src/Authorization/Authorization.cpp src/ParseQuery/ParseQuery.cpp src/ParseTableToStr/ParseTableToStr.cpp src/LoadDB/LoadDB.cpp src/JsonHandle/JsonHandle.cpp
OBJECTS_SDB=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES_SDB))
TARGET_SDB=ServerDataBase

# Files for ClientDataBase
SOURCES_CDB=src/ClientDataBase.cpp src/LoadDB/LoadDB.cpp src/JsonHandle/JsonHandle.cpp
OBJECTS_CDB=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES_CDB))
TARGET_CDB=ClientDataBase

# Files for AddFirstUser
SOURCES_AFU=src/AddFirstUser.cpp src/LoadDB/LoadDB.cpp src/JsonHandle/JsonHandle.cpp
OBJECTS_AFU=$(patsubst %.cpp, $(BUILD_DIR)/%.o, $(SOURCES_AFU))
TARGET_AFU=AddFirstUser

all: $(TARGET_SDB) $(TARGET_CDB) $(TARGET_AFU)

$(TARGET_SDB): $(OBJECTS_SDB)
	$(CC) $(OBJECTS_SDB) $(LDFLAGS) -o $@

$(TARGET_CDB): $(OBJECTS_CDB)
	$(CC) $(OBJECTS_CDB) $(LDFLAGS) -o $@

$(TARGET_AFU): $(OBJECTS_AFU)
	$(CC) $(OBJECTS_AFU) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	@find obj -type f -delete
	@rm $(TARGET_SDB)
	@rm $(TARGET_CDB)
	@rm $(TARGET_AFU)

