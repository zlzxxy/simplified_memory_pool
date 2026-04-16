all: MemoryPool.o

MemoryPool.o: MemoryPool.cpp MemoryPool.h
	g++ -std=c++11 -pthread -c MemoryPool.cpp -o MemoryPool.o

clean:
	rm -f MemoryPool.o