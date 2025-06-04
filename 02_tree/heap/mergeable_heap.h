#ifndef MERGEABLE_HEAP_H
#define MERGEABLE_HEAP_H

#include <memory>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <functional>

using namespace std;

// 左偏堆节点
template<typename T>
struct LeftistHeapNode {
    T value;
    int npl;  // null path length
    shared_ptr<LeftistHeapNode> left;
    shared_ptr<LeftistHeapNode> right;
    
    LeftistHeapNode(const T& val) : value(val), npl(0), left(nullptr), right(nullptr) {}
};

// 左偏堆
template<typename T, typename Compare = less<T>>
class LeftistHeap {
private:
    using NodePtr = shared_ptr<LeftistHeapNode<T>>;
    NodePtr root;
    Compare comp;
    size_t heap_size;
    
    // 合并两个左偏堆
    NodePtr merge(NodePtr h1, NodePtr h2) {
        if (!h1) return h2;
        if (!h2) return h1;
        
        // 确保h1的根节点更优
        if (comp(h1->value, h2->value)) {
            swap(h1, h2);
        }
        
        // 递归合并h1的右子树和h2
        h1->right = merge(h1->right, h2);
        
        // 维护左偏性质
        if (!h1->left || (h1->right && h1->left->npl < h1->right->npl)) {
            swap(h1->left, h1->right);
        }
        
        // 更新npl
        h1->npl = h1->right ? h1->right->npl + 1 : 0;
        
        return h1;
    }
    
    // 复制子树
    NodePtr copy_tree(NodePtr node) {
        if (!node) return nullptr;
        
        auto new_node = make_shared<LeftistHeapNode<T>>(node->value);
        new_node->npl = node->npl;
        new_node->left = copy_tree(node->left);
        new_node->right = copy_tree(node->right);
        
        return new_node;
    }
    
    // 计算子树大小
    size_t count_nodes(NodePtr node) {
        if (!node) return 0;
        return 1 + count_nodes(node->left) + count_nodes(node->right);
    }
    
public:
    // 构造函数
    explicit LeftistHeap(const Compare& cmp = Compare()) 
        : root(nullptr), comp(cmp), heap_size(0) {}
    
    // 拷贝构造函数
    LeftistHeap(const LeftistHeap& other) 
        : root(copy_tree(other.root)), comp(other.comp), heap_size(other.heap_size) {}
    
    // 移动构造函数
    LeftistHeap(LeftistHeap&& other) noexcept
        : root(move(other.root)), comp(move(other.comp)), heap_size(other.heap_size) {
        other.heap_size = 0;
    }
    
    // 拷贝赋值运算符
    LeftistHeap& operator=(const LeftistHeap& other) {
        if (this != &other) {
            root = copy_tree(other.root);
            comp = other.comp;
            heap_size = other.heap_size;
        }
        return *this;
    }
    
    // 移动赋值运算符
    LeftistHeap& operator=(LeftistHeap&& other) noexcept {
        if (this != &other) {
            root = move(other.root);
            comp = move(other.comp);
            heap_size = other.heap_size;
            other.heap_size = 0;
        }
        return *this;
    }
    
    // 插入元素
    void insert(const T& value) {
        auto new_node = make_shared<LeftistHeapNode<T>>(value);
        root = merge(root, new_node);
        heap_size++;
    }
    
    // 删除并返回最值
    T extract() {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        
        T result = root->value;
        root = merge(root->left, root->right);
        heap_size--;
        
        return result;
    }
    
    // 查看最值
    const T& top() const {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        return root->value;
    }
    
    // 合并另一个堆
    void merge_with(LeftistHeap& other) {
        root = merge(root, other.root);
        heap_size += other.heap_size;
        
        other.root = nullptr;
        other.heap_size = 0;
    }
    
    // 检查是否为空
    bool empty() const {
        return root == nullptr;
    }
    
    // 获取大小
    size_t size() const {
        return heap_size;
    }
    
    // 清空堆
    void clear() {
        root = nullptr;
        heap_size = 0;
    }
    
