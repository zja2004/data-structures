#ifndef FENWICK_TREE_H
#define FENWICK_TREE_H

#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <numeric>
#include <climits>

using namespace std;

// 基础树状数组模板类
template<typename T>
class FenwickTree {
private:
    vector<T> tree;  // 树状数组
    int n;           // 数组大小
    
    // lowbit操作：获取x的最低位1及其后面的0
    int lowbit(int x) {
        return x & (-x);
    }
    
public:
    // 构造函数：从大小构造
    explicit FenwickTree(int size) : n(size), tree(size + 1, T{}) {
        if (size <= 0) {
            throw invalid_argument("Size must be positive");
        }
    }
    
    // 构造函数：从数组构造
    explicit FenwickTree(const vector<T>& arr) : n(arr.size()), tree(arr.size() + 1, T{}) {
        if (arr.empty()) {
            throw invalid_argument("Array cannot be empty");
        }
        
        // 逐个添加元素构建树状数组
        for (int i = 0; i < n; ++i) {
            update(i + 1, arr[i]);
        }
    }
    
    // 拷贝构造函数
    FenwickTree(const FenwickTree& other) : tree(other.tree), n(other.n) {}
    
    // 赋值操作符
    FenwickTree& operator=(const FenwickTree& other) {
        if (this != &other) {
            tree = other.tree;
            n = other.n;
        }
        return *this;
    }
    
    // 移动构造函数
    FenwickTree(FenwickTree&& other) noexcept : tree(move(other.tree)), n(other.n) {
        other.n = 0;
    }
    
    // 移动赋值操作符
    FenwickTree& operator=(FenwickTree&& other) noexcept {
        if (this != &other) {
            tree = move(other.tree);
            n = other.n;
            other.n = 0;
        }
        return *this;
    }
    
    // 单点更新：将位置i的值增加delta
    void update(int i, T delta) {
        if (i < 1 || i > n) {
            throw out_of_range("Index out of range");
        }
        
        for (; i <= n; i += lowbit(i)) {
            tree[i] += delta;
        }
    }
    
    // 前缀和查询：计算[1, i]的和
    T prefix_sum(int i) {
        if (i < 0) return T{};
        if (i > n) i = n;
        
        T sum = T{};
        for (; i > 0; i -= lowbit(i)) {
            sum += tree[i];
        }
        return sum;
    }
    
    // 区间和查询：计算[l, r]的和
    T range_sum(int l, int r) {
        if (l > r) return T{};
        if (l < 1) l = 1;
        if (r > n) r = n;
        
        return prefix_sum(r) - prefix_sum(l - 1);
    }
    
    // 单点查询：获取位置i的值
    T query(int i) {
        if (i < 1 || i > n) {
            throw out_of_range("Index out of range");
        }
        
        return range_sum(i, i);
    }
    
    // 单点设置：将位置i的值设置为val
    void set(int i, T val) {
        T current = query(i);
        update(i, val - current);
    }
    
    // 获取数组大小
    int size() const {
        return n;
    }
    
    // 检查是否为空
    bool empty() const {
        return n == 0;
    }
    
    // 清空树状数组
    void clear() {
        fill(tree.begin(), tree.end(), T{});
    }
    
    // 获取总和
    T total_sum() {
        return prefix_sum(n);
    }
    
    // 二分查找：找到第一个前缀和 >= target 的位置
    int lower_bound(T target) {
        int pos = 0;
        int bit_mask = 1;
        
        // 找到最高位
        while (bit_mask <= n) bit_mask <<= 1;
        bit_mask >>= 1;
        
        // 从高位到低位二分查找
        for (; bit_mask > 0; bit_mask >>= 1) {
            int next_pos = pos + bit_mask;
            if (next_pos <= n && tree[next_pos] < target) {
                target -= tree[next_pos];
                pos = next_pos;
            }
        }
        
        return pos + 1;
    }
    
    // 查找第k小元素的位置（1-indexed）
    int kth_element(int k) {
        if (k < 1) {
            throw out_of_range("k must be positive");
        }
        
        int pos = lower_bound(k);
        if (pos > n) {
            throw out_of_range("k is too large");
        }
        
        return pos;
    }
    
