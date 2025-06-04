#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>

// 基于二叉堆的优先队列实现
template<typename T, typename Compare = std::less<T>>
class PriorityQueue {
private:
    std::vector<T> heap;
    Compare comp;
    
public:
    // 构造函数
    PriorityQueue() = default;
    
    explicit PriorityQueue(const Compare& compare) : comp(compare) {}
    
    // 从容器构造
    template<typename Iterator>
    PriorityQueue(Iterator first, Iterator last, const Compare& compare = Compare()) 
        : heap(first, last), comp(compare) {
        build_heap();
    }
    
    // 从初始化列表构造
    PriorityQueue(std::initializer_list<T> init, const Compare& compare = Compare()) 
        : heap(init), comp(compare) {
        build_heap();
    }
    
    // 基本操作
    void push(const T& value) {
        heap.push_back(value);
        heapify_up(heap.size() - 1);
    }
    
    void pop() {
        if (empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        
        // 将最后一个元素移到根部
        heap[0] = heap.back();
        heap.pop_back();
        
        // 向下调整
        if (!empty()) {
            heapify_down(0);
        }
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return heap[0];
    }
    
    // 状态查询
    bool empty() const {
        return heap.empty();
    }
    
    size_t size() const {
        return heap.size();
    }
    
    // 实用函数
    void clear() {
        heap.clear();
    }
    
    void reserve(size_t capacity) {
        heap.reserve(capacity);
    }
    
    void shrink_to_fit() {
        heap.shrink_to_fit();
    }
    
    // 输出函数（层序遍历）
    void print() const {
        std::cout << "PriorityQueue[" << size() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "top [" << top() << "] heap: [";
            for (size_t i = 0; i < heap.size(); ++i) {
                std::cout << heap[i];
                if (i < heap.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    
    // 以树形结构打印堆
    void print_tree() const {
        if (empty()) {
            std::cout << "Empty heap" << std::endl;
            return;
        }
        
        std::cout << "Heap tree structure:" << std::endl;
        print_tree_helper(0, 0);
    }
    
    // 验证堆性质
    bool is_valid_heap() const {
        for (size_t i = 0; i < heap.size(); ++i) {
            size_t left = 2 * i + 1;
            size_t right = 2 * i + 2;
            
            if (left < heap.size() && comp(heap[i], heap[left])) {
                return false;
            }
            if (right < heap.size() && comp(heap[i], heap[right])) {
                return false;
            }
        }
        return true;
    }
    
private:
    // 向上调整（用于插入）
    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (!comp(heap[parent], heap[index])) {
                break;
            }
            std::swap(heap[parent], heap[index]);
            index = parent;
        }
    }
    
    // 向下调整（用于删除）
    void heapify_down(size_t index) {
        size_t size = heap.size();
        
        while (true) {
            size_t largest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            
            if (left < size && comp(heap[largest], heap[left])) {
                largest = left;
            }
            
            if (right < size && comp(heap[largest], heap[right])) {
                largest = right;
            }
            
            if (largest == index) {
                break;
            }
            
            std::swap(heap[index], heap[largest]);
            index = largest;
        }
    }
    
    // 建堆（Floyd算法）
    void build_heap() {
        if (heap.size() <= 1) {
            return;
        }
        
        // 从最后一个非叶子节点开始向下调整
        for (int i = (heap.size() - 2) / 2; i >= 0; --i) {
            heapify_down(i);
        }
    }
    
    // 树形打印辅助函数
    void print_tree_helper(size_t index, int depth) const {
        if (index >= heap.size()) {
            return;
        }
        
        // 先打印右子树
        print_tree_helper(2 * index + 2, depth + 1);
        
        // 打印当前节点
        for (int i = 0; i < depth; ++i) {
            std::cout << "    ";
        }
        std::cout << heap[index] << std::endl;
        
        // 再打印左子树
        print_tree_helper(2 * index + 1, depth + 1);
    }
};

// 最小堆类型别名
template<typename T>
using MinPriorityQueue = PriorityQueue<T, std::greater<T>>;

// 最大堆类型别名
template<typename T>
using MaxPriorityQueue = PriorityQueue<T, std::less<T>>;

// 可更新优先队列（支持修改元素优先级）
template<typename T, typename Compare = std::less<T>>
class UpdatablePriorityQueue {
private:
    struct Element {
        T value;
        size_t id;
        
        Element(const T& val, size_t identifier) : value(val), id(identifier) {}
    };
    
    struct ElementCompare {
        Compare comp;
        
        ElementCompare(const Compare& c = Compare()) : comp(c) {}
        
        bool operator()(const Element& a, const Element& b) const {
            return comp(a.value, b.value);
        }
    };
    
    std::vector<Element> heap;
    std::vector<size_t> position;  // id到堆中位置的映射
    ElementCompare comp;
    size_t next_id;
    
public:
    // 构造函数
    UpdatablePriorityQueue(const Compare& compare = Compare()) 
        : comp(compare), next_id(0) {}
    
    // 插入元素，返回元素ID
    size_t push(const T& value) {
        size_t id = next_id++;
        
        // 扩展position数组
        if (id >= position.size()) {
            position.resize(id + 1, SIZE_MAX);
        }
        
        heap.emplace_back(value, id);
        position[id] = heap.size() - 1;
        
        heapify_up(heap.size() - 1);
        
        return id;
    }
    
    // 删除顶部元素
    void pop() {
        if (empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        
        size_t top_id = heap[0].id;
        position[top_id] = SIZE_MAX;  // 标记为已删除
        
        if (heap.size() == 1) {
            heap.pop_back();
            return;
        }
        
        // 将最后一个元素移到根部
        heap[0] = heap.back();
        position[heap[0].id] = 0;
        heap.pop_back();
        
        heapify_down(0);
    }
    
    // 更新指定ID的元素值
    void update(size_t id, const T& new_value) {
        if (id >= position.size() || position[id] == SIZE_MAX) {
            throw std::runtime_error("Invalid element ID");
        }
        
        size_t pos = position[id];
        T old_value = heap[pos].value;
        heap[pos].value = new_value;
        
        // 根据新旧值的比较结果决定向上还是向下调整
        if (comp(Element(old_value, 0), Element(new_value, 0))) {
            heapify_up(pos);
        } else {
            heapify_down(pos);
        }
    }
    
    // 删除指定ID的元素
    void erase(size_t id) {
        if (id >= position.size() || position[id] == SIZE_MAX) {
            throw std::runtime_error("Invalid element ID");
        }
        
        size_t pos = position[id];
        position[id] = SIZE_MAX;  // 标记为已删除
        
        if (pos == heap.size() - 1) {
            heap.pop_back();
            return;
        }
        
        // 将最后一个元素移到要删除的位置
        T old_value = heap[pos].value;
        heap[pos] = heap.back();
        position[heap[pos].id] = pos;
        heap.pop_back();
        
        // 调整堆
        if (comp(Element(old_value, 0), heap[pos])) {
            heapify_up(pos);
        } else {
            heapify_down(pos);
        }
    }
    
    // 获取指定ID的元素值
    const T& get(size_t id) const {
        if (id >= position.size() || position[id] == SIZE_MAX) {
            throw std::runtime_error("Invalid element ID");
        }
        return heap[position[id]].value;
    }
    
    // 检查ID是否存在
    bool contains(size_t id) const {
        return id < position.size() && position[id] != SIZE_MAX;
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return heap[0].value;
    }
    
    size_t top_id() const {
        if (empty()) {
            throw std::runtime_error("Priority queue is empty");
        }
        return heap[0].id;
    }
    
    bool empty() const {
        return heap.empty();
    }
    
    size_t size() const {
        return heap.size();
    }
    
    void clear() {
        heap.clear();
        position.clear();
        next_id = 0;
    }
    
    void print() const {
        std::cout << "UpdatablePriorityQueue[" << size() << "]: ";
        if (empty()) {
            std::cout << "(empty)";
        } else {
            std::cout << "top [" << top() << "(id:" << top_id() << ")] heap: [";
            for (size_t i = 0; i < heap.size(); ++i) {
                std::cout << heap[i].value << "(id:" << heap[i].id << ")";
                if (i < heap.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    
private:
    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (!comp(heap[parent], heap[index])) {
                break;
            }
            
            // 交换元素并更新位置映射
            std::swap(heap[parent], heap[index]);
            position[heap[parent].id] = parent;
            position[heap[index].id] = index;
            
            index = parent;
        }
    }
    
    void heapify_down(size_t index) {
        size_t size = heap.size();
        
        while (true) {
            size_t largest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;
            
            if (left < size && comp(heap[largest], heap[left])) {
                largest = left;
            }
            
            if (right < size && comp(heap[largest], heap[right])) {
                largest = right;
            }
            
            if (largest == index) {
                break;
            }
            
            // 交换元素并更新位置映射
            std::swap(heap[index], heap[largest]);
            position[heap[index].id] = index;
            position[heap[largest].id] = largest;
            
            index = largest;
        }
    }
};

#endif // PRIORITY_QUEUE_H