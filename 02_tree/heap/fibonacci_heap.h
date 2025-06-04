#ifndef FIBONACCI_HEAP_H
#define FIBONACCI_HEAP_H

#include <vector>
#include <iostream>
#include <memory>
#include <functional>
#include <algorithm>
#include <cmath>
#include <unordered_map>

// 斐波那契堆节点
template<typename T>
struct FibonacciNode {
    T key;
    int degree;
    bool marked;
    std::shared_ptr<FibonacciNode<T>> parent;
    std::shared_ptr<FibonacciNode<T>> child;
    std::shared_ptr<FibonacciNode<T>> left;
    std::shared_ptr<FibonacciNode<T>> right;
    
    FibonacciNode(const T& value) 
        : key(value), degree(0), marked(false), 
          parent(nullptr), child(nullptr) {
        left = right = std::shared_ptr<FibonacciNode<T>>(this, [](FibonacciNode<T>*){});
    }
};

// 斐波那契堆类
template<typename T, typename Compare = std::less<T>>
class FibonacciHeap {
public:
    using NodePtr = std::shared_ptr<FibonacciNode<T>>;
    
private:
    NodePtr min_node;
    size_t node_count;
    Compare comp;
    
    // 将节点插入到根链表中
    void insert_to_root_list(NodePtr node) {
        if (!min_node) {
            min_node = node;
            node->left = node->right = node;
        } else {
            // 插入到min_node的右边
            node->right = min_node->right;
            node->left = min_node;
            min_node->right->left = node;
            min_node->right = node;
            
            // 更新最小节点
            if (comp(min_node->key, node->key)) {
                min_node = node;
            }
        }
        node->parent = nullptr;
    }
    
    // 从链表中移除节点
    void remove_from_list(NodePtr node) {
        if (node->right == node) {
            // 只有一个节点
            return;
        }
        
        node->left->right = node->right;
        node->right->left = node->left;
    }
    
    // 链接两个相同度数的树
    NodePtr link_trees(NodePtr y, NodePtr z) {
        // 确保y的键值不大于z的键值
        if (comp(y->key, z->key)) {
            std::swap(y, z);
        }
        
        // 从根链表中移除z
        remove_from_list(z);
        
        // 将z作为y的子节点
        if (!y->child) {
            y->child = z;
            z->left = z->right = z;
        } else {
            z->right = y->child->right;
            z->left = y->child;
            y->child->right->left = z;
            y->child->right = z;
        }
        
        z->parent = y;
        y->degree++;
        z->marked = false;
        
        return y;
    }
    
    // 合并操作
    void consolidate() {
        if (!min_node) return;
        
        // 计算最大可能的度数
        int max_degree = static_cast<int>(std::log2(node_count)) + 1;
        std::vector<NodePtr> degree_table(max_degree + 1, nullptr);
        
        // 收集所有根节点
        std::vector<NodePtr> root_nodes;
        NodePtr current = min_node;
        do {
            root_nodes.push_back(current);
            current = current->right;
        } while (current != min_node);
        
        // 对每个根节点进行合并
        for (NodePtr node : root_nodes) {
            int degree = node->degree;
            
            while (degree_table[degree]) {
                NodePtr other = degree_table[degree];
                degree_table[degree] = nullptr;
                
                node = link_trees(node, other);
                degree++;
            }
            
            degree_table[degree] = node;
        }
        
        // 重建根链表
        min_node = nullptr;
        for (NodePtr node : degree_table) {
            if (node) {
                insert_to_root_list(node);
            }
        }
    }
    
    // 切断操作
    void cut(NodePtr x, NodePtr y) {
        // 从y的子链表中移除x
        if (x->right == x) {
            y->child = nullptr;
        } else {
            if (y->child == x) {
                y->child = x->right;
            }
            remove_from_list(x);
        }
        
        y->degree--;
        
        // 将x添加到根链表
        insert_to_root_list(x);
        x->marked = false;
    }
    
    // 级联切断
    void cascading_cut(NodePtr y) {
        NodePtr z = y->parent;
        if (z) {
            if (!y->marked) {
                y->marked = true;
            } else {
                cut(y, z);
                cascading_cut(z);
            }
        }
    }
    
