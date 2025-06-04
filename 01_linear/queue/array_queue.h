#ifndef ARRAY_QUEUE_H
#define ARRAY_QUEUE_H

#include <iostream>
#include <stdexcept>
#include <vector>

// 基于动态数组的简单队列实现
template<typename T>
class ArrayQueue {
private:
    std::vector<T> data;
    size_t front_idx;  // 队首索引
    size_t rear_idx;   // 队尾索引（指向最后一个元素的下一个位置）
    size_t count;      // 元素个数
    
public:
    // 构造函数
    ArrayQueue() : front_idx(0), rear_idx(0), count(0) {
        data.reserve(10);  // 预分配一些空间
    }
    
    explicit ArrayQueue(size_t initial_capacity) 
        : front_idx(0), rear_idx(0), count(0) {
        data.reserve(initial_capacity);
    }
    
    // 拷贝构造函数
    ArrayQueue(const ArrayQueue& other) 
        : data(other.data), front_idx(other.front_idx), 
          rear_idx(other.rear_idx), count(other.count) {}
    
    // 赋值操作符
    ArrayQueue& operator=(const ArrayQueue& other) {
        if (this != &other) {
            data = other.data;
            front_idx = other.front_idx;
            rear_idx = other.rear_idx;
            count = other.count;
        }
        return *this;
    }
    
    // 基本操作
    void enqueue(const T& value) {
        // 如果需要扩容
        if (rear_idx >= data.size()) {
            data.resize(data.size() == 0 ? 1 : data.size() * 2);
        }
        
        data[rear_idx++] = value;
        ++count;
    }
    
    void dequeue() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        ++front_idx;
        --count;
        
        // 如果队列变空，重置索引
        if (empty()) {
            front_idx = rear_idx = 0;
        }
        // 如果前面浪费的空间太多，进行压缩
        else if (front_idx > data.size() / 2) {
            compact();
        }
    }
    
    T& front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[front_idx];
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[front_idx];
    }
    
    T& back() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[rear_idx - 1];
    }
    
    const T& back() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[rear_idx - 1];
    }
    
    // 状态查询
    bool empty() const {
        return count == 0;
    }
    
    size_t size() const {
        return count;
    }
    
    size_t capacity() const {
        return data.size();
    }
    
    // 实用函数
    void clear() {
        front_idx = rear_idx = count = 0;
    }
    
    void shrink_to_fit() {
        if (empty()) {
            data.clear();
            data.shrink_to_fit();
        } else {
            compact();
            data.resize(count);
            data.shrink_to_fit();
            rear_idx = count;
        }
    }
    
    // 输出函数
    void print() const {
        std::cout << "ArrayQueue[" << size() << "/" << capacity() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "front [";
            for (size_t i = front_idx; i < rear_idx; ++i) {
                std::cout << data[i];
                if (i < rear_idx - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "] rear";
        }
        std::cout << std::endl;
    }
    
private:
    // 压缩数组，移除前面的空闲空间
    void compact() {
        if (front_idx > 0 && !empty()) {
            for (size_t i = 0; i < count; ++i) {
                data[i] = data[front_idx + i];
            }
            rear_idx = count;
            front_idx = 0;
        }
    }
};

// 循环队列实现
template<typename T>
class CircularArrayQueue {
private:
    std::vector<T> data;
    size_t front_idx;  // 队首索引
    size_t rear_idx;   // 队尾索引
    size_t count;      // 元素个数
    size_t capacity_;  // 容量
    
public:
    // 构造函数
    explicit CircularArrayQueue(size_t capacity) 
        : data(capacity), front_idx(0), rear_idx(0), count(0), capacity_(capacity) {}
    
    // 基本操作
    void enqueue(const T& value) {
        if (is_full()) {
            throw std::runtime_error("Queue is full");
        }
        
        data[rear_idx] = value;
        rear_idx = (rear_idx + 1) % capacity_;
        ++count;
    }
    
