# 队列 (Queue)

## 概述

队列是一种遵循先进先出（FIFO - First In First Out）原则的线性数据结构。元素从队列的一端（队尾）插入，从另一端（队首）删除。

## 核心特性

- **FIFO原则**：最先插入的元素最先被删除
- **双端操作**：一端插入（入队），另一端删除（出队）
- **受限访问**：只能访问队首和队尾元素
- **动态大小**：可以根据需要动态增长和收缩

## 基本操作

| 操作 | 描述 | 时间复杂度 |
|------|------|------------|
| enqueue(x) | 将元素x加入队尾 | O(1) |
| dequeue() | 删除并返回队首元素 | O(1) |
| front() | 返回队首元素（不删除） | O(1) |
| back() | 返回队尾元素（不删除） | O(1) |
| empty() | 检查队列是否为空 | O(1) |
| size() | 返回队列中元素个数 | O(1) |

## 实现方式

### 1. 基于数组的实现

#### 简单数组实现
- 使用两个指针：front和rear
- 问题：会产生"假溢出"现象

#### 循环数组实现
- 将数组看作环形结构
- 解决假溢出问题
- 需要区分队列满和队列空的状态

```cpp
class CircularQueue {
private:
    vector<T> data;
    int front_idx, rear_idx;
    int count;  // 元素个数
};
```

### 2. 基于链表的实现
- 使用单向链表，维护头尾指针
- 头部出队，尾部入队
- 动态分配内存，无大小限制

```cpp
class LinkedQueue {
private:
    struct Node {
        T data;
        Node* next;
    };
    Node* front_node;
    Node* rear_node;
};
```

### 3. 基于双端队列的实现
- 使用deque作为底层容器
- 提供更多灵活性

## 队列的变种

### 1. 双端队列 (Deque)
- 两端都可以进行插入和删除操作
- 结合了栈和队列的特性

### 2. 优先队列 (Priority Queue)
- 元素按优先级排序
- 高优先级元素先出队
- 通常用堆实现

### 3. 循环队列 (Circular Queue)
- 队尾连接到队首，形成环形
- 有效利用存储空间

### 4. 阻塞队列 (Blocking Queue)
- 线程安全的队列
- 支持阻塞的入队和出队操作

## 应用场景

### 1. 任务调度
- 操作系统的进程调度
- 打印任务队列
- CPU任务调度

### 2. 缓冲区管理
- 键盘缓冲区
- 网络数据包缓冲
- 生产者-消费者模式

### 3. 广度优先搜索（BFS）
- 图和树的层序遍历
- 最短路径算法
- 迷宫求解

### 4. 系统设计
- 消息队列系统
- 请求处理队列
- 事件处理系统

### 5. 算法实现
- 拓扑排序
- 二叉树层序遍历
- 图的连通性检测

## 经典算法问题

### 1. 用栈实现队列
```cpp
// 使用两个栈实现队列
class MyQueue {
private:
    stack<int> input, output;
public:
    void push(int x) { input.push(x); }
    int pop() { peek(); int res = output.top(); output.pop(); return res; }
    int peek() { if(output.empty()) while(!input.empty()) { output.push(input.top()); input.pop(); } return output.top(); }
};
```

### 2. 滑动窗口最大值
- 使用双端队列维护窗口内的最大值

### 3. 二叉树的层序遍历
- 使用队列实现BFS遍历

### 4. 岛屿数量
- 使用BFS搜索连通的陆地

## 性能分析

### 时间复杂度
| 实现方式 | 入队 | 出队 | 访问队首/队尾 |
|----------|------|------|---------------|
| 数组实现 | O(1) | O(1) | O(1) |
| 链表实现 | O(1) | O(1) | O(1) |
| 动态数组 | O(1)* | O(n) | O(1) |

*摊销时间复杂度

### 空间复杂度
- **数组实现**：O(n)，可能有未使用的空间
- **链表实现**：O(n)，每个节点有指针开销
- **循环数组**：O(n)，空间利用率高

## 实现细节

### 循环队列的关键点
```cpp
class CircularQueue {
private:
    vector<T> data;
    int front_idx, rear_idx, count, capacity;
    
public:
    bool is_full() { return count == capacity; }
    bool is_empty() { return count == 0; }
    
    void enqueue(T value) {
        if (is_full()) throw overflow_error("Queue is full");
        data[rear_idx] = value;
        rear_idx = (rear_idx + 1) % capacity;
        count++;
    }
    
    T dequeue() {
        if (is_empty()) throw underflow_error("Queue is empty");
        T value = data[front_idx];
        front_idx = (front_idx + 1) % capacity;
        count--;
        return value;
    }
};
```

### 链表队列的关键点
```cpp
class LinkedQueue {
private:
    struct Node { T data; Node* next; };
    Node* front_node, *rear_node;
    
public:
    void enqueue(T value) {
        Node* new_node = new Node{value, nullptr};
        if (rear_node) rear_node->next = new_node;
        else front_node = new_node;
        rear_node = new_node;
    }
    
    T dequeue() {
        if (!front_node) throw underflow_error("Queue is empty");
        T value = front_node->data;
        Node* old_front = front_node;
        front_node = front_node->next;
        if (!front_node) rear_node = nullptr;
        delete old_front;
        return value;
    }
};
```

## 代码实现

- [基于数组的队列实现](./array_queue.h)
- [基于链表的队列实现](./linked_queue.h)
- [循环队列实现](./circular_queue.h)
- [优先队列实现](./priority_queue.h)

## 使用示例

查看详细示例：[example.cpp](./example.cpp)

## 练习题

1. 用两个栈实现一个队列
2. 设计循环队列
3. 实现滑动窗口最大值
4. 二叉树的层序遍历
5. 实现生产者-消费者模式
6. 设计一个队列，支持在队首和队尾进行插入和删除操作
7. 实现一个带有最大值功能的队列

## 调试技巧

1. **边界检查**：确保在操作前检查队列是否为空或满
2. **指针管理**：链表实现要注意空指针和内存泄漏
3. **循环逻辑**：循环队列要正确处理索引的环绕
4. **状态维护**：正确维护队列的大小和状态信息

## 队列 vs 栈

| 特性 | 队列 | 栈 |
|------|------|----|
| 操作原则 | FIFO | LIFO |
| 插入位置 | 队尾 | 栈顶 |
| 删除位置 | 队首 | 栈顶 |
| 应用场景 | BFS、任务调度 | DFS、函数调用 |
| 访问限制 | 队首和队尾 | 仅栈顶 |

---

**上一章：** [栈 (Stack)](../stack/README.md)  
**下一章：** [双端队列 (Deque)](../deque/README.md)