    // 递归复制子树
    NodePtr copy_tree(NodePtr node, NodePtr parent = nullptr) {
        if (!node) return nullptr;
        
        auto new_node = std::make_shared<FibonacciNode<T>>(node->key);
        new_node->degree = node->degree;
        new_node->marked = node->marked;
        new_node->parent = parent;
        
        if (node->child) {
            new_node->child = copy_tree(node->child, new_node);
            
            // 复制子链表
            NodePtr current = node->child->right;
            NodePtr new_current = new_node->child;
            
            while (current != node->child) {
                auto new_sibling = copy_tree(current, new_node);
                new_sibling->left = new_current;
                new_sibling->right = new_current->right;
                new_current->right->left = new_sibling;
                new_current->right = new_sibling;
                
                current = current->right;
                new_current = new_sibling;
            }
        }
        
        return new_node;
    }
    
    // 计算子树大小
    size_t count_nodes(NodePtr node) const {
        if (!node) return 0;
        
        size_t count = 1;
        
        if (node->child) {
            NodePtr current = node->child;
            do {
                count += count_nodes(current);
                current = current->right;
            } while (current != node->child);
        }
        
        return count;
    }
    
public:
    // 构造函数
    FibonacciHeap() : min_node(nullptr), node_count(0) {}
    
    explicit FibonacciHeap(const Compare& compare) 
        : min_node(nullptr), node_count(0), comp(compare) {}
    
    // 拷贝构造函数
    FibonacciHeap(const FibonacciHeap& other) 
        : min_node(nullptr), node_count(0), comp(other.comp) {
        if (other.min_node) {
            // 复制根链表
            min_node = copy_tree(other.min_node);
            
            NodePtr current = other.min_node->right;
            NodePtr new_current = min_node;
            
            while (current != other.min_node) {
                auto new_root = copy_tree(current);
                new_root->left = new_current;
                new_root->right = new_current->right;
                new_current->right->left = new_root;
                new_current->right = new_root;
                
                if (comp(min_node->key, new_root->key)) {
                    min_node = new_root;
                }
                
                current = current->right;
                new_current = new_root;
            }
            
            node_count = other.node_count;
        }
    }
    
    // 赋值操作符
    FibonacciHeap& operator=(const FibonacciHeap& other) {
        if (this != &other) {
            clear();
            comp = other.comp;
            
            if (other.min_node) {
                // 复制根链表
                min_node = copy_tree(other.min_node);
                
                NodePtr current = other.min_node->right;
                NodePtr new_current = min_node;
                
                while (current != other.min_node) {
                    auto new_root = copy_tree(current);
                    new_root->left = new_current;
                    new_root->right = new_current->right;
                    new_current->right->left = new_root;
                    new_current->right = new_root;
                    
                    if (comp(min_node->key, new_root->key)) {
                        min_node = new_root;
                    }
                    
                    current = current->right;
                    new_current = new_root;
                }
                
                node_count = other.node_count;
            }
        }
        return *this;
    }
    
    // 移动构造函数
    FibonacciHeap(FibonacciHeap&& other) noexcept
        : min_node(std::move(other.min_node)), 
          node_count(other.node_count), 
          comp(std::move(other.comp)) {
        other.min_node = nullptr;
        other.node_count = 0;
    }
    
    // 移动赋值操作符
    FibonacciHeap& operator=(FibonacciHeap&& other) noexcept {
        if (this != &other) {
            clear();
            min_node = std::move(other.min_node);
            node_count = other.node_count;
            comp = std::move(other.comp);
            
            other.min_node = nullptr;
            other.node_count = 0;
        }
        return *this;
    }
    
    // 析构函数
    ~FibonacciHeap() {
        clear();
    }
    
    // 基本操作
    NodePtr insert(const T& key) {
        auto new_node = std::make_shared<FibonacciNode<T>>(key);
        insert_to_root_list(new_node);
        ++node_count;
        return new_node;
    }
    
    T extract_min() {
        if (empty()) {
            throw std::runtime_error("Heap is empty");
        }
        
        NodePtr z = min_node;
        T min_key = z->key;
        
        // 将z的所有子节点添加到根链表
        if (z->child) {
            NodePtr current = z->child;
            do {
                NodePtr next = current->right;
                insert_to_root_list(current);
                current = next;
            } while (current != z->child);
        }
        
        // 从根链表中移除z
        remove_from_list(z);
        
        if (z == z->right) {
            min_node = nullptr;
        } else {
            min_node = z->right;
            consolidate();
        }
        
        --node_count;
        return min_key;
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return min_node->key;
    }
    
