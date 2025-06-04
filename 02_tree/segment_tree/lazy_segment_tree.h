#ifndef LAZY_SEGMENT_TREE_H
#define LAZY_SEGMENT_TREE_H

#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <climits>
#include <stdexcept>

using namespace std;

// 懒惰传播线段树模板类
template<typename T>
class LazySegmentTree {
private:
    vector<T> tree;  // 线段树数组
    vector<T> lazy;  // 懒惰标记数组
    int n;           // 数组大小
    function<T(T, T)> combine;  // 合并函数
    function<T(T, T, int)> apply_lazy;  // 应用懒惰标记的函数
    T default_value; // 默认值
    T lazy_default;  // 懒惰标记默认值
    
    // 向上更新
    void push_up(int node) {
        int left_child = 2 * node;
        int right_child = 2 * node + 1;
        tree[node] = combine(tree[left_child], tree[right_child]);
    }
    
    // 向下传播懒惰标记
    void push_down(int node, int start, int end) {
        if (lazy[node] != lazy_default) {
            int len = end - start + 1;
            tree[node] = apply_lazy(tree[node], lazy[node], len);
            
            if (start != end) {
                int left_child = 2 * node;
                int right_child = 2 * node + 1;
                lazy[left_child] = combine_lazy(lazy[left_child], lazy[node]);
                lazy[right_child] = combine_lazy(lazy[right_child], lazy[node]);
            }
            
            lazy[node] = lazy_default;
        }
    }
    
    // 合并懒惰标记
    T combine_lazy(T old_lazy, T new_lazy) {
        if (old_lazy == lazy_default) return new_lazy;
        if (new_lazy == lazy_default) return old_lazy;
        return new_lazy;  // 默认为覆盖操作
    }
    
    // 构建线段树
    void build(int node, int start, int end, const vector<T>& arr) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            int mid = (start + end) / 2;
            int left_child = 2 * node;
            int right_child = 2 * node + 1;
            
            build(left_child, start, mid, arr);
            build(right_child, mid + 1, end, arr);
            
            push_up(node);
        }
    }
    
    // 区间更新
    void update_range(int node, int start, int end, int l, int r, T val) {
        push_down(node, start, end);
        
        if (start > r || end < l) {
            return;
        }
        
        if (start >= l && end <= r) {
            lazy[node] = combine_lazy(lazy[node], val);
            push_down(node, start, end);
            return;
        }
        
        int mid = (start + end) / 2;
        int left_child = 2 * node;
        int right_child = 2 * node + 1;
        
        update_range(left_child, start, mid, l, r, val);
        update_range(right_child, mid + 1, end, l, r, val);
        
        push_down(left_child, start, mid);
        push_down(right_child, mid + 1, end);
        push_up(node);
    }
    
    // 区间查询
    T query(int node, int start, int end, int l, int r) {
        if (start > r || end < l) {
            return default_value;
        }
        
        push_down(node, start, end);
        
        if (start >= l && end <= r) {
            return tree[node];
        }
        
        int mid = (start + end) / 2;
        int left_child = 2 * node;
        int right_child = 2 * node + 1;
        
        T left_result = query(left_child, start, mid, l, r);
        T right_result = query(right_child, mid + 1, end, l, r);
        
        return combine(left_result, right_result);
    }
    
    // 单点更新
    void update_point(int node, int start, int end, int idx, T val) {
        push_down(node, start, end);
        
        if (start == end) {
            tree[node] = val;
        } else {
            int mid = (start + end) / 2;
            int left_child = 2 * node;
            int right_child = 2 * node + 1;
            
            if (idx <= mid) {
                update_point(left_child, start, mid, idx, val);
            } else {
                update_point(right_child, mid + 1, end, idx, val);
            }
            
            push_down(left_child, start, mid);
            push_down(right_child, mid + 1, end);
            push_up(node);
        }
    }
    
    // 打印树结构
    void print_tree(int node, int start, int end, int depth) {
        if (node >= tree.size()) return;
        
        push_down(node, start, end);
        
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }
        
        cout << "[" << start << ", " << end << "]: " << tree[node];
        if (lazy[node] != lazy_default) {
            cout << " (lazy: " << lazy[node] << ")";
        }
        cout << endl;
        
        if (start != end) {
            int mid = (start + end) / 2;
            print_tree(2 * node, start, mid, depth + 1);
            print_tree(2 * node + 1, mid + 1, end, depth + 1);
        }
    }
    
