#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

#include <iostream>
#include <stdexcept>

template<typename T>
class DoublyLinkedList {
private:
    // 节点结构
    struct Node {
        T data;
        Node* prev;
        Node* next;
        
        Node(const T& value) : data(value), prev(nullptr), next(nullptr) {}
    };
    
    Node* head;     // 头指针
    Node* tail;     // 尾指针
    size_t size_;   // 链表大小
    
public:
    // 构造函数
    DoublyLinkedList();
    
    // 拷贝构造函数
    DoublyLinkedList(const DoublyLinkedList& other);
    
    // 析构函数
    ~DoublyLinkedList();
    
    // 赋值操作符
    DoublyLinkedList& operator=(const DoublyLinkedList& other);
    
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
    int find_reverse(const T& value) const;  // 从尾部开始查找
    bool contains(const T& value) const;
    
    // 实用函数
    void reverse();                     // 反转链表
    void remove_duplicates();           // 删除重复元素
    
    // 输出函数
    void print() const;
    void print_reverse() const;         // 从尾部开始打印
    
    // 双向迭代器支持
    class Iterator {
    private:
        Node* current;
    public:
        Iterator(Node* node) : current(node) {}
        T& operator*() { return current->data; }
        Iterator& operator++() { current = current->next; return *this; }
        Iterator& operator--() { current = current->prev; return *this; }
        bool operator!=(const Iterator& other) const { return current != other.current; }
        bool operator==(const Iterator& other) const { return current == other.current; }
    };
    
    class ReverseIterator {
    private:
        Node* current;
    public:
        ReverseIterator(Node* node) : current(node) {}
        T& operator*() { return current->data; }
        ReverseIterator& operator++() { current = current->prev; return *this; }
        ReverseIterator& operator--() { current = current->next; return *this; }
        bool operator!=(const ReverseIterator& other) const { return current != other.current; }
        bool operator==(const ReverseIterator& other) const { return current == other.current; }
    };
    
    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }
    ReverseIterator rbegin() { return ReverseIterator(tail); }
    ReverseIterator rend() { return ReverseIterator(nullptr); }
    
private:
    // 辅助函数
    Node* get_node(size_t index) const;
    void copy_from(const DoublyLinkedList& other);
};

// 实现部分
template<typename T>
DoublyLinkedList<T>::DoublyLinkedList() : head(nullptr), tail(nullptr), size_(0) {}

template<typename T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList& other) 
    : head(nullptr), tail(nullptr), size_(0) {
    copy_from(other);
}

template<typename T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    clear();
}

template<typename T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList& other) {
    if (this != &other) {
        clear();
        copy_from(other);
    }
    return *this;
}

template<typename T>
void DoublyLinkedList<T>::copy_from(const DoublyLinkedList& other) {
    Node* current = other.head;
    while (current != nullptr) {
        push_back(current->data);
        current = current->next;
    }
}

template<typename T>
T& DoublyLinkedList<T>::front() {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    return head->data;
}

template<typename T>
const T& DoublyLinkedList<T>::front() const {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    return head->data;
}

template<typename T>
T& DoublyLinkedList<T>::back() {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    return tail->data;
}

template<typename T>
const T& DoublyLinkedList<T>::back() const {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    return tail->data;
}

template<typename T>
T& DoublyLinkedList<T>::at(size_t index) {
    Node* node = get_node(index);
    return node->data;
}

template<typename T>
const T& DoublyLinkedList<T>::at(size_t index) const {
    Node* node = get_node(index);
    return node->data;
}

template<typename T>
typename DoublyLinkedList<T>::Node* DoublyLinkedList<T>::get_node(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Index out of range");
    }
    
    Node* current;
    
    // 优化：从较近的一端开始遍历
    if (index < size_ / 2) {
        // 从头开始
        current = head;
        for (size_t i = 0; i < index; ++i) {
            current = current->next;
        }
    } else {
        // 从尾开始
        current = tail;
        for (size_t i = size_ - 1; i > index; --i) {
            current = current->prev;
        }
    }
    
    return current;
}

template<typename T>
void DoublyLinkedList<T>::push_front(const T& value) {
    Node* new_node = new Node(value);
    
    if (empty()) {
        head = tail = new_node;
    } else {
        new_node->next = head;
        head->prev = new_node;
        head = new_node;
    }
    
    ++size_;
}