    // 合并另一个斐波那契堆
    void merge(FibonacciHeap& other) {
        if (other.empty()) return;
        
        if (empty()) {
            min_node = other.min_node;
        } else {
            // 连接两个根链表
            NodePtr temp = min_node->right;
            min_node->right = other.min_node->right;
            other.min_node->right->left = min_node;
            other.min_node->right = temp;
            temp->left = other.min_node;
            
            // 更新最小节点
            if (comp(min_node->key, other.min_node->key)) {
                min_node = other.min_node;
            }
        }
        
        node_count += other.node_count;
        
        other.min_node = nullptr;
        other.node_count = 0;
    }
    
    // 减少键值
    void decrease_key(NodePtr node, const T& new_key) {
        if (!comp(new_key, node->key)) {
            throw std::invalid_argument("New key is not smaller than current key");
        }
        
        node->key = new_key;
        NodePtr parent = node->parent;
        
        if (parent && comp(parent->key, node->key)) {
            cut(node, parent);
            cascading_cut(parent);
        }
        
        if (comp(min_node->key, node->key)) {
            min_node = node;
        }
    }
    
    // 删除节点
    void delete_node(NodePtr node) {
        T min_value = std::numeric_limits<T>::lowest();
        if constexpr (std::is_floating_point_v<T>) {
            min_value = -std::numeric_limits<T>::infinity();
        }
        
        decrease_key(node, min_value);
        extract_min();
    }
    
    // 状态查询
    bool empty() const {
        return min_node == nullptr;
    }
    
    size_t size() const {
        return node_count;
    }
    
    void clear() {
        min_node = nullptr;
        node_count = 0;
    }
    
    // 查找操作（线性搜索）
    NodePtr find(const T& key) const {
        if (!min_node) return nullptr;
        
        return find_in_tree(min_node, key);
    }
    
    bool contains(const T& key) const {
        return find(key) != nullptr;
    }
    
    // 获取所有元素（用于调试）
    std::vector<T> get_all_keys() const {
        std::vector<T> keys;
        if (min_node) {
            collect_keys(min_node, keys);
        }
        return keys;
    }
    
    // 验证堆性质
    bool is_valid_heap() const {
        if (!min_node) return true;
        
        return check_heap_property(min_node);
    }
    
    // 打印堆结构
    void print_heap() const {
        if (empty()) {
            std::cout << "(empty fibonacci heap)" << std::endl;
            return;
        }
        
        std::cout << "Fibonacci Heap (size: " << size() << ", min: " 
                  << min_node->key << "):" << std::endl;
        print_trees(min_node, 0);
    }
    
    // 获取根节点（用于调试）
    NodePtr get_min_node() const {
        return min_node;
    }
    
    // 统计信息
    void print_statistics() const {
        std::cout << "Fibonacci Heap Statistics:" << std::endl;
        std::cout << "  Size: " << size() << std::endl;
        std::cout << "  Empty: " << (empty() ? "Yes" : "No") << std::endl;
        
        if (!empty()) {
            std::cout << "  Min key: " << min_node->key << std::endl;
            
            // 统计根节点数量
            int root_count = 0;
            NodePtr current = min_node;
            do {
                root_count++;
                current = current->right;
            } while (current != min_node);
            
            std::cout << "  Root count: " << root_count << std::endl;
            std::cout << "  Valid heap: " << (is_valid_heap() ? "Yes" : "No") << std::endl;
        }
    }
    
private:
    // 在子树中查找键值
    NodePtr find_in_tree(NodePtr start, const T& key) const {
        if (!start) return nullptr;
        
        NodePtr current = start;
        do {
            if (current->key == key) {
                return current;
            }
            
            // 在子树中查找
            if (current->child) {
                NodePtr result = find_in_tree(current->child, key);
                if (result) return result;
            }
            
            current = current->right;
        } while (current != start);
        
        return nullptr;
    }
    
    // 收集所有键值
    void collect_keys(NodePtr start, std::vector<T>& keys) const {
        if (!start) return;
        
        NodePtr current = start;
        do {
            keys.push_back(current->key);
            
            if (current->child) {
                collect_keys(current->child, keys);
            }
            
            current = current->right;
        } while (current != start);
    }
    
    // 检查堆性质
    bool check_heap_property(NodePtr start) const {
        if (!start) return true;
        
        NodePtr current = start;
        do {
            // 检查父子关系
            if (current->child) {
                NodePtr child = current->child;
                do {
                    if (comp(current->key, child->key)) {
                        return false;
                    }
                    if (child->parent != current) {
                        return false;
                    }
                    if (!check_heap_property(child)) {
                        return false;
                    }
                    child = child->right;
                } while (child != current->child);
            }
            
            current = current->right;
        } while (current != start);
        
        return true;
    }
    
