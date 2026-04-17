#include <iostream>
#include <cassert>
#include "MemoryPool.h"

using namespace memoryPool;

struct TestObj {
    int value;
    static int constructed;
    static int destroyed;

    TestObj(int v = 0) : value(v) {
        ++constructed;
    }

    ~TestObj() {
        ++destroyed;
    }
};

int TestObj::constructed = 0;
int TestObj::destroyed = 0;

int main() {
    HashBucket::initMemoryPool();

    // Test simple allocation and deallocation through newElement/deleteElement
    int* intPtr = newElement<int>(42);
    assert(intPtr != nullptr);
    assert(*intPtr == 42);
    deleteElement(intPtr);

    TestObj* objPtr = newElement<TestObj>(123);
    assert(objPtr != nullptr);
    assert(objPtr->value == 123);
    deleteElement(objPtr);
    assert(TestObj::constructed == 1);
    assert(TestObj::destroyed == 1);

    // Test fallback path for large allocations
    size_t largeSize = MAX_SLOT_SIZE + 16;
    void* largePtr = HashBucket::useMemory(largeSize);
    assert(largePtr != nullptr);
    HashBucket::freeMemory(largePtr, largeSize);

    // Allocate and free many objects to exercise the pool
    const int kCount = 1000;
    TestObj* objects[kCount];
    for (int i = 0; i < kCount; ++i) {
        objects[i] = newElement<TestObj>(i);
        assert(objects[i] != nullptr);
        assert(objects[i]->value == i);
    }
    for (int i = 0; i < kCount; ++i) {
        deleteElement(objects[i]);
    }

    assert(TestObj::constructed == 1 + kCount);
    assert(TestObj::destroyed == 1 + kCount);

    std::cout << "MemoryPool test passed.\n";
    return 0;
}
