#ifndef BINOMIAL_HEAP_H
#define BINOMIAL_HEAP_H

#include <vector>
#include <iostream>
#include <memory>
#include <functional>
#include <algorithm>

// 二项树节点
template<typename T>
struct BinomialNode {
    T key;
    int degree;
    std::shared_ptr<BinomialNode<T>> parent;
    std::shared_ptr<BinomialNode<T>> child;
    std::shared_ptr<BinomialNode<T>> sibling;
    
    BinomialNode(const T& value) 
        : key(value), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
};

// 二项堆类
template<typename T, typename Compare = std::less<T>>
class BinomialHeap {
public:
    using NodePtr = std::shared_ptr<BinomialNode<T>>;
    
private:
    NodePtr head;  // 根链表的头指针
    Compare comp;
    size_t node_count;
    
    // 链接两个相同度数的二项树
    NodePtr link_trees(NodePtr y, NodePtr z) {
        // 确保y的键值不大于z的键值
        if (comp(y->key, z->key)) {
            std::swap(y, z);
        }
        
        // 将z作为y的子树
        z->parent = y;
        z->sibling = y->child;
        y->child = z;
        y->degree++;
        
        return y;
    }
    
    // 合并两个二项堆的根链表
    NodePtr merge_root_lists(NodePtr h1, NodePtr h2) {
        if (!h1) return h2;
        if (!h2) return h1;
        
        NodePtr head_ptr = nullptr;
        NodePtr tail = nullptr;
        
        // 按度数递增顺序合并
        while (h1 && h2) {
            NodePtr* next_node;
            if (h1->degree <= h2->degree) {
                next_node = &h1;
                h1 = h1->sibling;
            } else {
                next_node = &h2;
                h2 = h2->sibling;
            }
            
            if (!head_ptr) {
                head_ptr = tail = *next_node;
            } else {
                tail->sibling = *next_node;
                tail = *next_node;
            }
        }
        
        // 添加剩余的节点
        if (h1) {
            if (!head_ptr) {
                head_ptr = h1;
            } else {
                tail->sibling = h1;
            }
        } else if (h2) {
            if (!head_ptr) {
                head_ptr = h2;
            } else {
                tail->sibling = h2;
            }
        }
        
        return head_ptr;
    }
    
    // 合并操作的核心
    NodePtr union_heaps(NodePtr h1, NodePtr h2) {
        NodePtr merged_head = merge_root_lists(h1, h2);
        if (!merged_head) return nullptr;
        
        NodePtr prev = nullptr;
        NodePtr curr = merged_head;
        NodePtr next = curr->sibling;
        
        while (next) {
            if (curr->degree != next->degree || 
                (next->sibling && next->sibling->degree == curr->degree)) {
                // 情况1：当前和下一个度数不同，或者存在三个相同度数的树
                prev = curr;
                curr = next;
            } else {
                // 情况2：当前和下一个度数相同，需要链接
                if (!comp(curr->key, next->key)) {
                    // curr的键值不小于next的键值
                    if (!prev) {
                        merged_head = next;
                    } else {
                        prev->sibling = next;
                    }
                    curr = link_trees(curr, next);
                } else {
                    // curr的键值小于next的键值
                    curr->sibling = next->sibling;
                    curr = link_trees(next, curr);
                }
            }
            next = curr->sibling;
        }
        
        return merged_head;
    }
    
    // 查找最小键值的节点
    NodePtr find_min_node() const {
        if (!head) return nullptr;
        
        NodePtr min_node = head;
        NodePtr current = head->sibling;
        
        while (current) {
            if (comp(min_node->key, current->key)) {
                min_node = current;
            }
            current = current->sibling;
        }
        
        return min_node;
    }
    
    // 从根链表中移除指定节点
    NodePtr remove_from_root_list(NodePtr node) {
        if (head == node) {
            head = node->sibling;
            return head;
        }
        
        NodePtr current = head;
        while (current && current->sibling != node) {
            current = current->sibling;
        }
        
        if (current) {
            current->sibling = node->sibling;
        }
        
        return head;
    }
    
    // 反转子树链表
    NodePtr reverse_child_list(NodePtr node) {
        if (!node) return nullptr;
        
        NodePtr prev = nullptr;
        NodePtr current = node;
        
        while (current) {
            NodePtr next = current->sibling;
            current->sibling = prev;
            current->parent = nullptr;
            prev = current;
            current = next;
        }
        
        return prev;
    }
    
