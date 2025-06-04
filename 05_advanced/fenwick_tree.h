#ifndef FENWICK_TREE_H
#define FENWICK_TREE_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>
#include <random>
#include <chrono>

namespace fenwick_tree {

// 基础树状数组(Binary Indexed Tree)
template<typename T>
class FenwickTree {
private:
    std::vector<T> tree;
    int n;
    
    // 获取最低位的1
    int lowbit(int x) {
        return x & (-x);
    }
    
public:
    explicit FenwickTree(int size) : n(size) {
        tree.assign(n + 1, T{});
    }
    
    explicit FenwickTree(const std::vector<T>& data) : n(data.size()) {
        tree.assign(n + 1, T{});
        for (int i = 0; i < n; ++i) {
            update(i + 1, data[i]);
        }
    }
    
    // 单点更新：将位置i的值增加delta
    void update(int i, T delta) {
        for (; i <= n; i += lowbit(i)) {
            tree[i] += delta;
        }
    }
    
    // 前缀和查询：查询[1, i]的和
    T query(int i) {
        T sum = T{};
        for (; i > 0; i -= lowbit(i)) {
            sum += tree[i];
        }
        return sum;
    }
    
    // 区间和查询：查询[l, r]的和
    T range_query(int l, int r) {
        if (l > r) return T{};
        return query(r) - query(l - 1);
    }
    
    // 单点查询：查询位置i的值
    T point_query(int i) {
        return range_query(i, i);
    }
    
    // 单点赋值：将位置i的值设为val
    void set(int i, T val) {
        T current = point_query(i);
        update(i, val - current);
    }
    
    // 二分查找：找到第一个前缀和>=target的位置
    int lower_bound(T target) {
        int pos = 0;
        T sum = T{};
        
        for (int k = __builtin_clz(1) - __builtin_clz(n); k >= 0; --k) {
            int next_pos = pos + (1 << k);
            if (next_pos <= n && sum + tree[next_pos] < target) {
                pos = next_pos;
                sum += tree[pos];
            }
        }
        
        return pos + 1;
    }
    
    // 获取树状数组大小
    int size() const {
        return n;
    }
    
    // 打印树状数组状态
    void print_tree() {
        std::cout << "树状数组: ";
        for (int i = 1; i <= std::min(n, 10); ++i) {
            std::cout << tree[i] << " ";
        }
        if (n > 10) std::cout << "...";
        std::cout << "\n";
    }
    
    // 打印原数组
    void print_array() {
        std::cout << "原数组: ";
        for (int i = 1; i <= std::min(n, 10); ++i) {
            std::cout << point_query(i) << " ";
        }
        if (n > 10) std::cout << "...";
        std::cout << "\n";
    }
};

// 区间更新树状数组(使用差分数组)
template<typename T>
class RangeUpdateFenwickTree {
private:
    FenwickTree<T> diff_tree;  // 差分数组的树状数组
    
public:
    explicit RangeUpdateFenwickTree(int size) : diff_tree(size) {}
    
    explicit RangeUpdateFenwickTree(const std::vector<T>& data) : diff_tree(data.size()) {
        for (int i = 0; i < data.size(); ++i) {
            if (i == 0) {
                diff_tree.update(i + 1, data[i]);
            } else {
                diff_tree.update(i + 1, data[i] - data[i - 1]);
            }
        }
    }
    
    // 区间更新：将[l, r]区间内的所有元素增加delta
    void range_update(int l, int r, T delta) {
        diff_tree.update(l, delta);
        diff_tree.update(r + 1, -delta);
    }
    
    // 单点查询：查询位置i的值
    T point_query(int i) {
        return diff_tree.query(i);
    }
    
    // 单点更新：将位置i的值增加delta
    void point_update(int i, T delta) {
        range_update(i, i, delta);
    }
};

// 支持区间更新和区间查询的树状数组
template<typename T>
class RangeUpdateRangeQueryFenwickTree {
private:
    FenwickTree<T> tree1;  // 存储差分数组
    FenwickTree<T> tree2;  // 存储i*diff[i]
    
public:
    explicit RangeUpdateRangeQueryFenwickTree(int size) : tree1(size), tree2(size) {}
    
