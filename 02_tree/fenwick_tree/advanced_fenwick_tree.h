#ifndef ADVANCED_FENWICK_TREE_H
#define ADVANCED_FENWICK_TREE_H

#include "fenwick_tree.h"
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <stack>
#include <chrono>
#include <mutex>
#include <thread>

using namespace std;
using namespace chrono;

// 可撤销树状数组
template<typename T>
class UndoableFenwickTree {
private:
    struct Operation {
        int index;
        T old_value;
        T new_value;
        
        Operation(int idx, T old_val, T new_val) 
            : index(idx), old_value(old_val), new_value(new_val) {}
    };
    
    FenwickTree<T> tree;
    stack<Operation> history;
    
public:
    // 构造函数
    explicit UndoableFenwickTree(int size) : tree(size) {}
    
    explicit UndoableFenwickTree(const vector<T>& arr) : tree(arr) {}
    
    // 单点更新（可撤销）
    void update(int i, T delta) {
        T old_value = tree.query(i);
        tree.update(i, delta);
        T new_value = tree.query(i);
        
        history.emplace(i, old_value, new_value);
    }
    
    // 单点设置（可撤销）
    void set(int i, T val) {
        T old_value = tree.query(i);
        tree.set(i, val);
        
        history.emplace(i, old_value, val);
    }
    
    // 撤销最后一次操作
    bool undo() {
        if (history.empty()) {
            return false;
        }
        
        Operation op = history.top();
        history.pop();
        
        tree.set(op.index, op.old_value);
        return true;
    }
    
    // 撤销多次操作
    int undo(int count) {
        int undone = 0;
        for (int i = 0; i < count && !history.empty(); ++i) {
            if (undo()) {
                undone++;
            }
        }
        return undone;
    }
    
    // 清空历史记录
    void clear_history() {
        while (!history.empty()) {
            history.pop();
        }
    }
    
    // 获取历史记录大小
    size_t history_size() const {
        return history.size();
    }
    
    // 查询操作（不记录历史）
    T prefix_sum(int i) {
        return tree.prefix_sum(i);
    }
    
    T range_sum(int l, int r) {
        return tree.range_sum(l, r);
    }
    
    T query(int i) {
        return tree.query(i);
    }
    
    // 获取数组大小
    int size() const {
        return tree.size();
    }
    
    // 获取原数组表示
    vector<T> get_array() {
        return tree.get_array();
    }
    
    // 创建检查点
    void create_checkpoint() {
        clear_history();
    }
};

// 可持久化树状数组
template<typename T>
class PersistentFenwickTree {
private:
    struct Node {
        T value;
        shared_ptr<Node> left;
        shared_ptr<Node> right;
        
        Node(T val = T{}) : value(val), left(nullptr), right(nullptr) {}
    };
    
    using NodePtr = shared_ptr<Node>;
    
    vector<NodePtr> versions;  // 存储所有版本的根节点
    int n;  // 数组大小
    
    // 创建新节点
    NodePtr create_node(T val = T{}) {
        return make_shared<Node>(val);
    }
    
    // 更新操作，返回新版本的根节点
    NodePtr update(NodePtr node, int pos, int l, int r, T delta) {
        if (!node) node = create_node();
        
        auto new_node = create_node(node->value);
        new_node->left = node->left;
        new_node->right = node->right;
        
        if (l == r) {
            new_node->value += delta;
            return new_node;
        }
        
        int mid = l + (r - l) / 2;
        if (pos <= mid) {
            new_node->left = update(node->left, pos, l, mid, delta);
        } else {
            new_node->right = update(node->right, pos, mid + 1, r, delta);
        }
        
        T left_val = new_node->left ? new_node->left->value : T{};
        T right_val = new_node->right ? new_node->right->value : T{};
        new_node->value = left_val + right_val;
        
        return new_node;
    }
    