    // 递归复制子树
    NodePtr copy_tree(NodePtr node) {
        if (!node) return nullptr;
        
        auto new_node = std::make_shared<BinomialNode<T>>(node->key);
        new_node->degree = node->degree;
        
        if (node->child) {
            new_node->child = copy_tree(node->child);
            new_node->child->parent = new_node;
        }
        
        if (node->sibling) {
            new_node->sibling = copy_tree(node->sibling);
        }
        
        return new_node;
    }
    
    // 计算子树大小
    size_t count_nodes(NodePtr node) const {
        if (!node) return 0;
        
        size_t count = 1;
        count += count_nodes(node->child);
        count += count_nodes(node->sibling);
        
        return count;
    }
    
public:
    // 构造函数
    BinomialHeap() : head(nullptr), node_count(0) {}
    
    explicit BinomialHeap(const Compare& compare) 
        : head(nullptr), comp(compare), node_count(0) {}
    
    // 拷贝构造函数
    BinomialHeap(const BinomialHeap& other) 
        : head(copy_tree(other.head)), comp(other.comp), node_count(other.node_count) {}
    
    // 赋值操作符
    BinomialHeap& operator=(const BinomialHeap& other) {
        if (this != &other) {
            clear();
            head = copy_tree(other.head);
            comp = other.comp;
            node_count = other.node_count;
        }
        return *this;
    }
    
    // 移动构造函数
    BinomialHeap(BinomialHeap&& other) noexcept
        : head(std::move(other.head)), comp(std::move(other.comp)), 
          node_count(other.node_count) {
        other.head = nullptr;
        other.node_count = 0;
    }
    
    // 移动赋值操作符
    BinomialHeap& operator=(BinomialHeap&& other) noexcept {
        if (this != &other) {
            clear();
            head = std::move(other.head);
            comp = std::move(other.comp);
            node_count = other.node_count;
            
            other.head = nullptr;
            other.node_count = 0;
        }
        return *this;
    }
    
    // 析构函数
    ~BinomialHeap() {
        clear();
    }
    
    // 基本操作
    void insert(const T& key) {
        auto new_node = std::make_shared<BinomialNode<T>>(key);
        BinomialHeap temp_heap;
        temp_heap.head = new_node;
        temp_heap.node_count = 1;
        
        merge(temp_heap);
        ++node_count;
    }
    
    T extract_min() {
        if (empty()) {
            throw std::runtime_error("Heap is empty");
        }
        
        NodePtr min_node = find_min_node();
        T min_key = min_node->key;
        
        // 从根链表中移除最小节点
        remove_from_root_list(min_node);
        
        // 将最小节点的子树作为新的二项堆
        NodePtr child_list = reverse_child_list(min_node->child);
        
        BinomialHeap child_heap;
        child_heap.head = child_list;
        child_heap.node_count = (1 << min_node->degree) - 1; // 2^degree - 1
        
        merge(child_heap);
        --node_count;
        
        return min_key;
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Heap is empty");
        }
        
        return find_min_node()->key;
    }
    
    // 合并另一个二项堆
    void merge(BinomialHeap& other) {
        head = union_heaps(head, other.head);
        node_count += other.node_count;
        
        other.head = nullptr;
        other.node_count = 0;
    }
    
    // 减少键值
    void decrease_key(NodePtr node, const T& new_key) {
        if (!comp(new_key, node->key)) {
            throw std::invalid_argument("New key is not smaller than current key");
        }
        
        node->key = new_key;
        
        // 向上调整以维持堆性质
        NodePtr current = node;
        NodePtr parent = current->parent;
        
        while (parent && comp(parent->key, current->key)) {
            std::swap(current->key, parent->key);
            current = parent;
            parent = current->parent;
        }
    }
    
    // 删除节点
    void delete_node(NodePtr node) {
        // 将节点的键值减少到负无穷（或最小值）
        T min_value = std::numeric_limits<T>::lowest();
        if constexpr (std::is_floating_point_v<T>) {
            min_value = -std::numeric_limits<T>::infinity();
        }
        
        decrease_key(node, min_value);
        extract_min();
    }
    
    // 状态查询
    bool empty() const {
        return head == nullptr;
    }
    
    size_t size() const {
        return node_count;
    }
    
    void clear() {
        head = nullptr;
        node_count = 0;
    }
    
    // 查找操作（线性搜索）
    NodePtr find(const T& key) const {
        return find_in_tree(head, key);
    }
    
    bool contains(const T& key) const {
        return find(key) != nullptr;
    }
    
    // 获取所有元素（用于调试）
    std::vector<T> get_all_keys() const {
        std::vector<T> keys;
        collect_keys(head, keys);
        return keys;
    }
    
    // 验证堆性质
    bool is_valid_heap() const {
        return check_heap_property(head);
    }
    
    // 打印堆结构
    void print_heap() const {
        if (empty()) {
            std::cout << "(empty binomial heap)" << std::endl;
            return;
        }
        
        std::cout << "Binomial Heap (size: " << size() << "):" << std::endl;
        print_trees(head, 0);
    }
    
    // 获取根链表（用于调试）
    NodePtr get_head() const {
        return head;
    }
    
