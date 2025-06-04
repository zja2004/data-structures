#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H

#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <climits>
#include <stdexcept>

using namespace std;

// 线段树模板类
template<typename T>
class SegmentTree {
private:
    vector<T> tree;  // 线段树数组
    vector<T> arr;   // 原始数组
    int n;           // 数组大小
    function<T(T, T)> combine;  // 合并函数
    T default_value; // 默认值
    
    // 构建线段树
    void build(int node, int start, int end) {
        if (start == end) {
            // 叶子节点
            tree[node] = arr[start];
        } else {
            int mid = (start + end) / 2;
            int left_child = 2 * node;
            int right_child = 2 * node + 1;
            
            build(left_child, start, mid);
            build(right_child, mid + 1, end);
            
            // 合并子节点的值
            tree[node] = combine(tree[left_child], tree[right_child]);
        }
    }
    
    // 区间查询
    T query(int node, int start, int end, int l, int r) {
        if (r < start || end < l) {
            // 完全不重叠
            return default_value;
        }
        
        if (l <= start && end <= r) {
            // 完全包含
            return tree[node];
        }
        
        // 部分重叠
        int mid = (start + end) / 2;
        int left_child = 2 * node;
        int right_child = 2 * node + 1;
        
        T left_result = query(left_child, start, mid, l, r);
        T right_result = query(right_child, mid + 1, end, l, r);
        
        return combine(left_result, right_result);
    }
    
    // 单点更新
    void update(int node, int start, int end, int idx, T val) {
        if (start == end) {
            // 叶子节点
            arr[idx] = val;
            tree[node] = val;
        } else {
            int mid = (start + end) / 2;
            int left_child = 2 * node;
            int right_child = 2 * node + 1;
            
            if (idx <= mid) {
                update(left_child, start, mid, idx, val);
            } else {
                update(right_child, mid + 1, end, idx, val);
            }
            
            // 更新当前节点
            tree[node] = combine(tree[left_child], tree[right_child]);
        }
    }
    
    // 查找第一个满足条件的位置
    int find_first(int node, int start, int end, int l, int r, function<bool(T)> predicate) {
        if (r < start || end < l || !predicate(tree[node])) {
            return -1;
        }
        
        if (start == end) {
            return start;
        }
        
        int mid = (start + end) / 2;
        int left_child = 2 * node;
        int right_child = 2 * node + 1;
        
        // 先查找左子树
        int left_result = find_first(left_child, start, mid, l, r, predicate);
        if (left_result != -1) {
            return left_result;
        }
        
        // 再查找右子树
        return find_first(right_child, mid + 1, end, l, r, predicate);
    }
    
    // 打印树结构
    void print_tree(int node, int start, int end, int depth) const {
        if (node >= tree.size()) return;
        
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }
        
        cout << "[" << start << ", " << end << "]: " << tree[node] << endl;
        
        if (start != end) {
            int mid = (start + end) / 2;
            print_tree(2 * node, start, mid, depth + 1);
            print_tree(2 * node + 1, mid + 1, end, depth + 1);
        }
    }
    
