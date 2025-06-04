# 堆 (Heap)

## 概述

堆是一种特殊的完全二叉树数据结构，满足堆性质：在最大堆中，每个父节点的值都大于或等于其子节点的值；在最小堆中，每个父节点的值都小于或等于其子节点的值。堆通常用数组来实现，是优先队列的经典实现方式。

## 核心特性

### 基本性质

- **完全二叉树**：除了最后一层，其他层都被完全填满，最后一层从左到右填充
- **堆序性质**：父节点与子节点之间满足特定的大小关系
- **数组表示**：可以用数组高效地表示，节点i的左子节点为2i+1，右子节点为2i+2，父节点为(i-1)/2

### 堆的类型

1. **最大堆 (Max Heap)**：父节点值 ≥ 子节点值
2. **最小堆 (Min Heap)**：父节点值 ≤ 子节点值
3. **二项堆 (Binomial Heap)**：支持快速合并的堆
4. **斐波那契堆 (Fibonacci Heap)**：支持更快的减少键值操作

## 基本操作

### 核心操作

- `insert(value)` - 插入元素
- `extract_max/min()` - 删除并返回最大/最小元素
- `peek()` / `top()` - 查看最大/最小元素（不删除）
- `increase_key(i, new_value)` - 增加指定位置的键值
- `decrease_key(i, new_value)` - 减少指定位置的键值
- `delete(i)` - 删除指定位置的元素

### 辅助操作

- `heapify(array)` - 将数组转换为堆
- `build_heap(array)` - 从数组构建堆
- `heap_sort(array)` - 堆排序
- `merge(heap1, heap2)` - 合并两个堆

## 实现方式

### 1. 数组实现

```cpp
// 父节点索引
int parent(int i) { return (i - 1) / 2; }

// 左子节点索引
int left_child(int i) { return 2 * i + 1; }

// 右子节点索引
int right_child(int i) { return 2 * i + 2; }
```

### 2. 链表实现

- 使用指针连接节点
- 需要额外的父指针来支持某些操作
- 空间开销较大，但支持动态大小

## 应用场景

### 1. 优先队列

- 任务调度系统
- 事件驱动模拟
- Dijkstra算法
- A*搜索算法

### 2. 排序算法

- 堆排序 (Heap Sort)
- 部分排序 (找前K大/小元素)

### 3. 图算法

- 最小生成树 (Prim算法)
- 最短路径 (Dijkstra算法)

### 4. 数据流处理

- 中位数维护
- 滑动窗口最值
- Top-K问题

## 经典算法问题

### 基础问题

1. **堆排序**：使用堆进行排序
2. **第K大元素**：在数组中找第K大的元素
3. **合并K个有序链表**：使用最小堆
4. **数据流中的中位数**：使用两个堆

### 进阶问题

1. **滑动窗口最大值**：使用双端队列或堆
2. **丑数**：使用最小堆生成丑数序列
3. **会议室调度**：使用堆管理会议时间
4. **CPU任务调度**：基于优先级的任务调度

## 堆的变种

### 1. d-ary堆

- 每个节点有d个子节点
- 减少树的高度，但增加比较次数

### 2. 二项堆

- 支持O(log n)的合并操作
- 由多个二项树组成

### 3. 斐波那契堆

- 支持O(1)的decrease_key操作
- 复杂的实现，但理论性能优秀

### 4. 配对堆

- 简化的斐波那契堆
- 实际性能通常很好

## 时间复杂度

| 操作 | 二叉堆 | 二项堆 | 斐波那契堆 |
|------|--------|--------|------------|
| 插入 | O(log n) | O(log n) | O(1) |
| 查找最值 | O(1) | O(log n) | O(1) |
| 删除最值 | O(log n) | O(log n) | O(log n) |
| 减少键值 | O(log n) | O(log n) | O(1) |
| 合并 | O(n) | O(log n) | O(1) |
| 构建堆 | O(n) | O(n) | O(n) |

## 空间复杂度

- **数组实现**：O(n)
- **链表实现**：O(n)，但有额外的指针开销
- **辅助空间**：O(1)（原地操作）或O(log n)（递归调用栈）

## 实现文件结构

