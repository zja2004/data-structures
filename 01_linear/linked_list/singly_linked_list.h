#ifndef SINGLY_LINKED_LIST_H
#define SINGLY_LINKED_LIST_H

#include <iostream>
#include <stdexcept>

template<typename T>
class SinglyLinkedList {
private:
    // 节点结构
    struct Node {
        T data;
        Node* next;
        
        Node(const T& value) : data(value), next(nullptr) {}
    };
    
    Node* head;     // 头指针
    Node* tail;     // 尾指针（可选，用于优化尾部操作）
    size_t size_;   // 链表大小
    
public:
    // 构造函数
    SinglyLinkedList();
    
    // 拷贝构造函数
    SinglyLinkedList(const SinglyLinkedList& other);
    
    // 析构函数
    ~SinglyLinkedList();
    
    // 赋值操作符
    SinglyLinkedList& operator=(const SinglyLinkedList& other);
    
    // 基本信息
    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    
    // 元素访问
    T& front();
    const T& front() const;
    T& back();
    const T& back() const;
    T& at(size_t index);
    const T& at(size_t index) const;
    
    // 修改操作
    void push_front(const T& value);    // 头部插入
    void push_back(const T& value);     // 尾部插入
    void pop_front();                   // 头部删除
    void pop_back();                    // 尾部删除
    void insert(size_t index, const T& value);  // 指定位置插入
    void erase(size_t index);           // 指定位置删除
    void clear();                       // 清空链表
    
    // 查找操作
    int find(const T& value) const;
    bool contains(const T& value) const;
    
    // 实用函数
    void reverse();                     // 反转链表
    void remove_duplicates();           // 删除重复元素
    void sort();                        // 排序（归并排序）
    
    // 输出函数
    void print() const;
    void print_reverse() const;         // 递归反向打印
    
    // 高级操作
    bool has_cycle() const;             // 检测环
    size_t find_middle() const;         // 找中间节点
    void merge_sorted(const SinglyLinkedList& other);  // 合并有序链表
    
    // 迭代器支持（简化版）
    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* node) : current(node) {}
        T& operator*() { return current->data; }
        Iterator& operator++() { current = current->next; return *this; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
        bool operator==(const Iterator& other) const { return current == other.current; }
    };
    
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    
private:
    // 辅助函数
    Node* get_node(size_t index) const;
    void copy_from(const SinglyLinkedList& other);
    Node* merge_sort(Node* head);
    Node* merge(Node* left, Node* right);
    Node* get_middle(Node* head);
    void print_reverse_helper(Node* node) const;
};

// 实现部分
template<typename T>
SinglyLinkedList<T>::SinglyLinkedList() : head(nullptr), tail(nullptr), size_(0) {}

template<typename T>
SinglyLinkedList<T>::SinglyLinkedList(const SinglyLinkedList& other) 
    : head(nullptr), tail(nullptr), size_(0) {
    copy_from(other);
}

template<typename T>
SinglyLinkedList<T>::~SinglyLinkedList() {
    clear();
}

template<typename T>
SinglyLinkedList<T>& SinglyLinkedList<T>::operator=(const SinglyLinkedList& other) {
    if (this != &other) {
        clear();
        copy_from(other);
    }
    return *this;
}

template<typename T>
void SinglyLinkedList<T>::copy_from(const SinglyLinkedList& other) {
    Node* current = other.head;
    while (current != nullptr) {
        push_back(current->data);
        current = current->next;
    }
}

template<typename T>
T& SinglyLinkedList<T>::front() {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    return head->data;
}

template<typename T>
const T& SinglyLinkedList<T>::front() const {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    return head->data;
}

template<typename T>
T& SinglyLinkedList<T>::back() {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    return tail->data;
}

template<typename T>
const T& SinglyLinkedList<T>::back() const {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    return tail->data;
}

template<typename T>
T& SinglyLinkedList<T>::at(size_t index) {
    Node* node = get_node(index);
    return node->data;
}

template<typename T>
const T& SinglyLinkedList<T>::at(size_t index) const {
    Node* node = get_node(index);
    return node->data;
}

template<typename T>
typename SinglyLinkedList<T>::Node* SinglyLinkedList<T>::get_node(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Index out of range");
    }
    
    Node* current = head;
    for (size_t i = 0; i < index; ++i) {
        current = current->next;
    }
    return current;
}