    explicit RangeUpdateRangeQueryFenwickTree(const std::vector<T>& data) 
        : tree1(data.size()), tree2(data.size()) {
        for (int i = 0; i < data.size(); ++i) {
            range_update(i + 1, i + 1, data[i]);
        }
    }
    
    // 区间更新：将[l, r]区间内的所有元素增加delta
    void range_update(int l, int r, T delta) {
        tree1.update(l, delta);
        tree1.update(r + 1, -delta);
        tree2.update(l, delta * (l - 1));
        tree2.update(r + 1, -delta * r);
    }
    
    // 前缀和查询
    T prefix_sum(int i) {
        return tree1.query(i) * i - tree2.query(i);
    }
    
    // 区间和查询：查询[l, r]的和
    T range_query(int l, int r) {
        return prefix_sum(r) - prefix_sum(l - 1);
    }
    
    // 单点查询：查询位置i的值
    T point_query(int i) {
        return range_query(i, i);
    }
};

// 二维树状数组
template<typename T>
class FenwickTree2D {
private:
    std::vector<std::vector<T>> tree;
    int n, m;
    
    int lowbit(int x) {
        return x & (-x);
    }
    
public:
    FenwickTree2D(int rows, int cols) : n(rows), m(cols) {
        tree.assign(n + 1, std::vector<T>(m + 1, T{}));
    }
    
    FenwickTree2D(const std::vector<std::vector<T>>& matrix) 
        : n(matrix.size()), m(matrix[0].size()) {
        tree.assign(n + 1, std::vector<T>(m + 1, T{}));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                update(i + 1, j + 1, matrix[i][j]);
            }
        }
    }
    
    // 单点更新：将位置(i, j)的值增加delta
    void update(int i, int j, T delta) {
        for (int x = i; x <= n; x += lowbit(x)) {
            for (int y = j; y <= m; y += lowbit(y)) {
                tree[x][y] += delta;
            }
        }
    }
    
    // 查询矩形区域[1,1]到[i,j]的和
    T query(int i, int j) {
        T sum = T{};
        for (int x = i; x > 0; x -= lowbit(x)) {
            for (int y = j; y > 0; y -= lowbit(y)) {
                sum += tree[x][y];
            }
        }
        return sum;
    }
    
    // 查询矩形区域[x1,y1]到[x2,y2]的和
    T range_query(int x1, int y1, int x2, int y2) {
        return query(x2, y2) - query(x1 - 1, y2) - query(x2, y1 - 1) + query(x1 - 1, y1 - 1);
    }
    
    // 单点查询：查询位置(i, j)的值
    T point_query(int i, int j) {
        return range_query(i, j, i, j);
    }
    
    // 单点赋值：将位置(i, j)的值设为val
    void set(int i, int j, T val) {
        T current = point_query(i, j);
        update(i, j, val - current);
    }
};

// 树状数组应用类
class FenwickTreeApplications {
public:
    // 逆序对统计
    template<typename T>
    static long long count_inversions(std::vector<T> arr) {
        // 离散化
        std::vector<T> sorted_arr = arr;
        std::sort(sorted_arr.begin(), sorted_arr.end());
        sorted_arr.erase(std::unique(sorted_arr.begin(), sorted_arr.end()), sorted_arr.end());
        
        int n = arr.size();
        int m = sorted_arr.size();
        FenwickTree<int> ft(m);
        
        long long inversions = 0;
        
        for (int i = n - 1; i >= 0; --i) {
            int pos = std::lower_bound(sorted_arr.begin(), sorted_arr.end(), arr[i]) - sorted_arr.begin() + 1;
            
            // 查询比当前元素小的元素个数
            if (pos > 1) {
                inversions += ft.query(pos - 1);
            }
            
            // 更新计数
            ft.update(pos, 1);
        }
        
        return inversions;
    }
    
    // 动态排名查询
    template<typename T>
    class DynamicRankQuery {
    private:
        std::vector<T> values;
        FenwickTree<int> ft;
        
    public:
        explicit DynamicRankQuery(const std::vector<T>& initial_values) {
            values = initial_values;
            std::sort(values.begin(), values.end());
            values.erase(std::unique(values.begin(), values.end()), values.end());
            ft = FenwickTree<int>(values.size());
        }
        