public:
    // 构造函数 - 从数组构建
    SegmentTree(const vector<T>& input_arr, 
                function<T(T, T)> combine_func = [](T a, T b) { return a + b; },
                T default_val = T{}) 
        : arr(input_arr), n(input_arr.size()), combine(combine_func), default_value(default_val) {
        
        if (n == 0) {
            throw invalid_argument("Array cannot be empty");
        }
        
        tree.resize(4 * n);
        build(1, 0, n - 1);
    }
    
    // 构造函数 - 指定大小和初始值
    SegmentTree(int size, T initial_value = T{},
                function<T(T, T)> combine_func = [](T a, T b) { return a + b; },
                T default_val = T{}) 
        : n(size), combine(combine_func), default_value(default_val) {
        
        if (n <= 0) {
            throw invalid_argument("Size must be positive");
        }
        
        arr.assign(n, initial_value);
        tree.resize(4 * n);
        build(1, 0, n - 1);
    }
    
    // 拷贝构造函数
    SegmentTree(const SegmentTree& other) 
        : tree(other.tree), arr(other.arr), n(other.n), 
          combine(other.combine), default_value(other.default_value) {}
    
    // 赋值操作符
    SegmentTree& operator=(const SegmentTree& other) {
        if (this != &other) {
            tree = other.tree;
            arr = other.arr;
            n = other.n;
            combine = other.combine;
            default_value = other.default_value;
        }
        return *this;
    }
    
    // 移动构造函数
    SegmentTree(SegmentTree&& other) noexcept
        : tree(move(other.tree)), arr(move(other.arr)), n(other.n),
          combine(move(other.combine)), default_value(move(other.default_value)) {
        other.n = 0;
    }
    
    // 移动赋值操作符
    SegmentTree& operator=(SegmentTree&& other) noexcept {
        if (this != &other) {
            tree = move(other.tree);
            arr = move(other.arr);
            n = other.n;
            combine = move(other.combine);
            default_value = move(other.default_value);
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
        return arr[idx];
    }
    
    // 单点更新
    void update(int idx, T val) {
        if (idx < 0 || idx >= n) {
            throw out_of_range("Index out of range");
        }
        update(1, 0, n - 1, idx, val);
    }
    
    // 单点增加
    void add(int idx, T val) {
        if (idx < 0 || idx >= n) {
            throw out_of_range("Index out of range");
        }
        T current = arr[idx];
        update(1, 0, n - 1, idx, current + val);
    }
    
    // 查找第一个满足条件的位置
    int find_first(int l, int r, function<bool(T)> predicate) {
        if (l < 0 || r >= n || l > r) {
            throw out_of_range("Invalid range");
        }
        return find_first(1, 0, n - 1, l, r, predicate);
    }
    
    // 查找第一个大于等于val的位置
    int find_first_ge(int l, int r, T val) {
        return find_first(l, r, [val](T x) { return x >= val; });
    }
    
    // 查找第一个大于val的位置
    int find_first_gt(int l, int r, T val) {
        return find_first(l, r, [val](T x) { return x > val; });
    }
    
    // 获取数组大小
    int size() const {
        return n;
    }
    
    // 检查是否为空
    bool empty() const {
        return n == 0;
    }
    
    // 获取原始数组
    const vector<T>& get_array() const {
        return arr;
    }
    
    // 重建线段树（当需要更改合并函数时）
    void rebuild(function<T(T, T)> new_combine_func, T new_default_value = T{}) {
        combine = new_combine_func;
        default_value = new_default_value;
        build(1, 0, n - 1);
    }
    
    // 批量更新
    void batch_update(const vector<pair<int, T>>& updates) {
        for (const auto& update_pair : updates) {
            update(update_pair.first, update_pair.second);
        }
    }
    
    // 打印线段树结构
    void print_structure() const {
        cout << "Segment Tree Structure (size = " << n << "):" << endl;
        print_tree(1, 0, n - 1, 0);
    }
    
    // 打印原始数组
    void print_array() const {
        cout << "Array: [";
        for (int i = 0; i < n; ++i) {
            cout << arr[i];
            if (i < n - 1) cout << ", ";
        }
        cout << "]" << endl;
    }
    
    // 验证线段树的正确性
    bool validate() const {
        return validate_node(1, 0, n - 1);
    }
    
private:
    bool validate_node(int node, int start, int end) const {
        if (start == end) {
            return tree[node] == arr[start];
        }
        
        int mid = (start + end) / 2;
        int left_child = 2 * node;
        int right_child = 2 * node + 1;
        
        T expected = combine(tree[left_child], tree[right_child]);
        
        return tree[node] == expected && 
               validate_node(left_child, start, mid) &&
               validate_node(right_child, mid + 1, end);
    }
};

// 特化的线段树类型

// 区间和线段树
template<typename T>
class SumSegmentTree : public SegmentTree<T> {
public:
    SumSegmentTree(const vector<T>& arr) 
        : SegmentTree<T>(arr, [](T a, T b) { return a + b; }, T{}) {}
    
    SumSegmentTree(int size, T initial_value = T{}) 
        : SegmentTree<T>(size, initial_value, [](T a, T b) { return a + b; }, T{}) {}
};

// 区间最大值线段树
template<typename T>
class MaxSegmentTree : public SegmentTree<T> {
public:
    MaxSegmentTree(const vector<T>& arr) 
        : SegmentTree<T>(arr, [](T a, T b) { return max(a, b); }, numeric_limits<T>::lowest()) {}
    
    MaxSegmentTree(int size, T initial_value = T{}) 
        : SegmentTree<T>(size, initial_value, [](T a, T b) { return max(a, b); }, numeric_limits<T>::lowest()) {}
};

// 区间最小值线段树
template<typename T>
class MinSegmentTree : public SegmentTree<T> {
public:
    MinSegmentTree(const vector<T>& arr) 
        : SegmentTree<T>(arr, [](T a, T b) { return min(a, b); }, numeric_limits<T>::max()) {}
    