private:
    // 在子树中查找键值
    NodePtr find_in_tree(NodePtr node, const T& key) const {
        if (!node) return nullptr;
        
        if (node->key == key) {
            return node;
        }
        
        // 在子树中查找
        NodePtr result = find_in_tree(node->child, key);
        if (result) return result;
        
        // 在兄弟树中查找
        return find_in_tree(node->sibling, key);
    }
    
    // 收集所有键值
    void collect_keys(NodePtr node, std::vector<T>& keys) const {
        if (!node) return;
        
        keys.push_back(node->key);
        collect_keys(node->child, keys);
        collect_keys(node->sibling, keys);
    }
    
    // 检查堆性质
    bool check_heap_property(NodePtr node) const {
        if (!node) return true;
        
        // 检查父子关系
        NodePtr child = node->child;
        while (child) {
            if (comp(node->key, child->key)) {
                return false;
            }
            if (!check_heap_property(child)) {
                return false;
            }
            child = child->sibling;
        }
        
        // 检查兄弟节点
        return check_heap_property(node->sibling);
    }
    
    // 打印树结构
    void print_trees(NodePtr node, int depth) const {
        if (!node) return;
        
        // 打印当前节点
        for (int i = 0; i < depth; ++i) {
            std::cout << "  ";
        }
        std::cout << "B" << node->degree << ": " << node->key << std::endl;
        
        // 打印子树
        print_trees(node->child, depth + 1);
        
        // 打印兄弟树
        print_trees(node->sibling, depth);
    }
};

// 最小二项堆
template<typename T>
using MinBinomialHeap = BinomialHeap<T, std::greater<T>>;

// 最大二项堆
template<typename T>
using MaxBinomialHeap = BinomialHeap<T, std::less<T>>;

// 实用函数

// 从数组构建二项堆
template<typename T, typename Compare = std::less<T>>
BinomialHeap<T, Compare> build_binomial_heap(const std::vector<T>& data) {
    BinomialHeap<T, Compare> heap;
    for (const T& value : data) {
        heap.insert(value);
    }
    return heap;
}

// 合并多个二项堆
template<typename T, typename Compare = std::less<T>>
BinomialHeap<T, Compare> merge_multiple_heaps(std::vector<BinomialHeap<T, Compare>>& heaps) {
    if (heaps.empty()) {
        return BinomialHeap<T, Compare>();
    }
    
    BinomialHeap<T, Compare> result = std::move(heaps[0]);
    
    for (size_t i = 1; i < heaps.size(); ++i) {
        result.merge(heaps[i]);
    }
    
    return result;
}

// 堆排序（使用二项堆）
template<typename T>
void binomial_heap_sort(std::vector<T>& arr, bool ascending = true) {
    if (arr.size() <= 1) return;
    
    if (ascending) {
        MinBinomialHeap<T> heap;
        for (const T& value : arr) {
            heap.insert(value);
        }
        
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = heap.extract_min();
        }
    } else {
        MaxBinomialHeap<T> heap;
        for (const T& value : arr) {
            heap.insert(value);
        }
        
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = heap.extract_min();
        }
    }
}

// 二项堆迭代器（中序遍历）
template<typename T>
class BinomialHeapIterator {
private:
    std::vector<T> elements;
    size_t current_index;
    
    void collect_elements(std::shared_ptr<BinomialNode<T>> node) {
        if (!node) return;
        
        elements.push_back(node->key);
        collect_elements(node->child);
        collect_elements(node->sibling);
    }
    
public:
    template<typename Compare>
    explicit BinomialHeapIterator(const BinomialHeap<T, Compare>& heap) 
        : current_index(0) {
        collect_elements(heap.get_head());
        std::sort(elements.begin(), elements.end());
    }
    
    bool has_next() const {
        return current_index < elements.size();
    }
    
    T next() {
        if (!has_next()) {
            throw std::runtime_error("No more elements");
        }
        return elements[current_index++];
    }
    
    void reset() {
        current_index = 0;
    }
};

#endif // BINOMIAL_HEAP_H