        void insert(T val) {
            auto it = std::lower_bound(values.begin(), values.end(), val);
            if (it == values.end() || *it != val) {
                int pos = it - values.begin();
                values.insert(it, val);
                // 重建树状数组(简化实现)
                ft = FenwickTree<int>(values.size());
            }
            
            int pos = std::lower_bound(values.begin(), values.end(), val) - values.begin() + 1;
            ft.update(pos, 1);
        }
        
        void remove(T val) {
            int pos = std::lower_bound(values.begin(), values.end(), val) - values.begin() + 1;
            if (pos <= values.size() && values[pos - 1] == val) {
                ft.update(pos, -1);
            }
        }
        
        int get_rank(T val) {
            int pos = std::lower_bound(values.begin(), values.end(), val) - values.begin();
            return pos > 0 ? ft.query(pos) + 1 : 1;
        }
        
        T get_kth(int k) {
            int pos = ft.lower_bound(k);
            return pos <= values.size() ? values[pos - 1] : T{};
        }
    };
    
    // 区间最值查询(使用ST表的思想)
    template<typename T>
    class RangeMaxQuery {
    private:
        std::vector<std::vector<T>> st;
        std::vector<int> lg;
        
    public:
        explicit RangeMaxQuery(const std::vector<T>& data) {
            int n = data.size();
            lg.resize(n + 1);
            lg[1] = 0;
            for (int i = 2; i <= n; ++i) {
                lg[i] = lg[i / 2] + 1;
            }
            
            st.assign(lg[n] + 1, std::vector<T>(n));
            
            for (int i = 0; i < n; ++i) {
                st[0][i] = data[i];
            }
            
            for (int j = 1; j <= lg[n]; ++j) {
                for (int i = 0; i + (1 << j) <= n; ++i) {
                    st[j][i] = std::max(st[j - 1][i], st[j - 1][i + (1 << (j - 1))]);
                }
            }
        }
        
        T query(int l, int r) {
            int j = lg[r - l + 1];
            return std::max(st[j][l], st[j][r - (1 << j) + 1]);
        }
    };
    
    // 矩阵区域和查询
    template<typename T>
    static T matrix_range_sum(const std::vector<std::vector<T>>& matrix,
                             int x1, int y1, int x2, int y2) {
        FenwickTree2D<T> ft2d(matrix);
        return ft2d.range_query(x1 + 1, y1 + 1, x2 + 1, y2 + 1);
    }
    
    // 滑动窗口最大值(使用单调队列)
    template<typename T>
    static std::vector<T> sliding_window_maximum(const std::vector<T>& arr, int k) {
        std::deque<int> dq;
        std::vector<T> result;
        
        for (int i = 0; i < arr.size(); ++i) {
            // 移除超出窗口的元素
            while (!dq.empty() && dq.front() <= i - k) {
                dq.pop_front();
            }
            
            // 维护单调递减队列
            while (!dq.empty() && arr[dq.back()] <= arr[i]) {
                dq.pop_back();
            }
            
            dq.push_back(i);
            
            // 当窗口大小达到k时，记录最大值
            if (i >= k - 1) {
                result.push_back(arr[dq.front()]);
            }
        }
        
        return result;
    }
};

