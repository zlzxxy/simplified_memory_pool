# simplified_memory_pool

一个基于 C++ 实现的简易内存池项目，用于练习小对象内存复用、按大小分桶分配，以及 placement new / 手动析构等基础机制。

## Features

- 按对象大小分桶管理小块内存
- 小对象走内存池分配，大对象回退到系统分配
- 使用空闲链表复用已释放内存
- 使用 placement new 构造对象
- 使用手动析构 + 内存回收释放对象

## Project Structure

```text
simplified_memory_pool/
├── include/
│   └── MemoryPool.h
├── src/
│   └── MemoryPool.cpp
├── makefile
└── README.md
```

## Build

当前项目只有实现文件，没有测试文件或 `main.cpp`。

在项目根目录下执行：

```bash
make
```

如果后续添加测试文件，例如 `main.cpp`，可以手动编译：

```bash
g++ -std=c++11 -pthread main.cpp src/MemoryPool.cpp -Iinclude -o main
```

## Usage

使用前先初始化内存池：

```cpp
HashBucket::initMemoryPool();
```

申请和释放对象：

```cpp
#include <iostream>
#include "MemoryPool.h"

using namespace memoryPool;

struct TestObj {
    int value;
    TestObj(int v) : value(v) {}
};

int main() {
    HashBucket::initMemoryPool();

    TestObj* obj = newElement<TestObj>(123);
    std::cout << obj->value << std::endl;

    deleteElement(obj);
    return 0;
}
```

## Notes

这是一个偏学习性质的简化版内存池项目，主要用于理解内存池的基本实现思路，欢迎学习C++的同学来交流使用。
