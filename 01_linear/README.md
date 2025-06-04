# 线性数据结构教程

## 概述

线性数据结构是数据结构中最基础和最重要的一类，其特点是数据元素之间存在一对一的线性关系。每个元素最多只有一个前驱和一个后继元素。线性数据结构是学习其他复杂数据结构的基础。

## 核心概念

### 线性表的定义
线性表是由n(n≥0)个数据元素组成的有限序列，记作：(a₁, a₂, ..., aₙ)
- 当n=0时，称为空表
- 当n>0时，a₁是第一个元素，aₙ是最后一个元素
- 除第一个元素外，每个元素有且仅有一个前驱
- 除最后一个元素外，每个元素有且仅有一个后继

### 线性表的特性
1. **有序性**：元素之间有明确的顺序关系
2. **有限性**：元素个数是有限的
3. **同质性**：所有元素具有相同的数据类型
4. **可变性**：长度可以动态变化

## 本章内容

### 1. [动态数组 (Vector)](./vector/)
**核心特点**：
- 连续内存存储
- 支持随机访问
- 动态扩容
- 高效的尾部操作

**时间复杂度**：
- 访问：O(1)
- 插入/删除（尾部）：O(1) 摊销
- 插入/删除（中间）：O(n)
- 查找：O(n)

**适用场景**：
- 需要频繁随机访问元素
- 主要在尾部进行插入删除操作
- 内存使用要求较高的场景

### 2. [链表 (Linked List)](./linked_list/)
**核心特点**：
- 非连续内存存储
- 动态内存分配
- 插入删除操作灵活
- 不支持随机访问

**时间复杂度**：
- 访问：O(n)
- 插入/删除（已知位置）：O(1)
- 插入/删除（查找后）：O(n)
- 查找：O(n)

**适用场景**：
- 频繁插入删除操作
- 数据大小变化较大
- 不需要随机访问的场景

### 3. [栈 (Stack)](./stack/)
**核心特点**：
- 后进先出（LIFO）
- 只能在栈顶操作
- 受限的线性表

**时间复杂度**：
- 入栈/出栈：O(1)
- 查看栈顶：O(1)

**适用场景**：
- 函数调用管理
- 表达式求值
- 括号匹配
- 深度优先搜索
- 回溯算法

### 4. [队列 (Queue)](./queue/)
**核心特点**：
- 先进先出（FIFO）
- 队尾入队，队头出队
- 受限的线性表

**时间复杂度**：
- 入队/出队：O(1)
- 查看队头/队尾：O(1)

**适用场景**：
- 任务调度
- 广度优先搜索
- 缓冲区管理
- 打印队列

## 数据结构对比

| 数据结构 | 存储方式 | 随机访问 | 插入删除 | 内存开销 | 缓存友好性 |
|----------|----------|----------|----------|----------|------------|
| 动态数组 | 连续 | O(1) | O(n) | 低 | 高 |
| 链表 | 分散 | O(n) | O(1) | 高 | 低 |
| 栈 | 连续/分散 | 受限 | O(1) | 低/高 | 高/低 |
| 队列 | 连续/分散 | 受限 | O(1) | 低/高 | 高/低 |

## 选择指南

### 选择动态数组的情况
- 需要频繁的随机访问
- 主要在尾部进行插入删除
- 对内存使用效率要求高
- 需要与C风格数组兼容

### 选择链表的情况
- 频繁在中间位置插入删除
- 数据大小变化很大
- 不需要随机访问
- 内存不连续也可接受

### 选择栈的情况
- 需要LIFO语义
- 实现递归算法的迭代版本
- 需要临时存储和恢复状态

### 选择队列的情况
- 需要FIFO语义
- 实现BFS算法
- 任务调度和缓冲

## 实现技巧

### 1. 内存管理
```cpp
// 动态数组扩容策略
if (size >= capacity) {
    capacity = capacity == 0 ? 1 : capacity * 2;
    reallocate();
}

// 链表内存释放
while (head) {
    Node* temp = head;
    head = head->next;
    delete temp;
}
```

### 2. 异常安全
```cpp
// RAII原则
class Vector {
    ~Vector() {
        delete[] data;
    }
};

// 强异常安全保证
void push_back(const T& value) {
    if (size >= capacity) {
        Vector temp(*this);
        temp.reserve(capacity * 2);
        temp.data[temp.size++] = value;
        swap(temp);
    } else {
        data[size++] = value;
    }
}
```