// 性能测试类
class FenwickTreeTester {
public:
    template<typename T>
    static void performance_test(int n, int operations) {
        std::cout << "\n树状数组性能测试 (n=" << n << ", operations=" << operations << ")\n";
        
        // 生成测试数据
        std::vector<T> data(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<T> dis(1, 1000);
        
        for (int i = 0; i < n; ++i) {
            data[i] = dis(gen);
        }
        
        // 测试基础树状数组
        auto start = std::chrono::high_resolution_clock::now();
        FenwickTree<T> ft(data);
        
        std::uniform_int_distribution<int> idx_dis(1, n);
        
        // 测试查询操作
        for (int i = 0; i < operations; ++i) {
            int l = idx_dis(gen);
            int r = idx_dis(gen);
            if (l > r) std::swap(l, r);
            ft.range_query(l, r);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "区间查询用时: " << duration.count() << " 微秒\n";
        
        // 测试更新操作
        start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < operations; ++i) {
            int idx = idx_dis(gen);
            T val = dis(gen);
            ft.update(idx, val);
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "更新操作用时: " << duration.count() << " 微秒\n";
        
        // 测试二维树状数组
        int sqrt_n = static_cast<int>(std::sqrt(n));
        std::vector<std::vector<T>> matrix(sqrt_n, std::vector<T>(sqrt_n));
        for (int i = 0; i < sqrt_n; ++i) {
            for (int j = 0; j < sqrt_n; ++j) {
                matrix[i][j] = dis(gen);
            }
        }
        
        start = std::chrono::high_resolution_clock::now();
        FenwickTree2D<T> ft2d(matrix);
        
        std::uniform_int_distribution<int> coord_dis(1, sqrt_n);
        
        for (int i = 0; i < operations / 10; ++i) {
            int x1 = coord_dis(gen), y1 = coord_dis(gen);
            int x2 = coord_dis(gen), y2 = coord_dis(gen);
            if (x1 > x2) std::swap(x1, x2);
            if (y1 > y2) std::swap(y1, y2);
            ft2d.range_query(x1, y1, x2, y2);
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "二维区间查询用时: " << duration.count() << " 微秒\n";
    }
    
    static void correctness_test() {
        std::cout << "\n树状数组正确性测试\n";
        
        std::vector<int> data = {1, 3, 5, 7, 9, 11};
        FenwickTree<int> ft(data);
        
        // 测试前缀和查询
        assert(ft.query(3) == 9);   // 1 + 3 + 5
        assert(ft.query(6) == 36);  // 所有元素和
        
        // 测试区间和查询
        assert(ft.range_query(2, 4) == 15);  // 3 + 5 + 7
        assert(ft.range_query(1, 6) == 36);  // 所有元素
        
        // 测试更新操作
        ft.update(3, 5);  // 将位置3的值增加5
        assert(ft.query(3) == 14);  // 1 + 3 + (5+5)
        assert(ft.range_query(2, 4) == 20);  // 3 + (5+5) + 7
        
        // 测试单点赋值
        ft.set(3, 8);  // 将位置3的值设为8
        assert(ft.point_query(3) == 8);
        assert(ft.range_query(2, 4) == 18);  // 3 + 8 + 7
        
        // 测试二维树状数组
        std::vector<std::vector<int>> matrix = {
            {1, 2, 3},
            {4, 5, 6},
            {7, 8, 9}
        };
        FenwickTree2D<int> ft2d(matrix);
        
        assert(ft2d.range_query(1, 1, 2, 2) == 12);  // 1+2+4+5
        assert(ft2d.range_query(2, 2, 3, 3) == 28);  // 5+6+8+9
        
        ft2d.update(2, 2, 5);  // 将(2,2)位置增加5
        assert(ft2d.range_query(1, 1, 2, 2) == 17);  // 1+2+4+(5+5)
        
        std::cout << "所有测试通过！\n";
    }
    
    static void application_test() {
        std::cout << "\n树状数组应用测试\n";
        
        // 测试逆序对统计
        std::vector<int> arr = {3, 1, 4, 1, 5, 9, 2, 6};
        long long inversions = FenwickTreeApplications::count_inversions(arr);
        std::cout << "逆序对数量: " << inversions << "\n";
        
        // 测试动态排名查询
        std::vector<int> initial = {1, 3, 5, 7, 9};
        FenwickTreeApplications::DynamicRankQuery<int> drq(initial);
        
        drq.insert(1);
        drq.insert(3);
        drq.insert(6);
        
        std::cout << "元素6的排名: " << drq.get_rank(6) << "\n";
        std::cout << "第4小的元素: " << drq.get_kth(4) << "\n";
        
        // 测试滑动窗口最大值
        std::vector<int> window_arr = {1, 3, -1, -3, 5, 3, 6, 7};
        auto max_values = FenwickTreeApplications::sliding_window_maximum(window_arr, 3);
        
        std::cout << "滑动窗口最大值: ";
        for (int val : max_values) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
};

} // namespace fenwick_tree

#endif // FENWICK_TREE_H