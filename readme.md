# Simplified Memory Pool

## 项目简介

这是一个简化版的内存池实现，提供固定大小内存块分配和释放机制。项目支持在 `MAX_SLOT_SIZE` 以内的对象使用内存池分配，超出大小则回退到系统 `operator new` / `operator delete`。

## 目录结构

- `include/` - 头文件目录
  - `MemoryPool.h` - 内存池接口与 `newElement` / `deleteElement` 工具模板
- `src/` - 源码目录
  - `MemoryPool.cpp` - 内存池实现
- `test/` - 测试目录
  - `test_memory_pool.cpp` - 内存池功能验证程序
- `makefile` - 构建规则

## 构建与运行

在项目根目录运行：

```bash
make
```

生成中间目标文件到 `build/`。

运行测试程序：

```bash
make test
./test_memory_pool
```

清理构建产物：

```bash
make clean
```

## 代码说明

- `HashBucket::initMemoryPool()` 初始化 64 个不同大小的内存池，槽位大小从 8 字节开始，每项增加 8 字节，最多 512 字节。
- `HashBucket::useMemory(size)` 根据请求大小选择内存池分配，超出 `MAX_SLOT_SIZE` 时使用系统 `operator new`。
- `HashBucket::freeMemory(ptr, size)` 根据大小释放内存池或系统分配的内存。
- `newElement<T>(...)` 与 `deleteElement<T>(...)` 提供对象构造和析构封装。

## 注意事项

- 当前实现假定重复使用固定大小对象，适用于频繁的小对象分配场景。
- 如果需要支持更复杂的内存对齐或更大对象，可以扩展 `MemoryPool` 的块管理逻辑。