    // 打印树结构
    void print_trees(NodePtr start, int depth) const {
        if (!start) return;
        
        NodePtr current = start;
        do {
            // 打印当前节点
            for (int i = 0; i < depth; ++i) {
                std::cout << "  ";
            }
            std::cout << "F" << current->degree << ": " << current->key;
            if (current->marked) std::cout << " (marked)";
            std::cout << std::endl;
            
            // 打印子树
            if (current->child) {
                print_trees(current->child, depth + 1);
            }
            
            current = current->right;
        } while (current != start);
    }
};

// 最小斐波那契堆
template<typename T>
using MinFibonacciHeap = FibonacciHeap<T, std::greater<T>>;

// 最大斐波那契堆
template<typename T>
using MaxFibonacciHeap = FibonacciHeap<T, std::less<T>>;

// 实用函数

// 从数组构建斐波那契堆
template<typename T, typename Compare = std::less<T>>
FibonacciHeap<T, Compare> build_fibonacci_heap(const std::vector<T>& data) {
    FibonacciHeap<T, Compare> heap;
    for (const T& value : data) {
        heap.insert(value);
    }
    return heap;
}

// 合并多个斐波那契堆
template<typename T, typename Compare = std::less<T>>
FibonacciHeap<T, Compare> merge_multiple_fibonacci_heaps(
    std::vector<FibonacciHeap<T, Compare>>& heaps) {
    if (heaps.empty()) {
        return FibonacciHeap<T, Compare>();
    }
    
    FibonacciHeap<T, Compare> result = std::move(heaps[0]);
    
    for (size_t i = 1; i < heaps.size(); ++i) {
        result.merge(heaps[i]);
    }
    
    return result;
}

// 堆排序（使用斐波那契堆）
template<typename T>
void fibonacci_heap_sort(std::vector<T>& arr, bool ascending = true) {
    if (arr.size() <= 1) return;
    
    if (ascending) {
        MinFibonacciHeap<T> heap;
        for (const T& value : arr) {
            heap.insert(value);
        }
        
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = heap.extract_min();
        }
    } else {
        MaxFibonacciHeap<T> heap;
        for (const T& value : arr) {
            heap.insert(value);
        }
        
        for (size_t i = 0; i < arr.size(); ++i) {
            arr[i] = heap.extract_min();
        }
    }
}

// 斐波那契堆迭代器
template<typename T>
class FibonacciHeapIterator {
private:
    std::vector<T> elements;
    size_t current_index;
    
    void collect_elements(std::shared_ptr<FibonacciNode<T>> start) {
        if (!start) return;
        
        auto current = start;
        do {
            elements.push_back(current->key);
            
            if (current->child) {
                collect_elements(current->child);
            }
            
            current = current->right;
        } while (current != start);
    }
    
public:
    template<typename Compare>
    explicit FibonacciHeapIterator(const FibonacciHeap<T, Compare>& heap) 
        : current_index(0) {
        collect_elements(heap.get_min_node());
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

// 支持decrease_key的优先队列（基于斐波那契堆）
template<typename T, typename Compare = std::less<T>>
class PriorityQueueWithDecreaseKey {
public:
    using NodePtr = std::shared_ptr<FibonacciNode<T>>;
    
private:
    FibonacciHeap<T, Compare> heap;
    std::unordered_map<T, NodePtr> key_to_node;
    
public:
    NodePtr insert(const T& key) {
        NodePtr node = heap.insert(key);
        key_to_node[key] = node;
        return node;
    }
    
    T extract_min() {
        T min_key = heap.extract_min();
        key_to_node.erase(min_key);
        return min_key;
    }
    
    void decrease_key(const T& old_key, const T& new_key) {
        auto it = key_to_node.find(old_key);
        if (it == key_to_node.end()) {
            throw std::invalid_argument("Key not found");
        }
        
        NodePtr node = it->second;
        heap.decrease_key(node, new_key);
        
        key_to_node.erase(it);
        key_to_node[new_key] = node;
    }
    
    void delete_key(const T& key) {
        auto it = key_to_node.find(key);
        if (it == key_to_node.end()) {
            throw std::invalid_argument("Key not found");
        }
        
        heap.delete_node(it->second);
        key_to_node.erase(it);
    }
    
    const T& top() const {
        return heap.top();
    }
    
    bool empty() const {
        return heap.empty();
    }
    
    size_t size() const {
        return heap.size();
    }
    
    void clear() {
        heap.clear();
        key_to_node.clear();
    }
};

#endif // FIBONACCI_HEAP_H