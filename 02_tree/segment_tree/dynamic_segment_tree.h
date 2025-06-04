#ifndef DYNAMIC_SEGMENT_TREE_H
#define DYNAMIC_SEGMENT_TREE_H

#include <memory>
#include <functional>
#include <iostream>
#include <algorithm>
#include <climits>
#include <stdexcept>
#include <unordered_map>

using namespace std;

// 动态线段树节点
template<typename T>
struct DynamicSegmentTreeNode {
    T value;
    T lazy;
    shared_ptr<DynamicSegmentTreeNode> left;
    shared_ptr<DynamicSegmentTreeNode> right;
    
    DynamicSegmentTreeNode(T val = T{}, T lazy_val = T{}) 
        : value(val), lazy(lazy_val), left(nullptr), right(nullptr) {}
};

// 动态线段树模板类
template<typename T>
class DynamicSegmentTree {
private:
    using NodePtr = shared_ptr<DynamicSegmentTreeNode<T>>;
    
    NodePtr root;
    long long left_bound, right_bound;
    function<T(T, T)> combine;
    function<T(T, T, long long)> apply_lazy;
    T default_value;
    T lazy_default;
    
    // 创建新节点
    NodePtr create_node(T val = T{}) {
        return make_shared<DynamicSegmentTreeNode<T>>(val, lazy_default);
    }
    
    // 向上更新
    void push_up(NodePtr node, long long start, long long end) {
        if (!node) return;
        
        T left_val = default_value;
        T right_val = default_value;
        
        if (node->left) {
            long long mid = start + (end - start) / 2;
            push_down(node->left, start, mid);
            left_val = node->left->value;
        }
        
        if (node->right) {
            long long mid = start + (end - start) / 2;
            push_down(node->right, mid + 1, end);
            right_val = node->right->value;
        }
        
        node->value = combine(left_val, right_val);
    }
    
    // 向下传播懒惰标记
    void push_down(NodePtr node, long long start, long long end) {
        if (!node || node->lazy == lazy_default) return;
        
        long long len = end - start + 1;
        node->value = apply_lazy(node->value, node->lazy, len);
        
        if (start != end) {
            if (!node->left) node->left = create_node();
            if (!node->right) node->right = create_node();
            
            node->left->lazy = combine_lazy(node->left->lazy, node->lazy);
            node->right->lazy = combine_lazy(node->right->lazy, node->lazy);
        }
        
        node->lazy = lazy_default;
    }
    
    // 合并懒惰标记
    T combine_lazy(T old_lazy, T new_lazy) {
        if (old_lazy == lazy_default) return new_lazy;
        if (new_lazy == lazy_default) return old_lazy;
        return new_lazy;  // 默认为覆盖操作
    }
    
    // 区间更新
    void update_range(NodePtr& node, long long start, long long end, 
                     long long l, long long r, T val) {
        if (l > end || r < start) return;
        
        if (!node) node = create_node();
        
        if (l <= start && end <= r) {
            node->lazy = combine_lazy(node->lazy, val);
            push_down(node, start, end);
            return;
        }
        
        push_down(node, start, end);
        
        long long mid = start + (end - start) / 2;
        update_range(node->left, start, mid, l, r, val);
        update_range(node->right, mid + 1, end, l, r, val);
        
        push_up(node, start, end);
    }
    
    // 区间查询
    T query(NodePtr node, long long start, long long end, long long l, long long r) {
        if (!node || l > end || r < start) {
            return default_value;
        }
        
        push_down(node, start, end);
        
        if (l <= start && end <= r) {
            return node->value;
        }
        
        long long mid = start + (end - start) / 2;
        T left_result = query(node->left, start, mid, l, r);
        T right_result = query(node->right, mid + 1, end, l, r);
        
        return combine(left_result, right_result);
    }
    
    // 单点更新
    void update_point(NodePtr& node, long long start, long long end, long long idx, T val) {
        if (!node) node = create_node();
        
        push_down(node, start, end);
        
        if (start == end) {
            node->value = val;
            return;
        }
        
        long long mid = start + (end - start) / 2;
        if (idx <= mid) {
            update_point(node->left, start, mid, idx, val);
        } else {
            update_point(node->right, mid + 1, end, idx, val);
        }
        
        push_up(node, start, end);
    }
    