    // 验证左偏性质
    bool is_valid_leftist() const {
        return check_leftist_property(root);
    }
    
private:
    bool check_leftist_property(NodePtr node) const {
        if (!node) return true;
        
        int left_npl = node->left ? node->left->npl : -1;
        int right_npl = node->right ? node->right->npl : -1;
        
        // 检查npl计算是否正确
        if (node->npl != right_npl + 1) {
            return false;
        }
        
        // 检查左偏性质
        if (left_npl < right_npl) {
            return false;
        }
        
        return check_leftist_property(node->left) && check_leftist_property(node->right);
    }
};

// 斜堆节点
template<typename T>
struct SkewHeapNode {
    T value;
    shared_ptr<SkewHeapNode> left;
    shared_ptr<SkewHeapNode> right;
    
    SkewHeapNode(const T& val) : value(val), left(nullptr), right(nullptr) {}
};

// 斜堆
template<typename T, typename Compare = less<T>>
class SkewHeap {
private:
    using NodePtr = shared_ptr<SkewHeapNode<T>>;
    NodePtr root;
    Compare comp;
    size_t heap_size;
    
    // 合并两个斜堆
    NodePtr merge(NodePtr h1, NodePtr h2) {
        if (!h1) return h2;
        if (!h2) return h1;
        
        // 确保h1的根节点更优
        if (comp(h1->value, h2->value)) {
            swap(h1, h2);
        }
        
        // 递归合并h1的右子树和h2
        h1->right = merge(h1->right, h2);
        
        // 交换左右子树（斜堆的特点）
        swap(h1->left, h1->right);
        
        return h1;
    }
    
    // 复制子树
    NodePtr copy_tree(NodePtr node) {
        if (!node) return nullptr;
        
        auto new_node = make_shared<SkewHeapNode<T>>(node->value);
        new_node->left = copy_tree(node->left);
        new_node->right = copy_tree(node->right);
        
        return new_node;
    }
    
public:
    // 构造函数
    explicit SkewHeap(const Compare& cmp = Compare()) 
        : root(nullptr), comp(cmp), heap_size(0) {}
    
    // 拷贝构造函数
    SkewHeap(const SkewHeap& other) 
        : root(copy_tree(other.root)), comp(other.comp), heap_size(other.heap_size) {}
    
    // 移动构造函数
    SkewHeap(SkewHeap&& other) noexcept
        : root(move(other.root)), comp(move(other.comp)), heap_size(other.heap_size) {
        other.heap_size = 0;
    }
    
    // 拷贝赋值运算符
    SkewHeap& operator=(const SkewHeap& other) {
        if (this != &other) {
            root = copy_tree(other.root);
            comp = other.comp;
            heap_size = other.heap_size;
        }
        return *this;
    }
    
    // 移动赋值运算符
    SkewHeap& operator=(SkewHeap&& other) noexcept {
        if (this != &other) {
            root = move(other.root);
            comp = move(other.comp);
            heap_size = other.heap_size;
            other.heap_size = 0;
        }
        return *this;
    }
    
    // 插入元素
    void insert(const T& value) {
        auto new_node = make_shared<SkewHeapNode<T>>(value);
        root = merge(root, new_node);
        heap_size++;
    }
    
    // 删除并返回最值
    T extract() {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        
        T result = root->value;
        root = merge(root->left, root->right);
        heap_size--;
        
        return result;
    }
    
    // 查看最值
    const T& top() const {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        return root->value;
    }
    
    // 合并另一个堆
    void merge_with(SkewHeap& other) {
        root = merge(root, other.root);
        heap_size += other.heap_size;
        
        other.root = nullptr;
        other.heap_size = 0;
    }
    
    // 检查是否为空
    bool empty() const {
        return root == nullptr;
    }
    
    // 获取大小
    size_t size() const {
        return heap_size;
    }
    
    // 清空堆
    void clear() {
        root = nullptr;
        heap_size = 0;
    }
};