template<typename T>
void DoublyLinkedList<T>::push_back(const T& value) {
    Node* new_node = new Node(value);
    
    if (empty()) {
        head = tail = new_node;
    } else {
        tail->next = new_node;
        new_node->prev = tail;
        tail = new_node;
    }
    
    ++size_;
}

template<typename T>
void DoublyLinkedList<T>::pop_front() {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    
    Node* old_head = head;
    
    if (size_ == 1) {
        head = tail = nullptr;
    } else {
        head = head->next;
        head->prev = nullptr;
    }
    
    delete old_head;
    --size_;
}

template<typename T>
void DoublyLinkedList<T>::pop_back() {
    if (empty()) {
        throw std::runtime_error("List is empty");
    }
    
    Node* old_tail = tail;
    
    if (size_ == 1) {
        head = tail = nullptr;
    } else {
        tail = tail->prev;
        tail->next = nullptr;
    }
    
    delete old_tail;
    --size_;
}

template<typename T>
void DoublyLinkedList<T>::insert(size_t index, const T& value) {
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
    
    Node* current = get_node(index);
    Node* new_node = new Node(value);
    
    new_node->next = current;
    new_node->prev = current->prev;
    current->prev->next = new_node;
    current->prev = new_node;
    
    ++size_;
}

template<typename T>
void DoublyLinkedList<T>::erase(size_t index) {
    if (index >= size_) {
        throw std::out_of_range("Index out of range");
    }
    
    if (index == 0) {
        pop_front();
        return;
    }
    
    if (index == size_ - 1) {
        pop_back();
        return;
    }
    
    Node* to_delete = get_node(index);
    
    to_delete->prev->next = to_delete->next;
    to_delete->next->prev = to_delete->prev;
    
    delete to_delete;
    --size_;
}

template<typename T>
void DoublyLinkedList<T>::clear() {
    while (!empty()) {
        pop_front();
    }
}

template<typename T>
int DoublyLinkedList<T>::find(const T& value) const {
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
int DoublyLinkedList<T>::find_reverse(const T& value) const {
    Node* current = tail;
    int index = size_ - 1;
    
    while (current != nullptr) {
        if (current->data == value) {
            return index;
        }
        current = current->prev;
        --index;
    }
    
    return -1;
}

template<typename T>
bool DoublyLinkedList<T>::contains(const T& value) const {
    return find(value) != -1;
}

template<typename T>
void DoublyLinkedList<T>::reverse() {
    if (size_ <= 1) return;
    
    Node* current = head;
    
    while (current != nullptr) {
        // 交换prev和next指针
        Node* temp = current->prev;
        current->prev = current->next;
        current->next = temp;
        
        current = current->prev;  // 移动到下一个节点（原来的prev）
    }
    
    // 交换head和tail
    Node* temp = head;
    head = tail;
    tail = temp;
}

template<typename T>
void DoublyLinkedList<T>::remove_duplicates() {
    if (size_ <= 1) return;
    
    Node* current = head;
    
    while (current != nullptr && current->next != nullptr) {
        if (current->data == current->next->data) {
            Node* duplicate = current->next;
            current->next = duplicate->next;
            
            if (duplicate->next != nullptr) {
                duplicate->next->prev = current;
            } else {
                tail = current;  // 删除的是尾节点
            }
            
            delete duplicate;
            --size_;
        } else {
            current = current->next;
        }
    }
}

template<typename T>
void DoublyLinkedList<T>::print() const {
    std::cout << "DoublyList[" << size_ << "]: null <-> ";
    Node* current = head;
    while (current != nullptr) {
        std::cout << current->data;
        if (current->next != nullptr) {
            std::cout << " <-> ";
        }
        current = current->next;
    }
    std::cout << " <-> null" << std::endl;
}

template<typename T>
void DoublyLinkedList<T>::print_reverse() const {
    std::cout << "DoublyList[" << size_ << "] (reverse): null <-> ";
    Node* current = tail;
    while (current != nullptr) {
        std::cout << current->data;
        if (current->prev != nullptr) {
            std::cout << " <-> ";
        }
        current = current->prev;
    }
    std::cout << " <-> null" << std::endl;
}

#endif // DOUBLY_LINKED_LIST_H