    // 计算节点数量
    int count_nodes(NodePtr node) const {
        if (!node) return 0;
        return 1 + count_nodes(node->left) + count_nodes(node->right);
    }
    
    // 打印树结构
    void print_tree(NodePtr node, long long start, long long end, int depth) {
        if (!node) return;
        
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }
        
        cout << "[" << start << ", " << end << "]: " << node->value;
        if (node->lazy != lazy_default) {
            cout << " (lazy: " << node->lazy << ")";
        }
        cout << endl;
        
        if (start != end) {
            long long mid = start + (end - start) / 2;
            if (node->left) {
                print_tree(node->left, start, mid, depth + 1);
            }
            if (node->right) {
                print_tree(node->right, mid + 1, end, depth + 1);
            }
        }
    }
    
    // 复制子树
    NodePtr copy_subtree(NodePtr node) {
        if (!node) return nullptr;
        
        auto new_node = create_node(node->value);
        new_node->lazy = node->lazy;
        new_node->left = copy_subtree(node->left);
        new_node->right = copy_subtree(node->right);
        
        return new_node;
    }
    
public:
    // 构造函数
    DynamicSegmentTree(long long left = 0, long long right = 1e9,
                      function<T(T, T)> combine_func = [](T a, T b) { return a + b; },
                      function<T(T, T, long long)> apply_lazy_func = [](T val, T lazy_val, long long len) { return lazy_val * len; },
                      T default_val = T{},
                      T lazy_default_val = T{})
        : root(nullptr), left_bound(left), right_bound(right),
          combine(combine_func), apply_lazy(apply_lazy_func),
          default_value(default_val), lazy_default(lazy_default_val) {
        
        if (left > right) {
            throw invalid_argument("Invalid range: left > right");
        }
    }
    
    // 拷贝构造函数
    DynamicSegmentTree(const DynamicSegmentTree& other)
        : left_bound(other.left_bound), right_bound(other.right_bound),
          combine(other.combine), apply_lazy(other.apply_lazy),
          default_value(other.default_value), lazy_default(other.lazy_default) {
        root = copy_subtree(other.root);
    }
    
    // 赋值操作符
    DynamicSegmentTree& operator=(const DynamicSegmentTree& other) {
        if (this != &other) {
            left_bound = other.left_bound;
            right_bound = other.right_bound;
            combine = other.combine;
            apply_lazy = other.apply_lazy;
            default_value = other.default_value;
            lazy_default = other.lazy_default;
            root = copy_subtree(other.root);
        }
        return *this;
    }
    
    // 移动构造函数
    DynamicSegmentTree(DynamicSegmentTree&& other) noexcept
        : root(move(other.root)), left_bound(other.left_bound), right_bound(other.right_bound),
          combine(move(other.combine)), apply_lazy(move(other.apply_lazy)),
          default_value(move(other.default_value)), lazy_default(move(other.lazy_default)) {
        other.root = nullptr;
    }
    
    // 移动赋值操作符
    DynamicSegmentTree& operator=(DynamicSegmentTree&& other) noexcept {
        if (this != &other) {
            root = move(other.root);
            left_bound = other.left_bound;
            right_bound = other.right_bound;
            combine = move(other.combine);
            apply_lazy = move(other.apply_lazy);
            default_value = move(other.default_value);
            lazy_default = move(other.lazy_default);
            other.root = nullptr;
        }
        return *this;
    }
    
    // 区间查询 [l, r]
    T query(long long l, long long r) {
        if (l < left_bound || r > right_bound || l > r) {
            throw out_of_range("Invalid query range");
        }
        return query(root, left_bound, right_bound, l, r);
    }
    
    // 单点查询
    T query(long long idx) {
        if (idx < left_bound || idx > right_bound) {
            throw out_of_range("Index out of range");
        }
        return query(root, left_bound, right_bound, idx, idx);
    }
    
    // 区间更新 [l, r]
    void update_range(long long l, long long r, T val) {
        if (l < left_bound || r > right_bound || l > r) {
            throw out_of_range("Invalid update range");
        }
        update_range(root, left_bound, right_bound, l, r, val);
    }
    
    // 单点更新
    void update(long long idx, T val) {
        if (idx < left_bound || idx > right_bound) {
            throw out_of_range("Index out of range");
        }
        update_point(root, left_bound, right_bound, idx, val);
    }
    
    // 获取范围
    pair<long long, long long> get_range() const {
        return {left_bound, right_bound};
    }
    
    // 检查是否为空
    bool empty() const {
        return root == nullptr;
    }
    
    // 获取节点数量
    int node_count() const {
        return count_nodes(root);
    }
    
    // 清空树
    void clear() {
        root = nullptr;
    }
    
    // 打印树结构
    void print_structure() {
        cout << "Dynamic Segment Tree Structure [" << left_bound << ", " << right_bound << "]:" << endl;
        if (root) {
            print_tree(root, left_bound, right_bound, 0);
        } else {
            cout << "  (empty)" << endl;
        }
    }
    
    // 扩展范围
    void extend_range(long long new_left, long long new_right) {
        if (new_left > left_bound || new_right < right_bound) {
            throw invalid_argument("Cannot shrink the range");
        }
        left_bound = new_left;
        right_bound = new_right;
    }
    
    // 内存使用估算
    size_t memory_usage() const {
        return node_count() * sizeof(DynamicSegmentTreeNode<T>);
    }
};

