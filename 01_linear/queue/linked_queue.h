#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

#include <iostream>
#include <stdexcept>

// 基于链表的队列实现
template<typename T>
class LinkedQueue {
private:
    struct Node {
        T data;
        Node* next;
        
        Node(const T& value) : data(value), next(nullptr) {}
    };
    
    Node* front_node;  // 队首节点
    Node* rear_node;   // 队尾节点
    size_t count;      // 元素个数
    
public:
    // 构造函数
    LinkedQueue() : front_node(nullptr), rear_node(nullptr), count(0) {}
    
    // 拷贝构造函数
    LinkedQueue(const LinkedQueue& other) : front_node(nullptr), rear_node(nullptr), count(0) {
        copy_from(other);
    }
    
    // 赋值操作符
    LinkedQueue& operator=(const LinkedQueue& other) {
        if (this != &other) {
            clear();
            copy_from(other);
        }
        return *this;
    }
    
    // 析构函数
    ~LinkedQueue() {
        clear();
    }
    
    // 基本操作
    void enqueue(const T& value) {
        Node* new_node = new Node(value);
        
        if (empty()) {
            front_node = rear_node = new_node;
        } else {
            rear_node->next = new_node;
            rear_node = new_node;
        }
        
        ++count;
    }
    
    void dequeue() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        Node* old_front = front_node;
        front_node = front_node->next;
        
        if (front_node == nullptr) {
            rear_node = nullptr;  // 队列变空
        }
        
        delete old_front;
        --count;
    }
    
    T& front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return front_node->data;
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return front_node->data;
    }
    
    T& back() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return rear_node->data;
    }
    
    const T& back() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        return rear_node->data;
    }
    
    // 状态查询
    bool empty() const {
        return front_node == nullptr;
    }
    
    size_t size() const {
        return count;
    }
    
    // 实用函数
    void clear() {
        while (!empty()) {
            dequeue();
        }
    }
    
    // 输出函数
    void print() const {
        std::cout << "LinkedQueue[" << size() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "front [";
            Node* current = front_node;
            while (current != nullptr) {
                std::cout << current->data;
                if (current->next != nullptr) {
                    std::cout << ", ";
                }
                current = current->next;
            }
            std::cout << "] rear";
        }
        std::cout << std::endl;
    }
    
    // 反向输出（仅用于调试）
    void print_reverse() const {
        std::cout << "LinkedQueue reverse: ";
        print_reverse_helper(front_node);
        std::cout << std::endl;
    }
    
private:
    void copy_from(const LinkedQueue& other) {
        Node* current = other.front_node;
        while (current != nullptr) {
            enqueue(current->data);
            current = current->next;
        }
    }
    
    void print_reverse_helper(Node* node) const {
        if (node == nullptr) {
            return;
        }
        print_reverse_helper(node->next);
        std::cout << node->data << " ";
    }
};

// 双端队列（Deque）基于双向链表实现
template<typename T>
class LinkedDeque {
private:
    struct Node {
        T data;
        Node* next;
        Node* prev;
        
        Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
    };
    
    Node* front_node;  // 队首节点
    Node* rear_node;   // 队尾节点
    size_t count;      // 元素个数
    
public:
    // 构造函数
    LinkedDeque() : front_node(nullptr), rear_node(nullptr), count(0) {}
    
    // 拷贝构造函数
    LinkedDeque(const LinkedDeque& other) : front_node(nullptr), rear_node(nullptr), count(0) {
        copy_from(other);
    }
    
    // 赋值操作符
    LinkedDeque& operator=(const LinkedDeque& other) {
        if (this != &other) {
            clear();
            copy_from(other);
        }
        return *this;
    }
    
    // 析构函数
    ~LinkedDeque() {
        clear();
    }
    
    // 前端操作
    void push_front(const T& value) {
        Node* new_node = new Node(value);
        
        if (empty()) {
            front_node = rear_node = new_node;
        } else {
            new_node->next = front_node;
            front_node->prev = new_node;
            front_node = new_node;
        }
        
        ++count;
    }
    
    void pop_front() {
        if (empty()) {
            throw std::runtime_error("Deque is empty");
        }
        
        Node* old_front = front_node;
        front_node = front_node->next;
        
        if (front_node == nullptr) {
            rear_node = nullptr;  // 双端队列变空
        } else {
            front_node->prev = nullptr;
        }
        
        delete old_front;
        --count;
    }
    
    // 后端操作
    void push_back(const T& value) {
        Node* new_node = new Node(value);
        
        if (empty()) {
            front_node = rear_node = new_node;
        } else {
            rear_node->next = new_node;
            new_node->prev = rear_node;
            rear_node = new_node;
        }
        
        ++count;
    }
    