    // 查询操作
    T query(NodePtr node, int ql, int qr, int l, int r) {
        if (!node || ql > r || qr < l) {
            return T{};
        }
        
        if (ql <= l && r <= qr) {
            return node->value;
        }
        
        int mid = l + (r - l) / 2;
        T left_result = query(node->left, ql, qr, l, mid);
        T right_result = query(node->right, ql, qr, mid + 1, r);
        
        return left_result + right_result;
    }
    
public:
    // 构造函数
    explicit PersistentFenwickTree(int size) : n(size) {
        if (size <= 0) {
            throw invalid_argument("Size must be positive");
        }
        
        versions.push_back(nullptr);  // 初始空版本
    }
    
    explicit PersistentFenwickTree(const vector<T>& arr) : n(arr.size()) {
        if (arr.empty()) {
            throw invalid_argument("Array cannot be empty");
        }
        
        versions.push_back(nullptr);
        
        // 逐个添加元素
        int current_version = 0;
        for (int i = 0; i < n; ++i) {
            current_version = update(current_version, i + 1, arr[i]);
        }
    }
    
    // 单点更新，返回新版本号
    int update(int version, int i, T delta) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        if (i < 1 || i > n) {
            throw out_of_range("Index out of range");
        }
        
        NodePtr new_root = update(versions[version], i, 1, n, delta);
        versions.push_back(new_root);
        return versions.size() - 1;
    }
    
    // 区间查询
    T range_sum(int version, int l, int r) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        if (l < 1 || r > n || l > r) {
            throw out_of_range("Invalid range");
        }
        
        return query(versions[version], l, r, 1, n);
    }
    
    // 前缀和查询
    T prefix_sum(int version, int i) {
        if (i < 1) return T{};
        if (i > n) i = n;
        
        return range_sum(version, 1, i);
    }
    
    // 单点查询
    T query(int version, int i) {
        return range_sum(version, i, i);
    }
    
    // 获取版本数量
    int version_count() const {
        return versions.size();
    }
    
    // 获取数组大小
    int size() const {
        return n;
    }
    
    // 创建分支（复制版本）
    int create_branch(int version) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        
        versions.push_back(versions[version]);
        return versions.size() - 1;
    }
};

// 线程安全的树状数组
template<typename T>
class ThreadSafeFenwickTree {
private:
    FenwickTree<T> tree;
    mutable mutex tree_mutex;
    
public:
    // 构造函数
    explicit ThreadSafeFenwickTree(int size) : tree(size) {}
    
    explicit ThreadSafeFenwickTree(const vector<T>& arr) : tree(arr) {}
    
    // 线程安全的单点更新
    void update(int i, T delta) {
        lock_guard<mutex> lock(tree_mutex);
        tree.update(i, delta);
    }
    
    // 线程安全的查询
    T prefix_sum(int i) const {
        lock_guard<mutex> lock(tree_mutex);
        return tree.prefix_sum(i);
    }
    
    T range_sum(int l, int r) const {
        lock_guard<mutex> lock(tree_mutex);
        return tree.range_sum(l, r);
    }
    
    T query(int i) const {
        lock_guard<mutex> lock(tree_mutex);
        return tree.query(i);
    }
    
    // 批量操作（减少锁的开销）
    void batch_update(const vector<pair<int, T>>& updates) {
        lock_guard<mutex> lock(tree_mutex);
        for (const auto& [index, delta] : updates) {
            tree.update(index, delta);
        }
    }
    
    vector<T> batch_query(const vector<pair<int, int>>& ranges) const {
        lock_guard<mutex> lock(tree_mutex);
        vector<T> results;
        results.reserve(ranges.size());
        
        for (const auto& [l, r] : ranges) {
            results.push_back(tree.range_sum(l, r));
        }
        
        return results;
    }
    
    // 获取数组大小
    int size() const {
        lock_guard<mutex> lock(tree_mutex);
        return tree.size();
    }
    
    // 获取原数组表示
    vector<T> get_array() const {
        lock_guard<mutex> lock(tree_mutex);
        return tree.get_array();
    }
};

// 压缩树状数组（用于稀疏数据）
template<typename T>
class CompressedFenwickTree {
private:
    vector<int> coords;  // 压缩后的坐标
    FenwickTree<T> tree;
    
    // 坐标压缩
    void compress_coordinates(vector<int>& indices) {
        sort(indices.begin(), indices.end());
        indices.erase(unique(indices.begin(), indices.end()), indices.end());
        coords = indices;
    }
    
