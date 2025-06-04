# 树状数组 (Fenwick Tree / Binary Indexed Tree)

## 概述

树状数组（Fenwick Tree），也称为二进制索引树（Binary Indexed Tree，BIT），是一种支持高效单点更新和前缀和查询的数据结构。它由Peter Fenwick在1994年提出，是线段树的一个轻量级替代方案。

## 核心特性

### 1. 树形结构
- **二进制表示**: 基于数组索引的二进制表示
- **父子关系**: 通过位运算确定节点关系
- **lowbit操作**: `x & (-x)` 获取最低位1及其后面的0
- **空间效率**: 只需要O(n)的额外空间

### 2. 前缀和维护
- **快速查询**: O(log n)时间内计算前缀和
- **增量更新**: O(log n)时间内更新单个元素
- **区间查询**: 通过前缀和差值计算区间和
- **批量操作**: 支持批量更新和查询

### 3. 位运算优化
- **lowbit函数**: 核心位运算操作
- **索引映射**: 1-based索引系统
- **路径压缩**: 通过位运算快速跳转
- **缓存友好**: 连续内存访问模式

## 主要优势

1. **实现简单**: 代码简洁，易于理解和实现
2. **空间高效**: 相比线段树节省一半空间
3. **常数优化**: 位运算带来的性能优势
4. **扩展性强**: 容易扩展到多维和其他操作

## 树状数组类型

### 1. 基础树状数组
- **单点更新**: 修改单个元素值
- **前缀和查询**: 计算[1, i]的和
- **区间和查询**: 计算[l, r]的和
- **应用场景**: 动态前缀和问题

### 2. 差分树状数组
- **区间更新**: 对区间内所有元素加值
- **单点查询**: 查询单个元素值
- **差分思想**: 维护差分数组的前缀和
- **应用场景**: 区间修改，单点查询

### 3. 双树状数组
- **区间更新**: 对区间内所有元素加值
- **区间查询**: 查询区间和
- **双重维护**: 同时维护两个树状数组
- **应用场景**: 区间修改，区间查询

### 4. 二维树状数组
- **矩形更新**: 修改矩形区域内的值
- **矩形查询**: 查询矩形区域的和
- **二维扩展**: 将一维操作扩展到二维
- **应用场景**: 二维前缀和问题

## 基本操作

### 核心操作
```cpp
// lowbit操作
int lowbit(int x) { return x & (-x); }

// 单点更新
void update(int i, int delta) {
    for (; i <= n; i += lowbit(i)) {
        tree[i] += delta;
    }
}

// 前缀和查询
int query(int i) {
    int sum = 0;
    for (; i > 0; i -= lowbit(i)) {
        sum += tree[i];
    }
    return sum;
}

// 区间和查询
int range_query(int l, int r) {
    return query(r) - query(l - 1);
}
```

### 高级操作
- **二分查找**: 在树状数组上进行二分查找
- **第k小元素**: 结合离散化和二分查找
- **逆序对计数**: 经典应用场景
- **动态中位数**: 维护动态数据集的中位数

## 实现方法

### 1. 数组实现
```cpp
class FenwickTree {
private:
    vector<int> tree;
    int n;
    
public:
    FenwickTree(int size) : n(size), tree(size + 1, 0) {}
    
    void update(int i, int delta) {
        for (; i <= n; i += i & (-i)) {
            tree[i] += delta;
        }
    }
    
    int query(int i) {
        int sum = 0;
        for (; i > 0; i -= i & (-i)) {
            sum += tree[i];
        }
        return sum;
    }
};
```

### 2. 模板实现
```cpp
template<typename T>
class FenwickTree {
private:
    vector<T> tree;
    int n;
    function<T(T, T)> combine;
    T identity;
    
public:
    FenwickTree(int size, function<T(T, T)> op, T id)
        : n(size), tree(size + 1, id), combine(op), identity(id) {}
};
```

## 应用场景