template<typename T>
void SinglyLinkedList<T>::push_front(const T& value) {
    Node* new_node = new Node(value);
    new_node->next = head;
    head = new_node;
    
    if (tail == nullptr) {
        tail = head;
    }
    
    ++size_;
}

template<typename T>
void SinglyLinkedList<T>::push_back(const T& value) {
    Node* new_node = new Node(value);
    
    if (empty()) {
        head = tail = new_node;
    } else {
        tail->next = new_node;
        tail = new_node;
    }
    
    ++size_;
}

template<typename T>
void SinglyLinkedList<T>::pop_front() {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    
    Node* old_head = head;
    head = head->next;
    delete old_head;
    
    if (head == nullptr) {
        tail = nullptr;
    }
    
    --size_;
}

template<typename T>
void SinglyLinkedList<T>::pop_back() {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    
    if (size_ == 1) {
        delete head;
        head = tail = nullptr;
    } else {
        Node* current = head;
        while (current->next != tail) {
            current = current->next;
        }
        delete tail;
        tail = current;
        tail->next = nullptr;
    }
    
    --size_;
}

template<typename T>
void SinglyLinkedList<T>::insert(size_t index, const T& value) {
    if (index > size_) {
        throw std::out_of_range("Index out of range");
    }
    
    if (index == 0) {
        push_front(value);
        return;
    }
    
    if (index == size_) {
        push_back(value);
        return;
    }
    
    Node* prev = get_node(index - 1);
    Node* new_node = new Node(value);
    new_node->next = prev->next;
    prev->next = new_node;
    
    ++size_;
}

template<typename T>
void SinglyLinkedList<T>::erase(size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Index out of range");
    }
    
    if (index == 0) {
        pop_front();
        return;
    }
    
    Node* prev = get_node(index - 1);
    Node* to_delete = prev->next;
    prev->next = to_delete->next;
    
    if (to_delete == tail) {
        tail = prev;
    }
    
    delete to_delete;
    --size_;
}

template<typename T>
void SinglyLinkedList<T>::clear() {
    while (!empty()) {
        pop_front();
    }
}

template<typename T>
int SinglyLinkedList<T>::find(const T& value) const {
    Node* current = head;
    int index = 0;
    
    while (current != nullptr) {
        if (current->data == value) {
            return index;
        }
        current = current->next;
        ++index;
    }
    
    return -1;
}

template<typename T>
bool SinglyLinkedList<T>::contains(const T& value) const {
    return find(value) != -1;
}

template<typename T>
void SinglyLinkedList<T>::reverse() {
    if (size_ <= 1) return;
    
    Node* prev = nullptr;
    Node* current = head;
    tail = head;  // 原来的头变成尾
    
    while (current != nullptr) {
        Node* next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    
    head = prev;
}

template<typename T>
void SinglyLinkedList<T>::print() const {
    std::cout << "List[" << size_ << "]: ";
    Node* current = head;
    while (current != nullptr) {
        std::cout << current->data;
        if (current->next != nullptr) {
            std::cout << " -> ";
        }
        current = current->next;
    }
    std::cout << " -> null" << std::endl;
}

template<typename T>
void SinglyLinkedList<T>::print_reverse() const {
    std::cout << "Reverse List[" << size_ << "]: ";
    print_reverse_helper(head);
    std::cout << std::endl;
}

template<typename T>
void SinglyLinkedList<T>::print_reverse_helper(Node* node) const {
    if (node == nullptr) return;
    
    print_reverse_helper(node->next);
    std::cout << node->data;
    if (node != head) {
        std::cout << " -> ";
    }
}

template<typename T>
bool SinglyLinkedList<T>::has_cycle() const {
    if (empty()) return false;
    
    Node* slow = head;
    Node* fast = head;
    
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
        
        if (slow == fast) {
            return true;
        }
    }
    
    return false;
}

template<typename T>
size_t SinglyLinkedList<T>::find_middle() const {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    
    Node* slow = head;
    Node* fast = head;
    size_t index = 0;
    
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
        ++index;
    }
    
    return index;
}

#endif // SINGLY_LINKED_LIST_H