```
heap/
├── README.md              # 本文档
├── binary_heap.h          # 二叉堆实现
├── binomial_heap.h        # 二项堆实现
├── fibonacci_heap.h       # 斐波那契堆实现
└── example.cpp           # 使用示例和测试
```

## 使用示例

### 基本用法

```cpp
#include "binary_heap.h"

// 创建最大堆
MaxHeap<int> max_heap;
max_heap.insert(10);
max_heap.insert(20);
max_heap.insert(5);

cout << max_heap.top() << endl;  // 输出: 20
max_heap.extract_max();          // 删除最大元素
cout << max_heap.top() << endl;  // 输出: 10

// 创建最小堆
MinHeap<int> min_heap;
min_heap.insert(10);
min_heap.insert(20);
min_heap.insert(5);

cout << min_heap.top() << endl;  // 输出: 5
```

### 堆排序

```cpp
vector<int> arr = {64, 34, 25, 12, 22, 11, 90};
heap_sort(arr);
// arr现在是有序的
```

### 优先队列应用

```cpp
// 任务调度
struct Task {
    int priority;
    string name;
    bool operator<(const Task& other) const {
        return priority < other.priority;  // 高优先级先执行
    }
};

MaxHeap<Task> task_queue;
task_queue.insert({10, "High Priority Task"});
task_queue.insert({5, "Low Priority Task"});
task_queue.insert({8, "Medium Priority Task"});

while (!task_queue.empty()) {
    Task task = task_queue.extract_max();
    cout << "Executing: " << task.name << endl;
}
```

## 编译和运行

```bash
# 编译示例程序
g++ -std=c++17 -O2 example.cpp -o heap_example

# 运行示例
./heap_example

# 编译时启用调试信息
g++ -std=c++17 -g -DDEBUG example.cpp -o heap_debug
```

## 练习题

### 基础练习

1. 实现一个支持动态调整大小的堆
2. 实现堆排序算法
3. 找到数组中第K大的元素
4. 合并K个有序数组

### 进阶练习

1. 实现数据流中位数的维护
2. 设计一个支持插入、删除、查找中位数的数据结构
3. 实现滑动窗口最大值算法
4. 设计一个任务调度器

### 挑战练习

1. 实现二项堆
2. 实现斐波那契堆
3. 设计一个支持范围查询的堆
4. 实现可持久化堆

## 调试技巧

### 1. 堆性质验证

```cpp
bool is_valid_heap(const vector<int>& heap, bool is_max_heap = true) {
    for (int i = 0; i < heap.size(); ++i) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        
        if (left < heap.size()) {
            if (is_max_heap && heap[i] < heap[left]) return false;
            if (!is_max_heap && heap[i] > heap[left]) return false;
        }
        
        if (right < heap.size()) {
            if (is_max_heap && heap[i] < heap[right]) return false;
            if (!is_max_heap && heap[i] > heap[right]) return false;
        }
    }
    return true;
}
```

### 2. 可视化堆结构

```cpp
void print_heap(const vector<int>& heap) {
    int level = 0;
    int level_size = 1;
    int index = 0;
    
    while (index < heap.size()) {
        for (int i = 0; i < level_size && index < heap.size(); ++i) {
            cout << heap[index++] << " ";
        }
        cout << endl;
        level_size *= 2;
    }
}
```

### 3. 性能分析

- 使用计时器测量操作时间
- 分析不同数据规模下的性能
- 比较不同堆实现的效率

## 学习建议

### 学习路径

1. **理解概念**：掌握堆的基本性质和数组表示
2. **基本操作**：实现插入、删除、堆化操作
3. **应用实践**：解决经典的堆相关问题
4. **高级主题**：学习不同类型的堆和优化技巧

### 实践建议

1. 从简单的二叉堆开始实现
2. 通过可视化理解堆的结构变化
3. 解决LeetCode上的堆相关题目
4. 分析实际应用中的堆使用场景

### 扩展阅读

- 《算法导论》第6章：堆排序
- 《数据结构与算法分析》：优先队列
- 相关论文：斐波那契堆的理论分析

## 参考资料

1. Cormen, T. H. et al. "Introduction to Algorithms" - 堆和优先队列
2. Sedgewick, R. "Algorithms" - 堆排序和优先队列
3. Knuth, D. E. "The Art of Computer Programming, Volume 3" - 排序和搜索
4. 在线资源：VisuAlgo堆可视化工具