    // 获取原数组表示
    vector<T> get_array() {
        vector<T> result(n);
        for (int i = 1; i <= n; ++i) {
            result[i - 1] = query(i);
        }
        return result;
    }
    
    // 打印树状数组结构
    void print_structure() {
        cout << "Fenwick Tree Structure (size = " << n << "):" << endl;
        for (int i = 1; i <= n; ++i) {
            int left = i - lowbit(i) + 1;
            cout << "tree[" << i << "] = " << tree[i] 
                 << " (covers [" << left << ", " << i << "])" << endl;
        }
    }
    
    // 打印原数组
    void print_array() {
        cout << "Array: ";
        for (int i = 1; i <= n; ++i) {
            cout << query(i) << " ";
        }
        cout << endl;
    }
    
    // 验证树状数组的正确性
    bool validate() {
        vector<T> arr = get_array();
        
        // 验证前缀和
        for (int i = 1; i <= n; ++i) {
            T expected = T{};
            for (int j = 0; j < i; ++j) {
                expected += arr[j];
            }
            if (prefix_sum(i) != expected) {
                return false;
            }
        }
        
        return true;
    }
};

// 特化的树状数组类型

// 整数求和树状数组
using IntFenwickTree = FenwickTree<int>;
using LongFenwickTree = FenwickTree<long long>;

// 浮点数求和树状数组
using DoubleFenwickTree = FenwickTree<double>;

// 差分树状数组：支持区间更新，单点查询
template<typename T>
class DifferenceFenwickTree {
private:
    FenwickTree<T> diff_tree;  // 差分数组的树状数组
    
public:
    // 构造函数
    explicit DifferenceFenwickTree(int size) : diff_tree(size) {}
    
    explicit DifferenceFenwickTree(const vector<T>& arr) : diff_tree(arr.size()) {
        if (arr.empty()) {
            throw invalid_argument("Array cannot be empty");
        }
        
        // 构建差分数组
        diff_tree.update(1, arr[0]);
        for (int i = 1; i < arr.size(); ++i) {
            diff_tree.update(i + 1, arr[i] - arr[i - 1]);
        }
    }
    
    // 区间更新：将[l, r]区间内所有元素加上delta
    void range_update(int l, int r, T delta) {
        if (l > r) return;
        if (l < 1 || r > diff_tree.size()) {
            throw out_of_range("Range out of bounds");
        }
        
        diff_tree.update(l, delta);
        if (r + 1 <= diff_tree.size()) {
            diff_tree.update(r + 1, -delta);
        }
    }
    
    // 单点查询：获取位置i的值
    T query(int i) {
        if (i < 1 || i > diff_tree.size()) {
            throw out_of_range("Index out of range");
        }
        
        return diff_tree.prefix_sum(i);
    }
    
    // 单点更新：将位置i的值增加delta
    void update(int i, T delta) {
        range_update(i, i, delta);
    }
    
    // 获取数组大小
    int size() const {
        return diff_tree.size();
    }
    
    // 获取原数组表示
    vector<T> get_array() {
        vector<T> result(size());
        for (int i = 1; i <= size(); ++i) {
            result[i - 1] = query(i);
        }
        return result;
    }
    
    // 打印数组
    void print_array() {
        cout << "Array: ";
        for (int i = 1; i <= size(); ++i) {
            cout << query(i) << " ";
        }
        cout << endl;
    }
};

// 双树状数组：支持区间更新，区间查询
template<typename T>
class RangeFenwickTree {
private:
    FenwickTree<T> tree1;  // 维护差分数组
    FenwickTree<T> tree2;  // 维护差分数组乘以索引
    
public:
    // 构造函数
    explicit RangeFenwickTree(int size) : tree1(size), tree2(size) {}
    
    explicit RangeFenwickTree(const vector<T>& arr) : tree1(arr.size()), tree2(arr.size()) {
        if (arr.empty()) {
            throw invalid_argument("Array cannot be empty");
        }
        
        // 初始化
        for (int i = 0; i < arr.size(); ++i) {
            range_update(i + 1, i + 1, arr[i]);
        }
    }
    