    void dequeue() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        front_idx = (front_idx + 1) % capacity_;
        --count;
    }
    
    T& front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[front_idx];
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[front_idx];
    }
    
    T& back() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        size_t back_idx = (rear_idx - 1 + capacity_) % capacity_;
        return data[back_idx];
    }
    
    const T& back() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        size_t back_idx = (rear_idx - 1 + capacity_) % capacity_;
        return data[back_idx];
    }
    
    // 状态查询
    bool empty() const {
        return count == 0;
    }
    
    bool is_full() const {
        return count == capacity_;
    }
    
    size_t size() const {
        return count;
    }
    
    size_t capacity() const {
        return capacity_;
    }
    
    // 实用函数
    void clear() {
        front_idx = rear_idx = count = 0;
    }
    
    // 输出函数
    void print() const {
        std::cout << "CircularQueue[" << size() << "/" << capacity() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "front [";
            for (size_t i = 0; i < count; ++i) {
                size_t idx = (front_idx + i) % capacity_;
                std::cout << data[idx];
                if (i < count - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "] rear";
        }
        std::cout << std::endl;
    }
    
    // 调试函数：显示内部数组状态
    void print_internal() const {
        std::cout << "Internal array: [";
        for (size_t i = 0; i < capacity_; ++i) {
            if (i == front_idx && i == rear_idx && !empty()) {
                std::cout << "F/R:";
            } else if (i == front_idx) {
                std::cout << "F:";
            } else if (i == rear_idx) {
                std::cout << "R:";
            }
            
            if (count > 0) {
                bool in_queue = false;
                if (front_idx <= rear_idx) {
                    in_queue = (i >= front_idx && i < rear_idx);
                } else {
                    in_queue = (i >= front_idx || i < rear_idx);
                }
                
                if (in_queue) {
                    std::cout << data[i];
                } else {
                    std::cout << "_";
                }
            } else {
                std::cout << "_";
            }
            
            if (i < capacity_ - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
        std::cout << "front_idx: " << front_idx << ", rear_idx: " << rear_idx 
                  << ", count: " << count << std::endl;
    }
};

// 可扩容的循环队列
template<typename T>
class DynamicCircularQueue {
private:
    std::vector<T> data;
    size_t front_idx;
    size_t rear_idx;
    size_t count;
    
public:
    // 构造函数
    DynamicCircularQueue() : front_idx(0), rear_idx(0), count(0) {
        data.resize(4);  // 初始容量
    }
    
    explicit DynamicCircularQueue(size_t initial_capacity) 
        : front_idx(0), rear_idx(0), count(0) {
        data.resize(initial_capacity > 0 ? initial_capacity : 1);
    }
    
    // 基本操作
    void enqueue(const T& value) {
        if (count == data.size()) {
            resize();
        }
        
        data[rear_idx] = value;
        rear_idx = (rear_idx + 1) % data.size();
        ++count;
    }
    
    void dequeue() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        front_idx = (front_idx + 1) % data.size();
        --count;
        
        // 如果使用率太低，缩小容量
        if (count > 0 && count <= data.size() / 4 && data.size() > 4) {
            resize(data.size() / 2);
        }
    }
    
    T& front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[front_idx];
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return data[front_idx];
    }
    
    T& back() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        size_t back_idx = (rear_idx - 1 + data.size()) % data.size();
        return data[back_idx];
    }
    
    const T& back() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        size_t back_idx = (rear_idx - 1 + data.size()) % data.size();
        return data[back_idx];
    }
    
    // 状态查询
    bool empty() const {
        return count == 0;
    }
    
    size_t size() const {
        return count;
    }
    
    size_t capacity() const {
        return data.size();
    }
    
    void clear() {
        front_idx = rear_idx = count = 0;
    }
    
    void print() const {
        std::cout << "DynamicCircularQueue[" << size() << "/" << capacity() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "front [";
            for (size_t i = 0; i < count; ++i) {
                size_t idx = (front_idx + i) % data.size();
                std::cout << data[idx];
                if (i < count - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "] rear";
        }
        std::cout << std::endl;
    }
    
private:
    void resize(size_t new_capacity = 0) {
        if (new_capacity == 0) {
            new_capacity = data.size() * 2;
        }
        
        std::vector<T> new_data(new_capacity);
        
        // 复制元素到新数组
        for (size_t i = 0; i < count; ++i) {
            size_t old_idx = (front_idx + i) % data.size();
            new_data[i] = data[old_idx];
        }
        
        data = std::move(new_data);
        front_idx = 0;
        rear_idx = count;
    }
};

#endif // ARRAY_QUEUE_H