    MinSegmentTree(int size, T initial_value = T{}) 
        : SegmentTree<T>(size, initial_value, [](T a, T b) { return min(a, b); }, numeric_limits<T>::max()) {}
};

// 区间异或线段树
template<typename T>
class XorSegmentTree : public SegmentTree<T> {
public:
    XorSegmentTree(const vector<T>& arr) 
        : SegmentTree<T>(arr, [](T a, T b) { return a ^ b; }, T{}) {}
    
    XorSegmentTree(int size, T initial_value = T{}) 
        : SegmentTree<T>(size, initial_value, [](T a, T b) { return a ^ b; }, T{}) {}
};

// 区间GCD线段树
template<typename T>
class GcdSegmentTree : public SegmentTree<T> {
private:
    T gcd(T a, T b) {
        while (b != 0) {
            T temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }
    
public:
    GcdSegmentTree(const vector<T>& arr) 
        : SegmentTree<T>(arr, [this](T a, T b) { return gcd(a, b); }, T{}) {}
    
    GcdSegmentTree(int size, T initial_value = T{}) 
        : SegmentTree<T>(size, initial_value, [this](T a, T b) { return gcd(a, b); }, T{}) {}
};

// 实用函数

// 从数组构建求和线段树
template<typename T>
SumSegmentTree<T> build_sum_segment_tree(const vector<T>& arr) {
    return SumSegmentTree<T>(arr);
}

// 从数组构建最大值线段树
template<typename T>
MaxSegmentTree<T> build_max_segment_tree(const vector<T>& arr) {
    return MaxSegmentTree<T>(arr);
}

// 从数组构建最小值线段树
template<typename T>
MinSegmentTree<T> build_min_segment_tree(const vector<T>& arr) {
    return MinSegmentTree<T>(arr);
}

// 区间最大子段和结构
template<typename T>
struct MaxSubarrayInfo {
    T max_sum;        // 最大子段和
    T prefix_sum;     // 最大前缀和
    T suffix_sum;     // 最大后缀和
    T total_sum;      // 总和
    
    MaxSubarrayInfo(T val = T{}) 
        : max_sum(val), prefix_sum(val), suffix_sum(val), total_sum(val) {}
    
    MaxSubarrayInfo(T max_s, T prefix_s, T suffix_s, T total_s)
        : max_sum(max_s), prefix_sum(prefix_s), suffix_sum(suffix_s), total_sum(total_s) {}
};

// 区间最大子段和线段树
template<typename T>
class MaxSubarraySegmentTree : public SegmentTree<MaxSubarrayInfo<T>> {
private:
    MaxSubarrayInfo<T> combine_subarray(const MaxSubarrayInfo<T>& left, const MaxSubarrayInfo<T>& right) {
        MaxSubarrayInfo<T> result;
        
        result.total_sum = left.total_sum + right.total_sum;
        result.prefix_sum = max(left.prefix_sum, left.total_sum + right.prefix_sum);
        result.suffix_sum = max(right.suffix_sum, right.total_sum + left.suffix_sum);
        result.max_sum = max({left.max_sum, right.max_sum, left.suffix_sum + right.prefix_sum});
        
        return result;
    }
    
public:
    MaxSubarraySegmentTree(const vector<T>& arr) {
        vector<MaxSubarrayInfo<T>> info_arr;
        for (T val : arr) {
            info_arr.emplace_back(val);
        }
        
        *this = SegmentTree<MaxSubarrayInfo<T>>(
            info_arr,
            [this](const MaxSubarrayInfo<T>& a, const MaxSubarrayInfo<T>& b) {
                return combine_subarray(a, b);
            },
            MaxSubarrayInfo<T>(numeric_limits<T>::lowest())
        );
    }
    
    // 查询区间最大子段和
    T query_max_subarray(int l, int r) {
        auto info = this->query(l, r);
        return info.max_sum;
    }
    
    // 更新单点值
    void update_value(int idx, T val) {
        this->update(idx, MaxSubarrayInfo<T>(val));
    }
};

// 性能测试结果
struct SegmentTreePerformance {
    double build_time_ms;
    double query_time_ms;
    double update_time_ms;
    int query_count;
    int update_count;
    
    void print() const {
        cout << "Segment Tree Performance:" << endl;
        cout << "  Build time: " << build_time_ms << " ms" << endl;
        cout << "  Average query time: " << query_time_ms / query_count << " ms" << endl;
        cout << "  Average update time: " << update_time_ms / update_count << " ms" << endl;
        cout << "  Total queries: " << query_count << endl;
        cout << "  Total updates: " << update_count << endl;
    }
};

#endif // SEGMENT_TREE_H