### 1. 动态前缀和
- **股票价格**: 计算价格变化的累积效应
- **销售统计**: 动态计算销售额前缀和
- **得分统计**: 实时更新和查询得分排名
- **资源分配**: 动态资源使用情况统计

### 2. 逆序对问题
- **数组逆序对**: 计算数组中的逆序对数量
- **归并排序**: 在归并过程中统计逆序对
- **排列分析**: 分析排列的逆序程度
- **数据分析**: 衡量数据的有序程度

### 3. 区间统计
- **频率统计**: 动态维护元素出现频率
- **范围查询**: 查询指定范围内的统计信息
- **滑动窗口**: 维护滑动窗口内的统计值
- **时间序列**: 时间序列数据的区间分析

### 4. 几何问题
- **二维前缀和**: 矩形区域和查询
- **点更新**: 动态添加或删除点
- **范围计数**: 统计指定区域内的点数
- **最近邻查询**: 结合其他数据结构的几何查询

## 经典算法问题

### 基础问题
1. **动态前缀和**: 支持单点更新的前缀和查询
2. **区间和查询**: 计算任意区间的元素和
3. **逆序对计数**: 统计数组中的逆序对数量
4. **频率统计**: 动态维护元素出现次数

### 进阶问题
1. **第k小元素**: 在动态数组中查找第k小元素
2. **区间更新**: 支持区间加法的树状数组
3. **二维前缀和**: 二维数组的动态前缀和
4. **最大子数组**: 结合其他技巧解决复杂问题

### 高级问题
1. **持久化树状数组**: 支持历史版本查询
2. **可撤销操作**: 支持操作回滚的树状数组
3. **并行树状数组**: 多线程环境下的树状数组
4. **分布式统计**: 分布式系统中的统计问题

## 变种与扩展

### 1. 多维树状数组
- **二维BIT**: 支持矩形区域操作
- **三维BIT**: 支持立体区域操作
- **高维扩展**: 任意维度的扩展
- **空间优化**: 稀疏高维数组的优化

### 2. 功能扩展
- **最值查询**: 支持区间最大值/最小值查询
- **异或操作**: 支持异或运算的树状数组
- **乘法操作**: 支持乘法运算的树状数组
- **自定义操作**: 支持任意可逆操作

### 3. 性能优化
- **缓存优化**: 提高缓存命中率
- **SIMD优化**: 利用向量指令加速
- **并行化**: 多线程并行处理
- **内存池**: 减少内存分配开销

## 时间复杂度

| 操作 | 时间复杂度 | 空间复杂度 | 备注 |
|------|------------|------------|------|
| 构建 | O(n log n) | O(n) | 逐个插入元素 |
| 单点更新 | O(log n) | O(1) | 位运算路径 |
| 前缀和查询 | O(log n) | O(1) | 位运算路径 |
| 区间和查询 | O(log n) | O(1) | 两次前缀和 |
| 二分查找 | O(log² n) | O(1) | 在BIT上二分 |
| 区间更新 | O(log n) | O(1) | 差分BIT |

## 空间复杂度

| 类型 | 空间复杂度 | 说明 |
|------|------------|------|
| 一维BIT | O(n) | 基础树状数组 |
| 二维BIT | O(n²) | 二维扩展 |
| 差分BIT | O(n) | 支持区间更新 |
| 双BIT | O(n) | 区间更新+区间查询 |

## 文件结构

```
fenwick_tree/
├── README.md                 # 本文档
├── fenwick_tree.h           # 基础树状数组实现
├── advanced_fenwick_tree.h  # 高级树状数组实现
├── multi_dimensional_bit.h  # 多维树状数组实现
└── example.cpp             # 使用示例和测试
```

## 使用示例

### 基础使用
```cpp
#include "fenwick_tree.h"

int main() {
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    FenwickTree<int> bit(arr);
    
    // 前缀和查询
    cout << bit.prefix_sum(3) << endl;  // 1+3+5 = 9
    
    // 区间和查询
    cout << bit.range_sum(2, 4) << endl;  // 3+5+7 = 15
    
    // 单点更新
    bit.update(2, 2);  // arr[2] += 2, 即 5 -> 7
    cout << bit.range_sum(2, 4) << endl;  // 3+7+7 = 17
    
    return 0;
}
```