    // 区间更新：将[l, r]区间内所有元素加上delta
    void range_update(int l, int r, T delta) {
        if (l > r) return;
        if (l < 1 || r > tree1.size()) {
            throw out_of_range("Range out of bounds");
        }
        
        tree1.update(l, delta);
        tree1.update(r + 1, -delta);
        tree2.update(l, delta * (l - 1));
        tree2.update(r + 1, -delta * r);
    }
    
    // 前缀和查询
    T prefix_sum(int i) {
        if (i < 0) return T{};
        if (i > tree1.size()) i = tree1.size();
        
        return tree1.prefix_sum(i) * i - tree2.prefix_sum(i);
    }
    
    // 区间和查询：计算[l, r]的和
    T range_sum(int l, int r) {
        if (l > r) return T{};
        if (l < 1) l = 1;
        if (r > tree1.size()) r = tree1.size();
        
        return prefix_sum(r) - prefix_sum(l - 1);
    }
    
    // 单点查询：获取位置i的值
    T query(int i) {
        if (i < 1 || i > tree1.size()) {
            throw out_of_range("Index out of range");
        }
        
        return range_sum(i, i);
    }
    
    // 单点更新：将位置i的值增加delta
    void update(int i, T delta) {
        range_update(i, i, delta);
    }
    
    // 获取数组大小
    int size() const {
        return tree1.size();
    }
    
    // 获取原数组表示
    vector<T> get_array() {
        vector<T> result(size());
        for (int i = 1; i <= size(); ++i) {
            result[i - 1] = query(i);
        }
        return result;
    }
    
    // 打印数组
    void print_array() {
        cout << "Array: ";
        for (int i = 1; i <= size(); ++i) {
            cout << query(i) << " ";
        }
        cout << endl;
    }
};

// 最值树状数组（使用分块思想）
template<typename T>
class MaxFenwickTree {
private:
    vector<T> tree;
    vector<T> original;
    int n;
    
    int lowbit(int x) {
        return x & (-x);
    }
    
    // 重新计算tree[i]的值
    void rebuild(int i) {
        tree[i] = original[i];
        int left = i - lowbit(i) + 1;
        for (int j = left; j < i; ++j) {
            tree[i] = max(tree[i], tree[j]);
        }
    }
    
public:
    // 构造函数
    explicit MaxFenwickTree(int size) : n(size), tree(size + 1), original(size + 1) {
        if (size <= 0) {
            throw invalid_argument("Size must be positive");
        }
        
        fill(tree.begin(), tree.end(), numeric_limits<T>::lowest());
        fill(original.begin(), original.end(), numeric_limits<T>::lowest());
    }
    
    explicit MaxFenwickTree(const vector<T>& arr) : n(arr.size()), tree(arr.size() + 1), original(arr.size() + 1) {
        if (arr.empty()) {
            throw invalid_argument("Array cannot be empty");
        }
        
        // 初始化原数组
        for (int i = 0; i < n; ++i) {
            original[i + 1] = arr[i];
        }
        
        // 构建树状数组
        for (int i = 1; i <= n; ++i) {
            rebuild(i);
        }
    }
    
    // 单点更新
    void update(int i, T val) {
        if (i < 1 || i > n) {
            throw out_of_range("Index out of range");
        }
        
        original[i] = val;
        
        // 更新所有受影响的节点
        for (int j = i; j <= n; j += lowbit(j)) {
            rebuild(j);
        }
    }
    
    // 前缀最大值查询
    T prefix_max(int i) {
        if (i < 0) return numeric_limits<T>::lowest();
        if (i > n) i = n;
        
        T result = numeric_limits<T>::lowest();
        for (; i > 0; i -= lowbit(i)) {
            result = max(result, tree[i]);
        }
        return result;
    }
    
    // 区间最大值查询（简单实现，效率较低）
    T range_max(int l, int r) {
        if (l > r) return numeric_limits<T>::lowest();
        if (l < 1) l = 1;
        if (r > n) r = n;
        
        T result = numeric_limits<T>::lowest();
        for (int i = l; i <= r; ++i) {
            result = max(result, original[i]);
        }
        return result;
    }
    
    // 单点查询
    T query(int i) {
        if (i < 1 || i > n) {
            throw out_of_range("Index out of range");
        }
        
        return original[i];
    }
    
    // 获取数组大小
    int size() const {
        return n;
    }
    
