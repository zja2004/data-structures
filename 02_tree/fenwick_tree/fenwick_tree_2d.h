#ifndef FENWICK_TREE_2D_H
#define FENWICK_TREE_2D_H

#include <vector>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <functional>

using namespace std;

// 二维树状数组
template<typename T>
class FenwickTree2D {
private:
    vector<vector<T>> tree;
    int rows, cols;
    
    int lowbit(int x) {
        return x & (-x);
    }
    
public:
    // 构造函数
    FenwickTree2D(int r, int c) : rows(r), cols(c) {
        if (r <= 0 || c <= 0) {
            throw invalid_argument("Dimensions must be positive");
        }
        tree.assign(r + 1, vector<T>(c + 1, T{}));
    }
    
    // 从二维数组构造
    explicit FenwickTree2D(const vector<vector<T>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) {
            throw invalid_argument("Matrix cannot be empty");
        }
        
        rows = matrix.size();
        cols = matrix[0].size();
        tree.assign(rows + 1, vector<T>(cols + 1, T{}));
        
        // 逐个添加元素
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                update(i + 1, j + 1, matrix[i][j]);
            }
        }
    }
    
    // 拷贝构造函数
    FenwickTree2D(const FenwickTree2D& other) 
        : tree(other.tree), rows(other.rows), cols(other.cols) {}
    
    // 移动构造函数
    FenwickTree2D(FenwickTree2D&& other) noexcept
        : tree(move(other.tree)), rows(other.rows), cols(other.cols) {
        other.rows = other.cols = 0;
    }
    
    // 拷贝赋值运算符
    FenwickTree2D& operator=(const FenwickTree2D& other) {
        if (this != &other) {
            tree = other.tree;
            rows = other.rows;
            cols = other.cols;
        }
        return *this;
    }
    
    // 移动赋值运算符
    FenwickTree2D& operator=(FenwickTree2D&& other) noexcept {
        if (this != &other) {
            tree = move(other.tree);
            rows = other.rows;
            cols = other.cols;
            other.rows = other.cols = 0;
        }
        return *this;
    }
    
    // 单点更新
    void update(int r, int c, T delta) {
        if (r < 1 || r > rows || c < 1 || c > cols) {
            throw out_of_range("Coordinates out of range");
        }
        
        for (int i = r; i <= rows; i += lowbit(i)) {
            for (int j = c; j <= cols; j += lowbit(j)) {
                tree[i][j] += delta;
            }
        }
    }
    
    // 矩形区域和查询 (1,1) 到 (r,c)
    T prefix_sum(int r, int c) {
        if (r < 0 || c < 0) return T{};
        if (r > rows) r = rows;
        if (c > cols) c = cols;
        
        T sum = T{};
        for (int i = r; i > 0; i -= lowbit(i)) {
            for (int j = c; j > 0; j -= lowbit(j)) {
                sum += tree[i][j];
            }
        }
        return sum;
    }
    
    // 矩形区域和查询 (r1,c1) 到 (r2,c2)
    T range_sum(int r1, int c1, int r2, int c2) {
        if (r1 > r2 || c1 > c2) return T{};
        
        return prefix_sum(r2, c2) - prefix_sum(r1 - 1, c2) 
               - prefix_sum(r2, c1 - 1) + prefix_sum(r1 - 1, c1 - 1);
    }
    
    // 单点查询
    T query(int r, int c) {
        return range_sum(r, c, r, c);
    }
    
    // 单点设置
    void set(int r, int c, T val) {
        T current = query(r, c);
        update(r, c, val - current);
    }
    
    // 获取维度
    pair<int, int> dimensions() const {
        return {rows, cols};
    }
    
    // 检查是否为空
    bool empty() const {
        return rows == 0 || cols == 0;
    }
    
    // 清空
    void clear() {
        for (auto& row : tree) {
            fill(row.begin(), row.end(), T{});
        }
    }
    
    // 获取总和
    T total_sum() {
        return prefix_sum(rows, cols);
    }
    
    // 获取原矩阵表示
    vector<vector<T>> get_matrix() {
        vector<vector<T>> matrix(rows, vector<T>(cols));
        for (int i = 1; i <= rows; ++i) {
            for (int j = 1; j <= cols; ++j) {
                matrix[i - 1][j - 1] = query(i, j);
            }
        }
        return matrix;
    }
    
    // 打印树状数组结构
    void print_structure() {
        cout << "2D Fenwick Tree Structure (" << rows << "x" << cols << "):" << endl;
        for (int i = 1; i <= rows; ++i) {
            for (int j = 1; j <= cols; ++j) {
                cout << setw(8) << tree[i][j] << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    
    // 打印原矩阵
    void print_matrix() {
        cout << "Original Matrix:" << endl;
        auto matrix = get_matrix();
        for (const auto& row : matrix) {
            for (const auto& val : row) {
                cout << setw(8) << val << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
    
    // 验证树状数组的正确性
    bool validate() {
        auto matrix = get_matrix();
        
        // 验证前缀和
        for (int i = 1; i <= rows; ++i) {
            for (int j = 1; j <= cols; ++j) {
                T expected = T{};
                for (int r = 1; r <= i; ++r) {
                    for (int c = 1; c <= j; ++c) {
                        expected += matrix[r - 1][c - 1];
                    }
                }
                
                if (prefix_sum(i, j) != expected) {
                    return false;
                }
            }
        }
        
        return true;
    }
};

// 二维差分树状数组（支持区间更新，单点查询）
template<typename T>
class DifferenceFenwickTree2D {
private:
    FenwickTree2D<T> tree;
    
public:
    // 构造函数
    DifferenceFenwickTree2D(int r, int c) : tree(r, c) {}
    
    // 从二维数组构造
    explicit DifferenceFenwickTree2D(const vector<vector<T>>& matrix) {
        if (matrix.empty() || matrix[0].empty()) {
            throw invalid_argument("Matrix cannot be empty");
        }
        
        int rows = matrix.size();
        int cols = matrix[0].size();
        tree = FenwickTree2D<T>(rows, cols);
        
        // 构建差分数组
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                T diff = matrix[i][j];
                if (i > 0) diff -= matrix[i - 1][j];
                if (j > 0) diff -= matrix[i][j - 1];
                if (i > 0 && j > 0) diff += matrix[i - 1][j - 1];
                
                tree.update(i + 1, j + 1, diff);
            }
        }
    }
    
    // 矩形区域更新
    void range_update(int r1, int c1, int r2, int c2, T delta) {
        if (r1 > r2 || c1 > c2) return;
        
        tree.update(r1, c1, delta);
        tree.update(r1, c2 + 1, -delta);
        tree.update(r2 + 1, c1, -delta);
        tree.update(r2 + 1, c2 + 1, delta);
    }
    
    // 单点查询
    T query(int r, int c) {
        return tree.prefix_sum(r, c);
    }
    
    // 单点更新
    void update(int r, int c, T delta) {
        range_update(r, c, r, c, delta);
    }
    
    // 获取维度
    pair<int, int> dimensions() const {
        return tree.dimensions();
    }
    
    // 获取原矩阵表示
    vector<vector<T>> get_matrix() {
        auto dims = tree.dimensions();
        vector<vector<T>> matrix(dims.first, vector<T>(dims.second));
        
        for (int i = 1; i <= dims.first; ++i) {
            for (int j = 1; j <= dims.second; ++j) {
                matrix[i - 1][j - 1] = query(i, j);
            }
        }
        
        return matrix;
    }
    
    // 打印矩阵
    void print_matrix() {
        cout << "Matrix (after difference operations):" << endl;
        auto matrix = get_matrix();
        for (const auto& row : matrix) {
            for (const auto& val : row) {
                cout << setw(8) << val << " ";
            }
            cout << endl;
        }
        cout << endl;
    }
};

// 二维范围树状数组（支持区间更新，区间查询）
template<typename T>
class RangeFenwickTree2D {
private:
    FenwickTree2D<T> tree1;  // 存储 c * i * j
    FenwickTree2D<T> tree2;  // 存储 c * i
    FenwickTree2D<T> tree3;  // 存储 c * j
    FenwickTree2D<T> tree4;  // 存储 c
    
    void range_update_helper(int r1, int c1, int r2, int c2, T delta) {
        tree1.update(r1, c1, delta * r1 * c1);
        tree1.update(r1, c2 + 1, -delta * r1 * (c2 + 1));
        tree1.update(r2 + 1, c1, -delta * (r2 + 1) * c1);
        tree1.update(r2 + 1, c2 + 1, delta * (r2 + 1) * (c2 + 1));
        
        tree2.update(r1, c1, delta * r1);
        tree2.update(r1, c2 + 1, -delta * r1);
        tree2.update(r2 + 1, c1, -delta * (r2 + 1));
        tree2.update(r2 + 1, c2 + 1, delta * (r2 + 1));
        
        tree3.update(r1, c1, delta * c1);
        tree3.update(r1, c2 + 1, -delta * (c2 + 1));
        tree3.update(r2 + 1, c1, -delta * c1);
        tree3.update(r2 + 1, c2 + 1, delta * (c2 + 1));
        
        tree4.update(r1, c1, delta);
        tree4.update(r1, c2 + 1, -delta);
        tree4.update(r2 + 1, c1, -delta);
        tree4.update(r2 + 1, c2 + 1, delta);
    }
    
    T prefix_sum_helper(int r, int c) {
        return tree1.prefix_sum(r, c) - tree2.prefix_sum(r, c) * (c + 1)
               - tree3.prefix_sum(r, c) * (r + 1) + tree4.prefix_sum(r, c) * (r + 1) * (c + 1);
    }
    
public:
    // 构造函数
    RangeFenwickTree2D(int r, int c) 
        : tree1(r, c), tree2(r, c), tree3(r, c), tree4(r, c) {}
    
    // 矩形区域更新
    void range_update(int r1, int c1, int r2, int c2, T delta) {
        if (r1 > r2 || c1 > c2) return;
        range_update_helper(r1, c1, r2, c2, delta);
    }
    
    // 矩形区域和查询
    T range_sum(int r1, int c1, int r2, int c2) {
        if (r1 > r2 || c1 > c2) return T{};
        
        return prefix_sum_helper(r2, c2) - prefix_sum_helper(r1 - 1, c2)
               - prefix_sum_helper(r2, c1 - 1) + prefix_sum_helper(r1 - 1, c1 - 1);
    }
    
    // 单点更新
    void update(int r, int c, T delta) {
        range_update(r, c, r, c, delta);
    }
    
    // 单点查询
    T query(int r, int c) {
        return range_sum(r, c, r, c);
    }
    
    // 获取维度
    pair<int, int> dimensions() const {
        return tree1.dimensions();
    }
};

// 二维最大值树状数组
template<typename T>
class MaxFenwickTree2D {
private:
    vector<vector<T>> tree;
    int rows, cols;
    
    int lowbit(int x) {
        return x & (-x);
    }
    
public:
    // 构造函数
    MaxFenwickTree2D(int r, int c, T init_val = T{}) : rows(r), cols(c) {
        if (r <= 0 || c <= 0) {
            throw invalid_argument("Dimensions must be positive");
        }
        tree.assign(r + 1, vector<T>(c + 1, init_val));
    }
    
    // 单点更新（设置为更大值）
    void update(int r, int c, T val) {
        if (r < 1 || r > rows || c < 1 || c > cols) {
            throw out_of_range("Coordinates out of range");
        }
        
        for (int i = r; i <= rows; i += lowbit(i)) {
            for (int j = c; j <= cols; j += lowbit(j)) {
                tree[i][j] = max(tree[i][j], val);
            }
        }
    }
    
    // 矩形区域最大值查询（近似）
    T range_max(int r1, int c1, int r2, int c2) {
        if (r1 > r2 || c1 > c2) return T{};
        
        T max_val = T{};
        bool first = true;
        
        // 这是一个简化的实现，实际的2D最大值查询需要更复杂的数据结构
        for (int i = r1; i <= r2; ++i) {
            for (int j = c1; j <= c2; ++j) {
                T val = query(i, j);
                if (first || val > max_val) {
                    max_val = val;
                    first = false;
                }
            }
        }
        
        return max_val;
    }
    
    // 单点查询（近似）
    T query(int r, int c) {
        if (r < 1 || r > rows || c < 1 || c > cols) {
            return T{};
        }
        
        T max_val = T{};
        bool first = true;
        
        for (int i = r; i > 0; i -= lowbit(i)) {
            for (int j = c; j > 0; j -= lowbit(j)) {
                if (first || tree[i][j] > max_val) {
                    max_val = tree[i][j];
                    first = false;
                }
            }
        }
        
        return max_val;
    }
    
    // 获取维度
    pair<int, int> dimensions() const {
        return {rows, cols};
    }
};

// 类型别名
using IntFenwickTree2D = FenwickTree2D<int>;
using LongFenwickTree2D = FenwickTree2D<long long>;
using DoubleFenwickTree2D = FenwickTree2D<double>;

using IntDifferenceFenwickTree2D = DifferenceFenwickTree2D<int>;
using LongDifferenceFenwickTree2D = DifferenceFenwickTree2D<long long>;

using IntRangeFenwickTree2D = RangeFenwickTree2D<int>;
using LongRangeFenwickTree2D = RangeFenwickTree2D<long long>;

using IntMaxFenwickTree2D = MaxFenwickTree2D<int>;
using LongMaxFenwickTree2D = MaxFenwickTree2D<long long>;

// 实用函数

// 构建二维树状数组
template<typename T>
FenwickTree2D<T> build_fenwick_tree_2d(const vector<vector<T>>& matrix) {
    return FenwickTree2D<T>(matrix);
}

// 构建二维差分树状数组
template<typename T>
DifferenceFenwickTree2D<T> build_difference_fenwick_tree_2d(const vector<vector<T>>& matrix) {
    return DifferenceFenwickTree2D<T>(matrix);
}

// 构建二维范围树状数组
template<typename T>
RangeFenwickTree2D<T> build_range_fenwick_tree_2d(int rows, int cols) {
    return RangeFenwickTree2D<T>(rows, cols);
}

// 矩阵操作辅助函数

// 矩阵转置
template<typename T>
vector<vector<T>> transpose_matrix(const vector<vector<T>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
        return {};
    }
    
    int rows = matrix.size();
    int cols = matrix[0].size();
    vector<vector<T>> transposed(cols, vector<T>(rows));
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            transposed[j][i] = matrix[i][j];
        }
    }
    
    return transposed;
}

// 矩阵旋转90度
template<typename T>
vector<vector<T>> rotate_matrix_90(const vector<vector<T>>& matrix) {
    if (matrix.empty() || matrix[0].empty()) {
        return {};
    }
    
    int rows = matrix.size();
    int cols = matrix[0].size();
    vector<vector<T>> rotated(cols, vector<T>(rows));
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            rotated[j][rows - 1 - i] = matrix[i][j];
        }
    }
    
    return rotated;
}

// 生成随机矩阵
template<typename T>
vector<vector<T>> generate_random_matrix(int rows, int cols, T min_val, T max_val) {
    vector<vector<T>> matrix(rows, vector<T>(cols));
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = min_val + rand() % (max_val - min_val + 1);
        }
    }
    
    return matrix;
}

#endif // FENWICK_TREE_2D_H