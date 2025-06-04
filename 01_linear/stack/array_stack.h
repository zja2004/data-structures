#ifndef ARRAY_STACK_H
#define ARRAY_STACK_H

#include <iostream>
#include <stdexcept>
#include <vector>

template<typename T>
class ArrayStack {
private:
    std::vector<T> data;    // 使用vector作为底层存储
    
public:
    // 构造函数
    ArrayStack() = default;
    
    // 拷贝构造函数
    ArrayStack(const ArrayStack& other) : data(other.data) {}
    
    // 赋值操作符
    ArrayStack& operator=(const ArrayStack& other) {
        if (this != &other) {
            data = other.data;
        }
        return *this;
    }
    
    // 析构函数
    ~ArrayStack() = default;
    
    // 基本操作
    void push(const T& value) {
        data.push_back(value);
    }
    
    void pop() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        data.pop_back();
    }
    
    T& top() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return data.back();
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return data.back();
    }
    
    // 状态查询
    bool empty() const {
        return data.empty();
    }
    
    size_t size() const {
        return data.size();
    }
    
    size_t capacity() const {
        return data.capacity();
    }
    
    // 实用函数
    void clear() {
        data.clear();
    }
    
    void reserve(size_t new_capacity) {
        data.reserve(new_capacity);
    }
    
    void shrink_to_fit() {
        data.shrink_to_fit();
    }
    
    // 输出函数
    void print() const {
        std::cout << "ArrayStack[" << size() << "/" << capacity() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "bottom [";
            for (size_t i = 0; i < data.size(); ++i) {
                std::cout << data[i];
                if (i < data.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "] top";
        }
        std::cout << std::endl;
    }
    
    // 比较操作
    bool operator==(const ArrayStack& other) const {
        return data == other.data;
    }
    
    bool operator!=(const ArrayStack& other) const {
        return !(*this == other);
    }
    
    // 交换操作
    void swap(ArrayStack& other) {
        data.swap(other.data);
    }
};

// 特化的栈操作
template<typename T>
class ArrayStackWithMin : public ArrayStack<T> {
private:
    std::vector<T> min_stack;  // 辅助栈存储最小值
    
public:
    void push(const T& value) {
        ArrayStack<T>::push(value);
        
        // 更新最小值栈
        if (min_stack.empty() || value <= min_stack.back()) {
            min_stack.push_back(value);
        }
    }
    
    void pop() {
        if (this->empty()) {
            throw std::runtime_error("Stack is empty");
        }
        
        // 如果弹出的是最小值，也要从最小值栈中弹出
        if (this->top() == min_stack.back()) {
            min_stack.pop_back();
        }
        
        ArrayStack<T>::pop();
    }
    
    const T& get_min() const {
        if (min_stack.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return min_stack.back();
    }
    
    void clear() {
        ArrayStack<T>::clear();
        min_stack.clear();
    }
};

// 双栈实现（在一个数组中实现两个栈）
template<typename T>
class DualArrayStack {
private:
    std::vector<T> data;
    int top1;  // 第一个栈的栈顶
    int top2;  // 第二个栈的栈顶
    
public:
    explicit DualArrayStack(size_t capacity) 
        : data(capacity), top1(-1), top2(capacity) {}
    
    // 第一个栈的操作
    void push1(const T& value) {
        if (top1 + 1 == top2) {
            throw std::runtime_error("Stack overflow");
        }
        data[++top1] = value;
    }
    
    void pop1() {
        if (top1 == -1) {
            throw std::runtime_error("Stack 1 is empty");
        }
        --top1;
    }
    
    T& top_1() {
        if (top1 == -1) {
            throw std::runtime_error("Stack 1 is empty");
        }
        return data[top1];
    }
    
    bool empty1() const {
        return top1 == -1;
    }
    
    size_t size1() const {
        return top1 + 1;
    }
    
    // 第二个栈的操作
    void push2(const T& value) {
        if (top1 + 1 == top2) {
            throw std::runtime_error("Stack overflow");
        }
        data[--top2] = value;
    }
    
    void pop2() {
        if (top2 == static_cast<int>(data.size())) {
            throw std::runtime_error("Stack 2 is empty");
        }
        ++top2;
    }
    
    T& top_2() {
        if (top2 == static_cast<int>(data.size())) {
            throw std::runtime_error("Stack 2 is empty");
        }
        return data[top2];
    }
    
    bool empty2() const {
        return top2 == static_cast<int>(data.size());
    }
    
    size_t size2() const {
        return data.size() - top2;
    }
    
    // 通用操作
    bool full() const {
        return top1 + 1 == top2;
    }
    
    size_t capacity() const {
        return data.size();
    }
    
    void print() const {
        std::cout << "DualStack[" << size1() << ", " << size2() << "/" << capacity() << "]:" << std::endl;
        
        std::cout << "Stack1: ";
        if (empty1()) {
            std::cout << "(empty)";
        } else {
            for (int i = 0; i <= top1; ++i) {
                std::cout << data[i] << " ";
            }
        }
        std::cout << std::endl;
        
        std::cout << "Stack2: ";
        if (empty2()) {
            std::cout << "(empty)";
        } else {
            for (int i = top2; i < static_cast<int>(data.size()); ++i) {
                std::cout << data[i] << " ";
            }
        }
        std::cout << std::endl;
    }
};

#endif // ARRAY_STACK_H