// 特化的动态线段树类型

// 动态区间和线段树
template<typename T>
class DynamicSumSegmentTree : public DynamicSegmentTree<T> {
public:
    DynamicSumSegmentTree(long long left = 0, long long right = 1e9)
        : DynamicSegmentTree<T>(
            left, right,
            [](T a, T b) { return a + b; },  // 合并函数：求和
            [](T val, T lazy_val, long long len) { return lazy_val * len; },  // 应用懒惰标记：赋值
            T{},  // 查询默认值
            numeric_limits<T>::min()  // 懒惰标记默认值
        ) {}
};

// 动态区间最大值线段树
template<typename T>
class DynamicMaxSegmentTree : public DynamicSegmentTree<T> {
public:
    DynamicMaxSegmentTree(long long left = 0, long long right = 1e9)
        : DynamicSegmentTree<T>(
            left, right,
            [](T a, T b) { return max(a, b); },  // 合并函数：最大值
            [](T val, T lazy_val, long long len) { return lazy_val; },  // 应用懒惰标记：赋值
            numeric_limits<T>::lowest(),  // 查询默认值
            numeric_limits<T>::min()      // 懒惰标记默认值
        ) {}
};

// 动态区间最小值线段树
template<typename T>
class DynamicMinSegmentTree : public DynamicSegmentTree<T> {
public:
    DynamicMinSegmentTree(long long left = 0, long long right = 1e9)
        : DynamicSegmentTree<T>(
            left, right,
            [](T a, T b) { return min(a, b); },  // 合并函数：最小值
            [](T val, T lazy_val, long long len) { return lazy_val; },  // 应用懒惰标记：赋值
            numeric_limits<T>::max(),     // 查询默认值
            numeric_limits<T>::min()      // 懒惰标记默认值
        ) {}
};

// 坐标压缩动态线段树
template<typename T>
class CompressedDynamicSegmentTree {
private:
    vector<long long> coords;  // 坐标数组
    DynamicSegmentTree<T> tree;
    
    // 坐标压缩
    void compress_coordinates(vector<long long>& points) {
        sort(points.begin(), points.end());
        points.erase(unique(points.begin(), points.end()), points.end());
        coords = points;
    }
    
    // 获取压缩后的坐标
    int get_compressed_coord(long long x) {
        return lower_bound(coords.begin(), coords.end(), x) - coords.begin();
    }
    
public:
    // 构造函数
    CompressedDynamicSegmentTree(vector<long long> points,
                                function<T(T, T)> combine_func = [](T a, T b) { return a + b; },
                                function<T(T, T, long long)> apply_lazy_func = [](T val, T lazy_val, long long len) { return lazy_val * len; },
                                T default_val = T{},
                                T lazy_default_val = T{})
        : tree(0, 0, combine_func, apply_lazy_func, default_val, lazy_default_val) {
        
        compress_coordinates(points);
        if (!coords.empty()) {
            tree = DynamicSegmentTree<T>(0, coords.size() - 1, combine_func, apply_lazy_func, default_val, lazy_default_val);
        }
    }
    