public:
    // 构造函数 - 从数组构建
    LazySegmentTree(const vector<T>& arr,
                    function<T(T, T)> combine_func = [](T a, T b) { return a + b; },
                    function<T(T, T, int)> apply_lazy_func = [](T val, T lazy_val, int len) { return lazy_val * len; },
                    T default_val = T{},
                    T lazy_default_val = T{}) 
        : n(arr.size()), combine(combine_func), apply_lazy(apply_lazy_func),
          default_value(default_val), lazy_default(lazy_default_val) {
        
        if (n == 0) {
            throw invalid_argument("Array cannot be empty");
        }
        
        tree.resize(4 * n);
        lazy.resize(4 * n, lazy_default);
        build(1, 0, n - 1, arr);
    }
    
    // 构造函数 - 指定大小和初始值
    LazySegmentTree(int size, T initial_value = T{},
                    function<T(T, T)> combine_func = [](T a, T b) { return a + b; },
                    function<T(T, T, int)> apply_lazy_func = [](T val, T lazy_val, int len) { return lazy_val * len; },
                    T default_val = T{},
                    T lazy_default_val = T{}) 
        : n(size), combine(combine_func), apply_lazy(apply_lazy_func),
          default_value(default_val), lazy_default(lazy_default_val) {
        
        if (n <= 0) {
            throw invalid_argument("Size must be positive");
        }
        
        vector<T> arr(n, initial_value);
        tree.resize(4 * n);
        lazy.resize(4 * n, lazy_default);
        build(1, 0, n - 1, arr);
    }
    
    // 拷贝构造函数
    LazySegmentTree(const LazySegmentTree& other)
        : tree(other.tree), lazy(other.lazy), n(other.n),
          combine(other.combine), apply_lazy(other.apply_lazy),
          default_value(other.default_value), lazy_default(other.lazy_default) {}
    
    // 赋值操作符
    LazySegmentTree& operator=(const LazySegmentTree& other) {
        if (this != &other) {
            tree = other.tree;
            lazy = other.lazy;
            n = other.n;
            combine = other.combine;
            apply_lazy = other.apply_lazy;
            default_value = other.default_value;
            lazy_default = other.lazy_default;
        }
        return *this;
    }
    
    // 移动构造函数
    LazySegmentTree(LazySegmentTree&& other) noexcept
        : tree(move(other.tree)), lazy(move(other.lazy)), n(other.n),
          combine(move(other.combine)), apply_lazy(move(other.apply_lazy)),
          default_value(move(other.default_value)), lazy_default(move(other.lazy_default)) {
        other.n = 0;
    }
    
    // 移动赋值操作符
    LazySegmentTree& operator=(LazySegmentTree&& other) noexcept {
        if (this != &other) {
            tree = move(other.tree);
            lazy = move(other.lazy);
            n = other.n;
            combine = move(other.combine);
            apply_lazy = move(other.apply_lazy);
            default_value = move(other.default_value);
            lazy_default = move(other.lazy_default);
            other.n = 0;
        }
        return *this;
    }
    
    // 区间查询 [l, r]
    T query(int l, int r) {
        if (l < 0 || r >= n || l > r) {
            throw out_of_range("Invalid query range");
        }
        return query(1, 0, n - 1, l, r);
    }
    
    // 单点查询
    T query(int idx) {
        if (idx < 0 || idx >= n) {
            throw out_of_range("Index out of range");
        }
        return query(1, 0, n - 1, idx, idx);
    }
    
    // 区间更新 [l, r]
    void update_range(int l, int r, T val) {
        if (l < 0 || r >= n || l > r) {
            throw out_of_range("Invalid update range");
        }
        update_range(1, 0, n - 1, l, r, val);
    }
    
    // 单点更新
    void update(int idx, T val) {
        if (idx < 0 || idx >= n) {
            throw out_of_range("Index out of range");
        }
        update_point(1, 0, n - 1, idx, val);
    }
    
    // 获取数组大小
    int size() const {
        return n;
    }
    
    // 检查是否为空
    bool empty() const {
        return n == 0;
    }
    
    // 打印线段树结构
    void print_structure() {
        cout << "Lazy Segment Tree Structure (size = " << n << "):" << endl;
        print_tree(1, 0, n - 1, 0);
    }
    
    // 清空所有懒惰标记（强制传播）
    void flush_lazy() {
        flush_lazy_recursive(1, 0, n - 1);
    }
    