    void pop_back() {
        if (empty()) {
            throw std::runtime_error("Deque is empty");
        }
        
        Node* old_rear = rear_node;
        rear_node = rear_node->prev;
        
        if (rear_node == nullptr) {
            front_node = nullptr;  // 双端队列变空
        } else {
            rear_node->next = nullptr;
        }
        
        delete old_rear;
        --count;
    }
    
    // 队列接口兼容
    void enqueue(const T& value) {
        push_back(value);
    }
    
    void dequeue() {
        pop_front();
    }
    
    // 栈接口兼容
    void push(const T& value) {
        push_back(value);
    }
    
    void pop() {
        pop_back();
    }
    
    T& top() {
        return back();
    }
    
    const T& top() const {
        return back();
    }
    
    // 元素访问
    T& front() {
        if (empty()) {
            throw std::runtime_error("Deque is empty");
        }
        return front_node->data;
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Deque is empty");
        }
        return front_node->data;
    }
    
    T& back() {
        if (empty()) {
            throw std::runtime_error("Deque is empty");
        }
        return rear_node->data;
    }
    
    const T& back() const {
        if (empty()) {
            throw std::runtime_error("Deque is empty");
        }
        return rear_node->data;
    }
    
    // 状态查询
    bool empty() const {
        return front_node == nullptr;
    }
    
    size_t size() const {
        return count;
    }
    
    // 实用函数
    void clear() {
        while (!empty()) {
            pop_front();
        }
    }
    
    // 输出函数
    void print() const {
        std::cout << "LinkedDeque[" << size() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "front [";
            Node* current = front_node;
            while (current != nullptr) {
                std::cout << current->data;
                if (current->next != nullptr) {
                    std::cout << ", ";
                }
                current = current->next;
            }
            std::cout << "] rear";
        }
        std::cout << std::endl;
    }
    
    void print_reverse() const {
        std::cout << "LinkedDeque reverse[" << size() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "rear [";
            Node* current = rear_node;
            while (current != nullptr) {
                std::cout << current->data;
                if (current->prev != nullptr) {
                    std::cout << ", ";
                }
                current = current->prev;
            }
            std::cout << "] front";
        }
        std::cout << std::endl;
    }
    
private:
    void copy_from(const LinkedDeque& other) {
        Node* current = other.front_node;
        while (current != nullptr) {
            push_back(current->data);
            current = current->next;
        }
    }
};

// 使用两个栈实现队列
template<typename T>
class QueueUsingTwoStacks {
private:
    std::vector<T> input_stack;   // 输入栈
    std::vector<T> output_stack;  // 输出栈
    
public:
    // 基本操作
    void enqueue(const T& value) {
        input_stack.push_back(value);
    }
    
    void dequeue() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        if (output_stack.empty()) {
            transfer();
        }
        
        output_stack.pop_back();
    }
    
    T& front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        if (output_stack.empty()) {
            transfer();
        }
        
        return output_stack.back();
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        if (output_stack.empty()) {
            const_cast<QueueUsingTwoStacks*>(this)->transfer();
        }
        
        return output_stack.back();
    }
    
    // 状态查询
    bool empty() const {
        return input_stack.empty() && output_stack.empty();
    }
    
    size_t size() const {
        return input_stack.size() + output_stack.size();
    }
    
    void clear() {
        input_stack.clear();
        output_stack.clear();
    }
    
    // 输出函数
    void print() const {
        std::cout << "QueueUsingTwoStacks[" << size() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "front [";
            
            // 先输出output_stack（从栈顶到栈底）
            for (int i = output_stack.size() - 1; i >= 0; --i) {
                std::cout << output_stack[i];
                if (i > 0 || !input_stack.empty()) {
                    std::cout << ", ";
                }
            }
            
            // 再输出input_stack（从栈底到栈顶）
            for (size_t i = 0; i < input_stack.size(); ++i) {
                std::cout << input_stack[i];
                if (i < input_stack.size() - 1) {
                    std::cout << ", ";
                }
            }
            
            std::cout << "] rear";
        }
        std::cout << std::endl;
    }
    
    // 调试函数：显示两个栈的状态
    void print_stacks() const {
        std::cout << "Input stack (bottom to top): [";
        for (size_t i = 0; i < input_stack.size(); ++i) {
            std::cout << input_stack[i];
            if (i < input_stack.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
        
        std::cout << "Output stack (bottom to top): [";
        for (size_t i = 0; i < output_stack.size(); ++i) {
            std::cout << output_stack[i];
            if (i < output_stack.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;
    }
    
private:
    // 将输入栈的所有元素转移到输出栈
    void transfer() {
        while (!input_stack.empty()) {
            output_stack.push_back(input_stack.back());
            input_stack.pop_back();
        }
    }
};

#endif // LINKED_QUEUE_H