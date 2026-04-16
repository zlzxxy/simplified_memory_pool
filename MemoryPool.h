namespace memoryPool {
const int MEMORY_POOL_NUM = 64;
const int SLOT_BASE_SIZE = 8;
const int MAX_SLOT_SIZE = 512;

//定义链表节点
struct Slot {
    Slot* next;
};

class MemoryPool {
public:
    MemoryPool(size_t BlockSize = 4096);
    ~MemoryPool();

    void init(size_t);

    void* allocate(); //void*表示无类型指针，可以指向任何类型的数据
    void deallocate(void*);
private:
    void allocateNewBlock();
    size_t padPointer(char* p, size_t align);

private:
    int BlockSize_;
    int SlotSize_;

    //因为是链表节点，所以要用*
    Slot* firstBlock_;
    Slot* curSlot_;
    Slot* freeList_;
    Slot* lastSlot_;

    std::mutex mutexForFreeList_;
    std::mutex mutexForBlock_;
};

class HashBucket {
    public:
    static void initMemoryPool();
    static MemoryPool& getMemoryPool(int index);

    //申请一块大小为size的内存
    static void* useMemory(size_t size) {
        if (size == 0)
            return nullptr;
        if (size > MAX_SLOT_SIZE)
            //operator new() 内存分配函数，只负责分配原始内存，不会调用构造函数
            return operator new(size);

        return getMemoryPool(((size + 7) / SLOT_BASE_SIZE) - 1).allocate();
    }

    static void freeMemory(void* ptr, size_t size) {
        if (!ptr)
            return;
        if (size > MAX_SLOT_SIZE) {
            operator delete(ptr);
            return;
        }
        
        getMemoryPool(((size + 7) / SLOT_BASE_SIZE) - 1).deallocate(ptr);
    }

    //向内存池申请内存
    template<typename T, typename... Args>
    friend T* newElement(Args&&... args);

    //将申请的内存进行回收操作
    template<typename T>
    friend void deleteElement(T* p);
};

template<typename T, typename... Args>
T* newElement(Args&&... args) {
    T* p = nullptr;
    if ((p = reinterpret_cast<T*>(HashBucket::useMemory(sizeof(T)))) != nullptr)
        new(p) T(std::forward<Args>(args)...);

    return p;
}

template<typename T>
void deleteElement(T* p) {
    if (p) {
        //手动把p指向的对象销毁掉
        p->~T();
        HashBucket::freeMemory(reinterpret_cast<void*>(p), sizeof(T));
    }
}
}