// 二项堆节点
template<typename T>
struct BinomialHeapNode {
    T value;
    int degree;
    shared_ptr<BinomialHeapNode> parent;
    shared_ptr<BinomialHeapNode> child;
    shared_ptr<BinomialHeapNode> sibling;
    
    BinomialHeapNode(const T& val) 
        : value(val), degree(0), parent(nullptr), child(nullptr), sibling(nullptr) {}
};

// 二项堆
template<typename T, typename Compare = less<T>>
class BinomialHeap {
private:
    using NodePtr = shared_ptr<BinomialHeapNode<T>>;
    NodePtr head;  // 根链表的头
    Compare comp;
    size_t heap_size;
    
    // 链接两个相同度数的二项树
    NodePtr link_trees(NodePtr y, NodePtr z) {
        if (comp(y->value, z->value)) {
            swap(y, z);
        }
        
        y->parent = z;
        y->sibling = z->child;
        z->child = y;
        z->degree++;
        
        return z;
    }
    
    // 合并两个根链表
    NodePtr merge_root_lists(NodePtr h1, NodePtr h2) {
        if (!h1) return h2;
        if (!h2) return h1;
        
        NodePtr result = nullptr;
        NodePtr* current = &result;
        
        while (h1 && h2) {
            if (h1->degree <= h2->degree) {
                *current = h1;
                h1 = h1->sibling;
            } else {
                *current = h2;
                h2 = h2->sibling;
            }
            current = &((*current)->sibling);
        }
        
        *current = h1 ? h1 : h2;
        return result;
    }
    
    // 合并两个二项堆
    NodePtr union_heaps(NodePtr h1, NodePtr h2) {
        NodePtr merged = merge_root_lists(h1, h2);
        if (!merged) return nullptr;
        
        NodePtr prev = nullptr;
        NodePtr current = merged;
        NodePtr next = current->sibling;
        
        while (next) {
            if (current->degree != next->degree || 
                (next->sibling && next->sibling->degree == current->degree)) {
                prev = current;
                current = next;
            } else {
                if (comp(current->value, next->value)) {
                    current->sibling = next->sibling;
                    current = link_trees(next, current);
                } else {
                    if (prev) {
                        prev->sibling = next;
                    } else {
                        merged = next;
                    }
                    current = link_trees(current, next);
                }
            }
            next = current->sibling;
        }
        
        return merged;
    }
    
    // 找到最小值节点
    NodePtr find_min_node() {
        if (!head) return nullptr;
        
        NodePtr min_node = head;
        NodePtr current = head->sibling;
        
        while (current) {
            if (comp(min_node->value, current->value)) {
                min_node = current;
            }
            current = current->sibling;
        }
        
        return min_node;
    }
    