    // 区间查询
    T query(long long l, long long r) {
        int left_idx = get_compressed_coord(l);
        int right_idx = get_compressed_coord(r);
        
        if (left_idx >= coords.size() || right_idx >= coords.size() ||
            coords[left_idx] != l || coords[right_idx] != r) {
            throw invalid_argument("Coordinates not found in compressed array");
        }
        
        return tree.query(left_idx, right_idx);
    }
    
    // 区间更新
    void update_range(long long l, long long r, T val) {
        int left_idx = get_compressed_coord(l);
        int right_idx = get_compressed_coord(r);
        
        if (left_idx >= coords.size() || right_idx >= coords.size() ||
            coords[left_idx] != l || coords[right_idx] != r) {
            throw invalid_argument("Coordinates not found in compressed array");
        }
        
        tree.update_range(left_idx, right_idx, val);
    }
    
    // 单点更新
    void update(long long idx, T val) {
        int compressed_idx = get_compressed_coord(idx);
        
        if (compressed_idx >= coords.size() || coords[compressed_idx] != idx) {
            throw invalid_argument("Coordinate not found in compressed array");
        }
        
        tree.update(compressed_idx, val);
    }
    
    // 获取原始坐标
    const vector<long long>& get_coordinates() const {
        return coords;
    }
    
    // 打印结构
    void print_structure() {
        cout << "Compressed Dynamic Segment Tree:" << endl;
        cout << "Coordinates: ";
        for (long long coord : coords) {
            cout << coord << " ";
        }
        cout << endl;
        tree.print_structure();
    }
};

// 实用函数

// 构建动态求和线段树
template<typename T>
DynamicSumSegmentTree<T> build_dynamic_sum_tree(long long left = 0, long long right = 1e9) {
    return DynamicSumSegmentTree<T>(left, right);
}

// 构建动态最大值线段树
template<typename T>
DynamicMaxSegmentTree<T> build_dynamic_max_tree(long long left = 0, long long right = 1e9) {
    return DynamicMaxSegmentTree<T>(left, right);
}

// 构建动态最小值线段树
template<typename T>
DynamicMinSegmentTree<T> build_dynamic_min_tree(long long left = 0, long long right = 1e9) {
    return DynamicMinSegmentTree<T>(left, right);
}

// 区间合并操作
template<typename T>
class RangeMergeTree {
private:
    struct Interval {
        long long left, right;
        T value;
        
        Interval(long long l = 0, long long r = 0, T v = T{}) : left(l), right(r), value(v) {}
        
        bool operator<(const Interval& other) const {
            return left < other.left;
        }
    };
    
    vector<Interval> intervals;
    
public:
    // 添加区间
    void add_interval(long long left, long long right, T value) {
        intervals.emplace_back(left, right, value);
    }
    
    // 合并重叠区间
    void merge_intervals() {
        if (intervals.empty()) return;
        
        sort(intervals.begin(), intervals.end());
        
        vector<Interval> merged;
        merged.push_back(intervals[0]);
        
        for (size_t i = 1; i < intervals.size(); ++i) {
            if (intervals[i].left <= merged.back().right) {
                // 重叠，合并
                merged.back().right = max(merged.back().right, intervals[i].right);
                merged.back().value = max(merged.back().value, intervals[i].value);  // 或其他合并策略
            } else {
                // 不重叠，添加新区间
                merged.push_back(intervals[i]);
            }
        }
        
        intervals = merged;
    }
    
    // 查询点是否在某个区间内
    pair<bool, T> query_point(long long point) {
        for (const auto& interval : intervals) {
            if (point >= interval.left && point <= interval.right) {
                return {true, interval.value};
            }
        }
        return {false, T{}};
    }
    
    // 获取所有区间
    const vector<Interval>& get_intervals() const {
        return intervals;
    }
    
    // 打印区间
    void print_intervals() {
        cout << "Intervals:" << endl;
        for (const auto& interval : intervals) {
            cout << "  [" << interval.left << ", " << interval.right << "]: " << interval.value << endl;
        }
    }
};

#endif // DYNAMIC_SEGMENT_TREE_H