private:
    void flush_lazy_recursive(int node, int start, int end) {
        push_down(node, start, end);
        
        if (start != end) {
            int mid = (start + end) / 2;
            flush_lazy_recursive(2 * node, start, mid);
            flush_lazy_recursive(2 * node + 1, mid + 1, end);
        }
    }
};

// 特化的懒惰传播线段树类型

// 区间赋值求和线段树
template<typename T>
class RangeAssignSumTree : public LazySegmentTree<T> {
public:
    RangeAssignSumTree(const vector<T>& arr)
        : LazySegmentTree<T>(
            arr,
            [](T a, T b) { return a + b; },  // 合并函数：求和
            [](T val, T lazy_val, int len) { return lazy_val * len; },  // 应用懒惰标记：赋值
            T{},  // 查询默认值
            numeric_limits<T>::min()  // 懒惰标记默认值（表示无标记）
        ) {}
    
    RangeAssignSumTree(int size, T initial_value = T{})
        : LazySegmentTree<T>(
            size, initial_value,
            [](T a, T b) { return a + b; },
            [](T val, T lazy_val, int len) { return lazy_val * len; },
            T{},
            numeric_limits<T>::min()
        ) {}
};

// 区间加法求和线段树
template<typename T>
class RangeAddSumTree : public LazySegmentTree<T> {
public:
    RangeAddSumTree(const vector<T>& arr)
        : LazySegmentTree<T>(
            arr,
            [](T a, T b) { return a + b; },  // 合并函数：求和
            [](T val, T lazy_val, int len) { return val + lazy_val * len; },  // 应用懒惰标记：加法
            T{},  // 查询默认值
            T{}   // 懒惰标记默认值
        ) {}
    
    RangeAddSumTree(int size, T initial_value = T{})
        : LazySegmentTree<T>(
            size, initial_value,
            [](T a, T b) { return a + b; },
            [](T val, T lazy_val, int len) { return val + lazy_val * len; },
            T{},
            T{}
        ) {}
};

// 区间乘法求和线段树
template<typename T>
class RangeMultiplySumTree : public LazySegmentTree<T> {
public:
    RangeMultiplySumTree(const vector<T>& arr)
        : LazySegmentTree<T>(
            arr,
            [](T a, T b) { return a + b; },  // 合并函数：求和
            [](T val, T lazy_val, int len) { return val * lazy_val; },  // 应用懒惰标记：乘法
            T{},  // 查询默认值
            T{1}  // 懒惰标记默认值（乘法单位元）
        ) {}
    
    RangeMultiplySumTree(int size, T initial_value = T{})
        : LazySegmentTree<T>(
            size, initial_value,
            [](T a, T b) { return a + b; },
            [](T val, T lazy_val, int len) { return val * lazy_val; },
            T{},
            T{1}
        ) {}
};

// 区间最大值线段树（支持区间赋值）
template<typename T>
class RangeAssignMaxTree : public LazySegmentTree<T> {
public:
    RangeAssignMaxTree(const vector<T>& arr)
        : LazySegmentTree<T>(
            arr,
            [](T a, T b) { return max(a, b); },  // 合并函数：最大值
            [](T val, T lazy_val, int len) { return lazy_val; },  // 应用懒惰标记：赋值
            numeric_limits<T>::lowest(),  // 查询默认值
            numeric_limits<T>::min()      // 懒惰标记默认值
        ) {}
    
    RangeAssignMaxTree(int size, T initial_value = T{})
        : LazySegmentTree<T>(
            size, initial_value,
            [](T a, T b) { return max(a, b); },
            [](T val, T lazy_val, int len) { return lazy_val; },
            numeric_limits<T>::lowest(),
            numeric_limits<T>::min()
        ) {}
};