    // 反转子树链表
    NodePtr reverse_children(NodePtr node) {
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
    
    // 复制子树
    NodePtr copy_tree(NodePtr node) {
        if (!node) return nullptr;
        
        auto new_node = make_shared<BinomialHeapNode<T>>(node->value);
        new_node->degree = node->degree;
        new_node->child = copy_tree(node->child);
        new_node->sibling = copy_tree(node->sibling);
        
        if (new_node->child) {
            new_node->child->parent = new_node;
        }
        
        return new_node;
    }
    
public:
    // 构造函数
    explicit BinomialHeap(const Compare& cmp = Compare()) 
        : head(nullptr), comp(cmp), heap_size(0) {}
    
    // 拷贝构造函数
    BinomialHeap(const BinomialHeap& other) 
        : head(copy_tree(other.head)), comp(other.comp), heap_size(other.heap_size) {}
    
    // 移动构造函数
    BinomialHeap(BinomialHeap&& other) noexcept
        : head(move(other.head)), comp(move(other.comp)), heap_size(other.heap_size) {
        other.heap_size = 0;
    }
    
    // 拷贝赋值运算符
    BinomialHeap& operator=(const BinomialHeap& other) {
        if (this != &other) {
            head = copy_tree(other.head);
            comp = other.comp;
            heap_size = other.heap_size;
        }
        return *this;
    }
    
    // 移动赋值运算符
    BinomialHeap& operator=(BinomialHeap&& other) noexcept {
        if (this != &other) {
            head = move(other.head);
            comp = move(other.comp);
            heap_size = other.heap_size;
            other.heap_size = 0;
        }
        return *this;
    }
    
    // 插入元素
    void insert(const T& value) {
        auto new_node = make_shared<BinomialHeapNode<T>>(value);
        head = union_heaps(head, new_node);
        heap_size++;
    }
    
    // 删除并返回最小值
    T extract() {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        
        NodePtr min_node = find_min_node();
        T result = min_node->value;
        
        // 从根链表中移除最小节点
        if (min_node == head) {
            head = head->sibling;
        } else {
            NodePtr current = head;
            while (current->sibling != min_node) {
                current = current->sibling;
            }
            current->sibling = min_node->sibling;
        }
        
        // 反转最小节点的子树并合并
        NodePtr reversed_children = reverse_children(min_node->child);
        head = union_heaps(head, reversed_children);
        
        heap_size--;
        return result;
    }
    
    // 查看最小值
    const T& top() const {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        
        NodePtr min_node = const_cast<BinomialHeap*>(this)->find_min_node();
        return min_node->value;
    }
    
    // 合并另一个堆
    void merge_with(BinomialHeap& other) {
        head = union_heaps(head, other.head);
        heap_size += other.heap_size;
        
        other.head = nullptr;
        other.heap_size = 0;
    }
    
    // 检查是否为空
    bool empty() const {
        return head == nullptr;
    }
    
    // 获取大小
    size_t size() const {
        return heap_size;
    }
    
    // 清空堆
    void clear() {
        head = nullptr;
        heap_size = 0;
    }
    
    // 获取根的数量
    int root_count() const {
        int count = 0;
        NodePtr current = head;
        while (current) {
            count++;
            current = current->sibling;
        }
        return count;
    }
};

// 类型别名
template<typename T>
using LeftistMaxHeap = LeftistHeap<T, less<T>>;

template<typename T>
using LeftistMinHeap = LeftistHeap<T, greater<T>>;

template<typename T>
using SkewMaxHeap = SkewHeap<T, less<T>>;

template<typename T>
using SkewMinHeap = SkewHeap<T, greater<T>>;

template<typename T>
using BinomialMaxHeap = BinomialHeap<T, less<T>>;

template<typename T>
using BinomialMinHeap = BinomialHeap<T, greater<T>>;

// 实用函数

// 构建左偏堆
template<typename T>
LeftistHeap<T> build_leftist_heap(const vector<T>& arr) {
    LeftistHeap<T> heap;
    for (const T& val : arr) {
        heap.insert(val);
    }
    return heap;
}

// 构建斜堆
template<typename T>
SkewHeap<T> build_skew_heap(const vector<T>& arr) {
    SkewHeap<T> heap;
    for (const T& val : arr) {
        heap.insert(val);
    }
    return heap;
}

// 构建二项堆
template<typename T>
BinomialHeap<T> build_binomial_heap(const vector<T>& arr) {
    BinomialHeap<T> heap;
    for (const T& val : arr) {
        heap.insert(val);
    }
    return heap;
}

// 合并多个堆
template<typename HeapType>
HeapType merge_multiple_heaps(vector<HeapType>& heaps) {
    if (heaps.empty()) {
        return HeapType();
    }
    
    HeapType result = move(heaps[0]);
    for (int i = 1; i < heaps.size(); ++i) {
        result.merge_with(heaps[i]);
    }
    
    return result;
}

// 堆排序（使用可合并堆）
template<typename T, typename HeapType>
vector<T> heap_sort_mergeable(vector<T> arr) {
    HeapType heap;
    for (const T& val : arr) {
        heap.insert(val);
    }
    
    vector<T> result;
    result.reserve(arr.size());
    
    while (!heap.empty()) {
        result.push_back(heap.extract());
    }
    
    return result;
}

#endif // MERGEABLE_HEAP_H