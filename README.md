# simplified_memory_pool

一个基于 C++ 实现的简易内存池项目，用于练习自定义内存管理、对象构造与析构、空闲链表复用，以及按对象大小分桶分配的基本思想

## 项目简介

在频繁创建和销毁小对象的场景中，直接使用 `new` / `delete` 往往会带来较高的分配开销。这个项目实现了一个简化版内存池，对小块内存进行分桶管理：

- 小对象优先从内存池中申请
- 释放后的内存进入空闲链表，后续可直接复用
- 大对象不进入内存池，直接走 `operator new` / `operator delete`

这个项目适合用于学习以下内容：

- C++ 内存管理基础
- placement new
- 手动析构
- 按大小分桶的内存池设计
- 空闲链表管理
- 简单线程安全控制

## 项目结构

```text
simplified_memory_pool/
├── include/
│   └── MemoryPool.h
├── src/
│   └── MemoryPool.cpp
├── makefile
└── README.md
```

## 核心设计

### 1. 分桶管理

项目中定义了：

- `MEMORY_POOL_NUM = 64`
- `SLOT_BASE_SIZE = 8`
- `MAX_SLOT_SIZE = 512`

这表示内存池维护了 64 个池子，分别管理：

- 8 字节
- 16 字节
- 24 字节
- ...
- 512 字节

当申请内存时，会根据对象大小选择对应的池子。

例如：

- `sizeof(T) = 8`，进入第 1 个池
- `sizeof(T) = 16`，进入第 2 个池
- `sizeof(T) = 20`，向上对齐后进入 24 字节对应的池

### 2. 小对象与大对象分配策略

项目中的 `HashBucket::useMemory(size_t size)` 采用了两种路径：

#### 小对象：`size <= 512`

从对应的内存池中申请内存。

#### 大对象：`size > 512`

直接调用系统分配：

```cpp
operator new(size)
```

释放时同理：

```cpp
operator delete(ptr)
```

### 3. 空闲链表复用

当对象被释放时，并不会立刻把内存还给操作系统，而是挂回当前池子的空闲链表 `freeList_`。

下次再申请同样大小的对象时，会优先从 `freeList_` 取出可复用节点，从而减少重复分配开销。

### 4. 分块申请

当当前块空间不足时，内存池会调用 `allocateNewBlock()` 一次性向系统申请一整块内存，并在这块大内存中按槽位切分，供后续对象使用。

### 5. 对象构造与析构

该项目没有直接暴露普通 `new` / `delete`，而是提供了两个模板接口：

```cpp
template<typename T, typename... Args>
T* newElement(Args&&... args);

template<typename T>
void deleteElement(T* p);
```

其中：

- `newElement<T>()`：先申请原始内存，再用 placement new 构造对象
- `deleteElement<T>()`：先手动调用析构函数，再把内存还回池子

这也是内存池项目中非常重要的一点：

> 分配原始内存 和 构造对象 是两件事；
> 销毁对象 和 释放原始内存 也是两件事。

## 主要接口

### `MemoryPool`

负责单个固定大小槽位的内存管理。

主要成员函数：

- `void init(size_t size)`：初始化槽位大小
- `void* allocate()`：申请一个槽位
- `void deallocate(void* ptr)`：释放一个槽位

### `HashBucket`

负责按大小选择合适的内存池。

主要成员函数：

- `static void initMemoryPool()`：初始化全部内存池
- `static MemoryPool& getMemoryPool(int index)`：获取指定池
- `static void* useMemory(size_t size)`：按大小申请内存
- `static void freeMemory(void* ptr, size_t size)`：按大小释放内存

### 模板接口

```cpp
newElement<T>(args...)
deleteElement<T>(ptr)
```

这是项目对外最推荐使用的接口。

## 使用方式

### 1. 包含头文件

```cpp
#include "MemoryPool.h"
using namespace memoryPool;
```

### 2. 初始化内存池

在正式使用之前，需要先初始化：

```cpp
HashBucket::initMemoryPool();
```

### 3. 申请对象

```cpp
int* p = newElement<int>(42);
```

或者：

```cpp
struct Test {
    int x;
    Test(int v) : x(v) {}
};

Test* t = newElement<Test>(10);
```

### 4. 释放对象

```cpp
deleteElement(p);
deleteElement(t);
```

## 编译方式

当前项目只有实现文件，没有 `main.cpp` 或测试文件，因此 `make` 的作用通常是先把实现文件编译成目标文件。

如果你的 `makefile` 配置的是编译目标文件，那么直接在项目根目录执行：

```bash
make
```

如果你之后新增测试文件，例如 `main.cpp`，也可以手动编译：

```bash
g++ -std=c++11 -pthread main.cpp src/MemoryPool.cpp -Iinclude -o main
```

运行：

```bash
./main
```

## 示例代码

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

## 项目特点

- 按对象大小分桶管理小块内存
- 支持空闲链表复用
- 支持大对象回退到系统分配
- 使用 placement new 构造对象
- 使用手动析构 + 回收内存的方式释放对象
- 使用 `std::mutex` 对空闲链表和块分配做了基础保护

## 当前项目的定位

这是一个偏学习性质的简化版内存池项目，适合理解内存池的基础原理，但它并不是工业级实现。

目前更偏向于：

- 学习自定义分配器思想
- 理解小对象池化管理
- 练习 C++ 底层内存操作
- 为后续学习 STL allocator、线程池、服务器项目打基础

## 可以继续优化的方向

后续可以考虑继续完善：

- 增加完整测试代码
- 优化 `Makefile`
- 增加 benchmark，对比 `new/delete` 与内存池的性能
- 改进线程安全策略
- 增加更详细的注释和设计文档
- 支持更灵活的块大小配置
- 引入单元测试框架

## 适合谁看

这个项目适合：

- 正在学习 C++ 内存管理的同学
- 想理解内存池基本实现思路的初学者
- 想做 C++ 后端 / 中间件方向项目练习的人

## 说明

本项目主要用于学习和练手，代码实现以清晰理解核心机制为主。

如果你也在学习 C++，欢迎交流和改进。