### 3. 迭代器设计
```cpp
// 统一的迭代器接口
template<typename T>
class Iterator {
public:
    T& operator*();
    Iterator& operator++();
    bool operator!=(const Iterator& other);
};
```

## 性能优化

### 1. 缓存优化
- 使用连续内存布局
- 预取数据
- 减少内存跳转

### 2. 分支预测优化
```cpp
// 使用likely/unlikely提示
if (likely(size < capacity)) {
    data[size++] = value;
} else {
    grow_and_insert(value);
}
```

### 3. 内存池
```cpp
// 链表节点内存池
class NodePool {
    std::vector<Node> pool;
    std::stack<Node*> free_nodes;
public:
    Node* allocate();
    void deallocate(Node* node);
};
```

## 常见问题和解决方案

### 1. 内存泄漏
**问题**：忘记释放动态分配的内存
**解决**：使用RAII、智能指针、容器类

### 2. 迭代器失效
**问题**：容器修改后迭代器失效
**解决**：了解各种操作对迭代器的影响，及时更新迭代器

### 3. 栈溢出
**问题**：递归深度过大导致栈溢出
**解决**：使用显式栈模拟递归，或增加栈大小

### 4. 队列假溢出
**问题**：循环队列中的假溢出现象
**解决**：正确实现循环队列的判满条件

## 调试技巧

### 1. 边界检查
```cpp
// 添加断言检查
assert(index >= 0 && index < size);

// 使用调试版本的边界检查
#ifdef DEBUG
T& at(size_t index) {
    if (index >= size) throw std::out_of_range("Index out of range");
    return data[index];
}
#endif
```

### 2. 内存检查
```cpp
// 使用工具检查内存泄漏
// Valgrind, AddressSanitizer, Dr. Memory

// 添加内存使用统计
class MemoryTracker {
    static size_t allocated;
public:
    static void* allocate(size_t size);
    static void deallocate(void* ptr);
    static size_t get_allocated() { return allocated; }
};
```

### 3. 单元测试
```cpp
// 测试边界条件
void test_empty_container();
void test_single_element();
void test_full_capacity();
void test_resize_operations();
```

## 学习路径

### 初学者路径
1. **动态数组** - 理解连续存储和动态扩容
2. **栈** - 掌握LIFO原理和应用
3. **队列** - 理解FIFO原理和循环队列
4. **链表** - 学习指针操作和动态内存管理

### 进阶路径
1. 实现泛型版本的数据结构
2. 添加异常安全保证
3. 设计统一的迭代器接口
4. 性能优化和内存管理
5. 线程安全版本的实现

## 实际应用

### 1. 系统编程
- 操作系统中的进程队列
- 内存管理中的空闲块链表
- 函数调用栈的实现

### 2. 编译器设计
- 词法分析中的符号表
- 语法分析中的操作符栈
- 代码生成中的临时变量管理

### 3. 网络编程
- 数据包缓冲队列
- 连接池管理
- 事件队列处理

### 4. 游戏开发
- 游戏对象管理
- 动画帧队列
- 输入事件处理

## 扩展阅读

### 经典教材
- 《数据结构与算法分析》- Mark Allen Weiss
- 《算法导论》- CLRS
- 《STL源码剖析》- 侯捷

### 在线资源
- [cppreference.com](https://cppreference.com) - C++标准库参考
- [Visualgo](https://visualgo.net) - 数据结构可视化
- [GeeksforGeeks](https://geeksforgeeks.org) - 算法和数据结构教程

## 练习题目

### 基础题目
1. 实现一个支持动态扩容的数组
2. 用数组实现栈和队列
3. 实现单链表的基本操作
4. 用栈实现括号匹配检查
5. 用队列实现BFS遍历

### 进阶题目
1. 实现LRU缓存（链表+哈希表）
2. 设计一个最小栈（支持O(1)时间获取最小值）
3. 实现双端队列
4. 用两个栈实现队列
5. 实现循环缓冲区

### 挑战题目
1. 实现线程安全的无锁队列
2. 设计内存池管理器
3. 实现支持快照的数据结构
4. 设计分布式队列系统
5. 实现高性能的字符串处理库

---

**下一步**：选择一个具体的数据结构开始深入学习
- [动态数组详解](./vector/README.md)
- [链表详解](./linked_list/README.md)
- [栈详解](./stack/README.md)
- [队列详解](./queue/README.md)