// 复合操作线段树（支持区间乘法和加法）
template<typename T>
class RangeMultiplyAddSumTree {
private:
    struct LazyTag {
        T multiply;
        T add;
        
        LazyTag(T m = T{1}, T a = T{}) : multiply(m), add(a) {}
        
        bool is_default() const {
            return multiply == T{1} && add == T{};
        }
        
        void reset() {
            multiply = T{1};
            add = T{};
        }
    };
    
    vector<T> tree;
    vector<LazyTag> lazy;
    int n;
    
    void push_up(int node) {
        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }
    
    void apply_tag(int node, int start, int end, const LazyTag& tag) {
        int len = end - start + 1;
        tree[node] = tree[node] * tag.multiply + tag.add * len;
        
        if (start != end) {
            lazy[node].multiply *= tag.multiply;
            lazy[node].add = lazy[node].add * tag.multiply + tag.add;
        }
    }
    
    void push_down(int node, int start, int end) {
        if (!lazy[node].is_default()) {
            apply_tag(node, start, end, lazy[node]);
            
            if (start != end) {
                int mid = (start + end) / 2;
                apply_tag(2 * node, start, mid, lazy[node]);
                apply_tag(2 * node + 1, mid + 1, end, lazy[node]);
            }
            
            lazy[node].reset();
        }
    }
    
    void build(int node, int start, int end, const vector<T>& arr) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            int mid = (start + end) / 2;
            build(2 * node, start, mid, arr);
            build(2 * node + 1, mid + 1, end, arr);
            push_up(node);
        }
    }
    
    void update_range(int node, int start, int end, int l, int r, const LazyTag& tag) {
        push_down(node, start, end);
        
        if (start > r || end < l) {
            return;
        }
        
        if (start >= l && end <= r) {
            apply_tag(node, start, end, tag);
            return;
        }
        
        int mid = (start + end) / 2;
        update_range(2 * node, start, mid, l, r, tag);
        update_range(2 * node + 1, mid + 1, end, l, r, tag);
        
        push_down(2 * node, start, mid);
        push_down(2 * node + 1, mid + 1, end);
        push_up(node);
    }
    
    T query(int node, int start, int end, int l, int r) {
        if (start > r || end < l) {
            return T{};
        }
        
        push_down(node, start, end);
        
        if (start >= l && end <= r) {
            return tree[node];
        }
        
        int mid = (start + end) / 2;
        return query(2 * node, start, mid, l, r) + 
               query(2 * node + 1, mid + 1, end, l, r);
    }
    
public:
    RangeMultiplyAddSumTree(const vector<T>& arr) : n(arr.size()) {
        tree.resize(4 * n);
        lazy.resize(4 * n);
        build(1, 0, n - 1, arr);
    }
    
    RangeMultiplyAddSumTree(int size, T initial_value = T{}) : n(size) {
        vector<T> arr(n, initial_value);
        tree.resize(4 * n);
        lazy.resize(4 * n);
        build(1, 0, n - 1, arr);
    }
    
    // 区间乘法
    void multiply_range(int l, int r, T val) {
        update_range(1, 0, n - 1, l, r, LazyTag(val, T{}));
    }
    
    // 区间加法
    void add_range(int l, int r, T val) {
        update_range(1, 0, n - 1, l, r, LazyTag(T{1}, val));
    }
    
    // 区间查询
    T query(int l, int r) {
        return query(1, 0, n - 1, l, r);
    }
    
    int size() const {
        return n;
    }
};

// 实用函数

// 构建区间赋值求和线段树
template<typename T>
RangeAssignSumTree<T> build_range_assign_sum_tree(const vector<T>& arr) {
    return RangeAssignSumTree<T>(arr);
}

// 构建区间加法求和线段树
template<typename T>
RangeAddSumTree<T> build_range_add_sum_tree(const vector<T>& arr) {
    return RangeAddSumTree<T>(arr);
}

// 构建区间最大值线段树
template<typename T>
RangeAssignMaxTree<T> build_range_assign_max_tree(const vector<T>& arr) {
    return RangeAssignMaxTree<T>(arr);
}

#endif // LAZY_SEGMENT_TREE_H