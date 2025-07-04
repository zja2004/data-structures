# 栈 (Stack)

## 概述

栈是一种遵循后进先出（LIFO - Last In First Out）原则的线性数据结构。元素只能从栈的一端（称为栈顶）进行插入和删除操作。

## 核心特性

- **LIFO原则**：最后插入的元素最先被删除
- **受限访问**：只能访问栈顶元素
- **动态大小**：可以根据需要动态增长和收缩
- **简单操作**：主要操作包括push、pop、top

## 基本操作

| 操作 | 描述 | 时间复杂度 |
|------|------|------------|
| push(x) | 将元素x压入栈顶 | O(1) |
| pop() | 删除并返回栈顶元素 | O(1) |
| top() | 返回栈顶元素（不删除） | O(1) |
| empty() | 检查栈是否为空 | O(1) |
| size() | 返回栈中元素个数 | O(1) |

## 实现方式

### 1. 基于数组的实现
- 使用动态数组作为底层存储
- 维护一个top指针指向栈顶
- 优点：内存连续，缓存友好
- 缺点：可能需要扩容操作

### 2. 基于链表的实现
- 使用单向链表，头部作为栈顶
- 优点：动态分配，不需要预先确定大小
- 缺点：额外的指针开销，内存不连续

## 栈的应用场景

### 1. 函数调用管理
- 程序运行时的函数调用栈
- 局部变量和返回地址的管理

### 2. 表达式求值
- 中缀表达式转后缀表达式
- 后缀表达式的计算
- 括号匹配检查

### 3. 撤销操作
- 文本编辑器的撤销功能
- 浏览器的后退功能

### 4. 深度优先搜索（DFS）
- 图和树的遍历
- 迷宫求解

### 5. 语法分析
- 编译器的语法分析
- 括号、标签的匹配

## 经典算法问题

### 1. 括号匹配
```
输入: "({[]})"
输出: true

输入: "({[})"
输出: false
```

### 2. 逆波兰表达式求值
```
输入: ["2", "1", "+", "3", "*"]
输出: 9
解释: ((2 + 1) * 3) = 9
```

### 3. 最大矩形面积
- 使用栈来解决柱状图中的最大矩形问题

### 4. 下一个更大元素
- 使用单调栈来找到每个元素右边第一个更大的元素

## 栈的变种

### 1. 最小栈
- 支持在常数时间内获取栈中最小元素
- 需要额外的辅助栈来存储最小值

### 2. 双栈
- 使用一个数组实现两个栈
- 一个从左端增长，一个从右端增长

### 3. 栈的链式存储
- 使用链表实现，避免固定大小限制

## 实现细节

### 数组实现的关键点
```cpp
class Stack {
private:
    vector<T> data;
    
public:
    void push(const T& value) {
        data.push_back(value);
    }
    
    void pop() {
        if (!empty()) {
            data.pop_back();
        }
    }
    
    T& top() {
        return data.back();
    }
};
```

### 链表实现的关键点
```cpp
class Stack {
private:
    struct Node {
        T data;
        Node* next;
    };
    Node* top_node;
    
public:
    void push(const T& value) {
        Node* new_node = new Node{value, top_node};
        top_node = new_node;
    }
};
```

## 性能分析

### 时间复杂度
- 所有基本操作都是O(1)
- 扩容操作（数组实现）：摊销O(1)

### 空间复杂度
- 数组实现：O(n)，可能有额外的未使用空间
- 链表实现：O(n)，每个节点有指针开销

## 代码实现

- [基于数组的栈实现](./array_stack.h)
- [基于链表的栈实现](./linked_stack.h)
- [最小栈实现](./min_stack.h)

## 使用示例

查看详细示例：[example.cpp](./example.cpp)

## 练习题

1. 实现一个支持getMin()操作的栈
2. 用两个栈实现一个队列
3. 判断括号序列是否有效
4. 计算逆波兰表达式的值
5. 实现浏览器的前进后退功能
6. 设计一个栈，支持push、pop、top和在常数时间内检索最小元素

## 调试技巧

1. **栈溢出检查**：确保在pop和top操作前检查栈是否为空
2. **内存管理**：链表实现要注意内存泄漏
3. **边界条件**：测试空栈和满栈的情况
4. **异常处理**：对非法操作抛出适当的异常

---

**上一章：** [链表 (Linked List)](../linked_list/README.md)  
**下一章：** [队列 (Queue)](../queue/README.md)