    // 获取原数组表示
    vector<T> get_array() {
        vector<T> result(n);
        for (int i = 1; i <= n; ++i) {
            result[i - 1] = original[i];
        }
        return result;
    }
};

// 实用函数

// 构建整数树状数组
IntFenwickTree build_int_fenwick_tree(const vector<int>& arr) {
    return IntFenwickTree(arr);
}

// 构建长整数树状数组
LongFenwickTree build_long_fenwick_tree(const vector<long long>& arr) {
    return LongFenwickTree(arr);
}

// 构建浮点数树状数组
DoubleFenwickTree build_double_fenwick_tree(const vector<double>& arr) {
    return DoubleFenwickTree(arr);
}

// 计算逆序对数量
template<typename T>
long long count_inversions(vector<T> arr) {
    if (arr.empty()) return 0;
    
    // 离散化
    vector<T> sorted_arr = arr;
    sort(sorted_arr.begin(), sorted_arr.end());
    sorted_arr.erase(unique(sorted_arr.begin(), sorted_arr.end()), sorted_arr.end());
    
    // 映射到离散化后的值
    for (auto& x : arr) {
        x = lower_bound(sorted_arr.begin(), sorted_arr.end(), x) - sorted_arr.begin() + 1;
    }
    
    FenwickTree<int> bit(sorted_arr.size());
    long long inversions = 0;
    
    // 从右到左扫描
    for (int i = arr.size() - 1; i >= 0; i--) {
        // 查询比当前元素小的元素个数
        inversions += bit.prefix_sum(arr[i] - 1);
        // 将当前元素加入树状数组
        bit.update(arr[i], 1);
    }
    
    return inversions;
}

// 动态第k小元素
template<typename T>
class DynamicKthElement {
private:
    vector<T> values;
    FenwickTree<int> bit;
    
public:
    DynamicKthElement(const vector<T>& sorted_values) 
        : values(sorted_values), bit(sorted_values.size()) {
        
        // 确保values是排序的
        if (!is_sorted(values.begin(), values.end())) {
            throw invalid_argument("Values must be sorted");
        }
    }
    
    // 添加元素
    void add(T val) {
        int pos = lower_bound(values.begin(), values.end(), val) - values.begin();
        if (pos < values.size() && values[pos] == val) {
            bit.update(pos + 1, 1);
        } else {
            throw invalid_argument("Value not in predefined range");
        }
    }
    
    // 删除元素
    void remove(T val) {
        int pos = lower_bound(values.begin(), values.end(), val) - values.begin();
        if (pos < values.size() && values[pos] == val) {
            if (bit.query(pos + 1) > 0) {
                bit.update(pos + 1, -1);
            } else {
                throw runtime_error("Element not found");
            }
        } else {
            throw invalid_argument("Value not in predefined range");
        }
    }
    
    // 查询第k小元素
    T kth_element(int k) {
        if (k < 1 || k > bit.total_sum()) {
            throw out_of_range("k out of range");
        }
        
        int pos = bit.kth_element(k);
        return values[pos - 1];
    }
    
    // 获取元素总数
    int size() {
        return bit.total_sum();
    }
    
    // 查询小于等于val的元素个数
    int count_le(T val) {
        int pos = upper_bound(values.begin(), values.end(), val) - values.begin();
        return bit.prefix_sum(pos);
    }
    
    // 查询小于val的元素个数
    int count_lt(T val) {
        int pos = lower_bound(values.begin(), values.end(), val) - values.begin();
        return bit.prefix_sum(pos);
    }
};

// 性能统计结构
struct FenwickTreePerformance {
    int update_count = 0;
    int query_count = 0;
    double total_update_time = 0.0;
    double total_query_time = 0.0;
    
    double avg_update_time() const {
        return update_count > 0 ? total_update_time / update_count : 0.0;
    }
    
    double avg_query_time() const {
        return query_count > 0 ? total_query_time / query_count : 0.0;
    }
    
    void print_stats() const {
        cout << "Fenwick Tree Performance:" << endl;
        cout << "  Updates: " << update_count << " (avg: " << avg_update_time() << " ms)" << endl;
        cout << "  Queries: " << query_count << " (avg: " << avg_query_time() << " ms)" << endl;
    }
};

#endif // FENWICK_TREE_H