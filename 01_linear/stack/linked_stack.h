#ifndef LINKED_STACK_H
#define LINKED_STACK_H

#include <iostream>
#include <stdexcept>

template<typename T>
class LinkedStack {
private:
    // 节点结构
    struct Node {
        T data;
        Node* next;
        
        Node(const T& value, Node* next_node = nullptr) 
            : data(value), next(next_node) {}
    };
    
    Node* top_node;  // 栈顶指针
    size_t size_;    // 栈大小
    
public:
    // 构造函数
    LinkedStack() : top_node(nullptr), size_(0) {}
    
    // 拷贝构造函数
    LinkedStack(const LinkedStack& other) : top_node(nullptr), size_(0) {
        copy_from(other);
    }
    
    // 赋值操作符
    LinkedStack& operator=(const LinkedStack& other) {
        if (this != &other) {
            clear();
            copy_from(other);
        }
        return *this;
    }
    
    // 析构函数
    ~LinkedStack() {
        clear();
    }
    
    // 基本操作
    void push(const T& value) {
        Node* new_node = new Node(value, top_node);
        top_node = new_node;
        ++size_;
    }
    
    void pop() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        
        Node* old_top = top_node;
        top_node = top_node->next;
        delete old_top;
        --size_;
    }
    
    T& top() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return top_node->data;
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return top_node->data;
    }
    
    // 状态查询
    bool empty() const {
        return top_node == nullptr;
    }
    
    size_t size() const {
        return size_;
    }
    
    // 实用函数
    void clear() {
        while (!empty()) {
            pop();
        }
    }
    
    // 输出函数
    void print() const {
        std::cout << "LinkedStack[" << size() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "top [";
            Node* current = top_node;
            while (current != nullptr) {
                std::cout << current->data;
                if (current->next != nullptr) {
                    std::cout << ", ";
                }
                current = current->next;
            }
            std::cout << "] bottom";
        }
        std::cout << std::endl;
    }
    
    // 比较操作
    bool operator==(const LinkedStack& other) const {
        if (size_ != other.size_) {
            return false;
        }
        
        Node* current1 = top_node;
        Node* current2 = other.top_node;
        
        while (current1 != nullptr && current2 != nullptr) {
            if (current1->data != current2->data) {
                return false;
            }
            current1 = current1->next;
            current2 = current2->next;
        }
        
        return current1 == nullptr && current2 == nullptr;
    }
    
    bool operator!=(const LinkedStack& other) const {
        return !(*this == other);
    }
    
    // 交换操作
    void swap(LinkedStack& other) {
        std::swap(top_node, other.top_node);
        std::swap(size_, other.size_);
    }
    
private:
    // 辅助函数：从另一个栈复制数据
    void copy_from(const LinkedStack& other) {
        if (other.empty()) {
            return;
        }
        
        // 使用递归来保持元素顺序
        copy_recursive(other.top_node);
    }
    
    void copy_recursive(Node* node) {
        if (node == nullptr) {
            return;
        }
        
        // 先递归到底部
        copy_recursive(node->next);
        // 然后从底部开始push
        push(node->data);
    }
};

// 带最小值功能的链表栈
template<typename T>
class LinkedStackWithMin {
private:
    struct Node {
        T data;
        T min_val;  // 存储到当前节点为止的最小值
        Node* next;
        
        Node(const T& value, const T& min_value, Node* next_node = nullptr)
            : data(value), min_val(min_value), next(next_node) {}
    };
    
    Node* top_node;
    size_t size_;
    
public:
    LinkedStackWithMin() : top_node(nullptr), size_(0) {}
    
    ~LinkedStackWithMin() {
        clear();
    }
    
    void push(const T& value) {
        T min_value = empty() ? value : std::min(value, top_node->min_val);
        Node* new_node = new Node(value, min_value, top_node);
        top_node = new_node;
        ++size_;
    }
    
    void pop() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        
        Node* old_top = top_node;
        top_node = top_node->next;
        delete old_top;
        --size_;
    }
    
    T& top() {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return top_node->data;
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return top_node->data;
    }
    
    const T& get_min() const {
        if (empty()) {
            throw std::runtime_error("Stack is empty");
        }
        return top_node->min_val;
    }
    
    bool empty() const {
        return top_node == nullptr;
    }
    
    size_t size() const {
        return size_;
    }
    
    void clear() {
        while (!empty()) {
            pop();
        }
    }
    
    void print() const {
        std::cout << "LinkedStackWithMin[" << size() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "top [";
            Node* current = top_node;
            while (current != nullptr) {
                std::cout << "(" << current->data << ",min:" << current->min_val << ")";
                if (current->next != nullptr) {
                    std::cout << ", ";
                }
                current = current->next;
            }
            std::cout << "] bottom";
        }
        std::cout << std::endl;
    }
};

// 使用两个栈实现队列
template<typename T>
class QueueUsingTwoStacks {
private:
    LinkedStack<T> input_stack;   // 输入栈
    LinkedStack<T> output_stack;  // 输出栈
    
    // 将输入栈的所有元素转移到输出栈
    void transfer() {
        while (!input_stack.empty()) {
            output_stack.push(input_stack.top());
            input_stack.pop();
        }
    }
    
public:
    // 入队操作
    void enqueue(const T& value) {
        input_stack.push(value);
    }
    
    // 出队操作
    void dequeue() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        if (output_stack.empty()) {
            transfer();
        }
        
        output_stack.pop();
    }
    
    // 获取队首元素
    T& front() {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        if (output_stack.empty()) {
            transfer();
        }
        
        return output_stack.top();
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("Queue is empty");
        }
        
        if (output_stack.empty()) {
            const_cast<QueueUsingTwoStacks*>(this)->transfer();
        }
        
        return output_stack.top();
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
    
    void print() const {
        std::cout << "QueueUsingTwoStacks[" << size() << "]:" << std::endl;
        std::cout << "  Input stack: ";
        input_stack.print();
        std::cout << "  Output stack: ";
        output_stack.print();
    }
};

#endif // LINKED_STACK_H