CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pthread -Iinclude

SRC = src/MemoryPool.cpp
OBJ = MemoryPool.o

all: $(OBJ)

$(OBJ): $(SRC) include/MemoryPool.h
	$(CXX) $(CXXFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ)

rebuild: clean all