### 高级应用
```cpp
// 逆序对计数
int count_inversions(vector<int>& arr) {
    // 离散化
    auto sorted = arr;
    sort(sorted.begin(), sorted.end());
    sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());
    
    FenwickTree<int> bit(sorted.size());
    int inversions = 0;
    
    for (int i = arr.size() - 1; i >= 0; i--) {
        int pos = lower_bound(sorted.begin(), sorted.end(), arr[i]) - sorted.begin();
        inversions += bit.prefix_sum(pos);
        bit.update(pos + 1, 1);
    }
    
    return inversions;
}
```

## 编译和运行

```bash
# 编译示例
g++ -std=c++17 -O2 example.cpp -o example

# 运行示例
./example

# 编译时启用调试
g++ -std=c++17 -g -DDEBUG example.cpp -o example_debug
```

## 练习题目

### 初级练习
1. **基础操作**: 实现基本的单点更新和前缀和查询
2. **区间查询**: 实现区间和查询功能
3. **数组构建**: 从给定数组构建树状数组
4. **逆序对**: 使用树状数组计算逆序对数量

### 中级练习
1. **区间更新**: 实现支持区间更新的树状数组
2. **第k小**: 在动态数组中查找第k小元素
3. **二维BIT**: 实现二维树状数组
4. **最值查询**: 扩展树状数组支持最值查询

### 高级练习
1. **可撤销BIT**: 实现支持操作撤销的树状数组
2. **持久化BIT**: 实现可持久化树状数组
3. **并行BIT**: 实现线程安全的树状数组
4. **分布式统计**: 设计分布式环境下的统计系统

## 调试技巧

### 1. 可视化调试
```cpp
void print_tree_structure() {
    cout << "Tree structure:" << endl;
    for (int i = 1; i <= n; i++) {
        cout << "tree[" << i << "] = " << tree[i] 
             << " (covers " << (i - lowbit(i) + 1) << " to " << i << ")" << endl;
    }
}
```

### 2. 边界检查
```cpp
void check_bounds(int index) {
    if (index < 1 || index > n) {
        throw out_of_range("Index out of bounds");
    }
}
```

### 3. 性能分析
```cpp
struct BITPerformance {
    int update_count = 0;
    int query_count = 0;
    double avg_update_time = 0.0;
    double avg_query_time = 0.0;
};
```

## 学习建议

### 1. 理论基础
- 深入理解lowbit操作的原理
- 掌握树状数组的树形结构
- 理解前缀和的数学性质
- 学习位运算的优化技巧

### 2. 实践步骤
1. 从基础的一维树状数组开始
2. 练习经典的逆序对问题
3. 扩展到区间更新操作
4. 尝试二维树状数组
5. 探索高级应用和优化

### 3. 进阶方向
- 学习线段树，对比两种数据结构
- 研究可持久化数据结构
- 探索并行和分布式算法
- 深入学习位运算优化技巧

## 参考资料

### 经典论文
- Fenwick, Peter M. "A new data structure for cumulative frequency tables." (1994)
- "Binary Indexed Trees" - TopCoder Tutorial
- "Fenwick Tree" - CP-Algorithms

### 在线资源
- [OI Wiki - 树状数组](https://oi-wiki.org/ds/fenwick/)
- [GeeksforGeeks - Binary Indexed Tree](https://www.geeksforgeeks.org/binary-indexed-tree-or-fenwick-tree-2/)
- [Codeforces - Fenwick Tree Tutorial](https://codeforces.com/blog/entry/61364)

### 相关算法
- 线段树 (Segment Tree)
- 平方分割 (Square Root Decomposition)
- 莫队算法 (Mo's Algorithm)
- 可持久化数据结构 (Persistent Data Structures)