    // 获取压缩后的索引
    int get_compressed_index(int original_index) {
        auto it = lower_bound(coords.begin(), coords.end(), original_index);
        if (it == coords.end() || *it != original_index) {
            throw invalid_argument("Index not found in compressed coordinates");
        }
        return it - coords.begin() + 1;
    }
    
public:
    // 构造函数
    explicit CompressedFenwickTree(vector<int> indices) : tree(0) {
        if (indices.empty()) {
            throw invalid_argument("Indices cannot be empty");
        }
        
        compress_coordinates(indices);
        tree = FenwickTree<T>(coords.size());
    }
    
    // 单点更新
    void update(int original_index, T delta) {
        int compressed_index = get_compressed_index(original_index);
        tree.update(compressed_index, delta);
    }
    
    // 前缀和查询
    T prefix_sum(int original_index) {
        auto it = upper_bound(coords.begin(), coords.end(), original_index);
        if (it == coords.begin()) {
            return T{};
        }
        
        int compressed_index = it - coords.begin();
        return tree.prefix_sum(compressed_index);
    }
    
    // 区间和查询
    T range_sum(int l, int r) {
        if (l > r) return T{};
        return prefix_sum(r) - prefix_sum(l - 1);
    }
    
    // 单点查询
    T query(int original_index) {
        try {
            int compressed_index = get_compressed_index(original_index);
            return tree.query(compressed_index);
        } catch (const invalid_argument&) {
            return T{};  // 未压缩的索引返回默认值
        }
    }
    
    // 获取压缩后的坐标
    const vector<int>& get_coordinates() const {
        return coords;
    }
    
    // 获取压缩后的大小
    int compressed_size() const {
        return coords.size();
    }
    
    // 获取原始范围
    pair<int, int> get_original_range() const {
        if (coords.empty()) {
            return {0, 0};
        }
        return {coords.front(), coords.back()};
    }
};

// 动态树状数组（支持动态扩容）
template<typename T>
class DynamicFenwickTree {
private:
    vector<T> tree;
    int n;
    int capacity;
    
    int lowbit(int x) {
        return x & (-x);
    }
    
    // 扩容
    void resize(int new_capacity) {
        if (new_capacity <= capacity) return;
        
        tree.resize(new_capacity + 1, T{});
        capacity = new_capacity;
    }
    
public:
    // 构造函数
    explicit DynamicFenwickTree(int initial_capacity = 1000) 
        : n(0), capacity(initial_capacity), tree(initial_capacity + 1, T{}) {}
    
    // 动态添加元素
    void push_back(T val) {
        n++;
        if (n > capacity) {
            resize(capacity * 2);
        }
        
        update(n, val);
    }
    
    // 单点更新
    void update(int i, T delta) {
        if (i < 1) {
            throw out_of_range("Index must be positive");
        }
        
        if (i > capacity) {
            resize(max(i, capacity * 2));
        }
        
        if (i > n) {
            n = i;
        }
        
        for (; i <= capacity; i += lowbit(i)) {
            tree[i] += delta;
        }
    }
    
    // 前缀和查询
    T prefix_sum(int i) {
        if (i < 0) return T{};
        if (i > n) i = n;
        
        T sum = T{};
        for (; i > 0; i -= lowbit(i)) {
            sum += tree[i];
        }
        return sum;
    }
    
    // 区间和查询
    T range_sum(int l, int r) {
        if (l > r) return T{};
        if (l < 1) l = 1;
        if (r > n) r = n;
        
        return prefix_sum(r) - prefix_sum(l - 1);
    }
    
    // 单点查询
    T query(int i) {
        if (i < 1 || i > n) {
            return T{};
        }
        
        return range_sum(i, i);
    }
    
    // 获取当前大小
    int size() const {
        return n;
    }
    
    // 获取容量
    int get_capacity() const {
        return capacity;
    }
    
    // 收缩到合适大小
    void shrink_to_fit() {
        if (n < capacity / 4 && capacity > 1000) {
            int new_capacity = max(n * 2, 1000);
            vector<T> new_tree(new_capacity + 1, T{});
            
            for (int i = 1; i <= min(n, capacity); ++i) {
                new_tree[i] = tree[i];
            }
            
            tree = move(new_tree);
            capacity = new_capacity;
        }
    }
};

// 带权重的树状数组
template<typename T, typename W>
class WeightedFenwickTree {
private:
    FenwickTree<T> value_tree;   // 值的树状数组
    FenwickTree<W> weight_tree;  // 权重的树状数组
    
public:
    // 构造函数
    explicit WeightedFenwickTree(int size) : value_tree(size), weight_tree(size) {}
    
    // 添加带权重的值
    void update(int i, T value, W weight) {
        value_tree.update(i, value * weight);
        weight_tree.update(i, weight);
    }
    
    // 查询加权平均值
    double weighted_average(int l, int r) {
        T total_weighted_value = value_tree.range_sum(l, r);
        W total_weight = weight_tree.range_sum(l, r);
        
        if (total_weight == W{}) {
            return 0.0;
        }
        
        return static_cast<double>(total_weighted_value) / static_cast<double>(total_weight);
    }
    
    // 查询总权重
    W total_weight(int l, int r) {
        return weight_tree.range_sum(l, r);
    }
    
    // 查询加权总和
    T weighted_sum(int l, int r) {
        return value_tree.range_sum(l, r);
    }
    
    // 获取数组大小
    int size() const {
        return value_tree.size();
    }
};

// 性能监控的树状数组
template<typename T>
class MonitoredFenwickTree {
private:
    FenwickTree<T> tree;
    mutable FenwickTreePerformance perf;
    
public:
    // 构造函数
    explicit MonitoredFenwickTree(int size) : tree(size) {}
    
    explicit MonitoredFenwickTree(const vector<T>& arr) : tree(arr) {}
    
    // 监控的单点更新
    void update(int i, T delta) {
        auto start = high_resolution_clock::now();
        tree.update(i, delta);
        auto end = high_resolution_clock::now();
        
        perf.update_count++;
        perf.total_update_time += duration_cast<microseconds>(end - start).count() / 1000.0;
    }
    
    // 监控的查询
    T prefix_sum(int i) const {
        auto start = high_resolution_clock::now();
        T result = tree.prefix_sum(i);
        auto end = high_resolution_clock::now();
        
        perf.query_count++;
        perf.total_query_time += duration_cast<microseconds>(end - start).count() / 1000.0;
        
        return result;
    }
    
    T range_sum(int l, int r) const {
        auto start = high_resolution_clock::now();
        T result = tree.range_sum(l, r);
        auto end = high_resolution_clock::now();
        
        perf.query_count++;
        perf.total_query_time += duration_cast<microseconds>(end - start).count() / 1000.0;
        
        return result;
    }
    
    T query(int i) const {
        return range_sum(i, i);
    }
    
    // 获取性能统计
    const FenwickTreePerformance& get_performance() const {
        return perf;
    }
    
    // 重置性能统计
    void reset_performance() {
        perf = FenwickTreePerformance{};
    }
    
    // 打印性能统计
    void print_performance() const {
        perf.print_stats();
    }
    
    // 获取数组大小
    int size() const {
        return tree.size();
    }
    
    // 获取原数组表示
    vector<T> get_array() {
        return tree.get_array();
    }
};

// 实用函数

// 构建可撤销树状数组
template<typename T>
UndoableFenwickTree<T> build_undoable_fenwick_tree(const vector<T>& arr) {
    return UndoableFenwickTree<T>(arr);
}

// 构建可持久化树状数组
template<typename T>
PersistentFenwickTree<T> build_persistent_fenwick_tree(const vector<T>& arr) {
    return PersistentFenwickTree<T>(arr);
}

// 构建线程安全树状数组
template<typename T>
ThreadSafeFenwickTree<T> build_thread_safe_fenwick_tree(const vector<T>& arr) {
    return ThreadSafeFenwickTree<T>(arr);
}

// 构建压缩树状数组
template<typename T>
CompressedFenwickTree<T> build_compressed_fenwick_tree(const vector<int>& indices) {
    return CompressedFenwickTree<T>(indices);
}

#endif // ADVANCED_FENWICK_TREE_H