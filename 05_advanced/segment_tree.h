#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <climits>
#include <cassert>
#include <memory>

namespace segment_tree {

// 基础线段树模板类
template<typename T>
class SegmentTree {
private:
    std::vector<T> tree;
    std::vector<T> arr;
    int n;
    std::function<T(T, T)> combine;  // 合并函数
    T identity;  // 单位元
    
    void build(int node, int start, int end) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            int mid = (start + end) / 2;
            build(2 * node, start, mid);
            build(2 * node + 1, mid + 1, end);
            tree[node] = combine(tree[2 * node], tree[2 * node + 1]);
        }
    }
    
    void update_helper(int node, int start, int end, int idx, T val) {
        if (start == end) {
            arr[idx] = val;
            tree[node] = val;
        } else {
            int mid = (start + end) / 2;
            if (idx <= mid) {
                update_helper(2 * node, start, mid, idx, val);
            } else {
                update_helper(2 * node + 1, mid + 1, end, idx, val);
            }
            tree[node] = combine(tree[2 * node], tree[2 * node + 1]);
        }
    }
    
    T query_helper(int node, int start, int end, int l, int r) {
        if (r < start || end < l) {
            return identity;
        }
        if (l <= start && end <= r) {
            return tree[node];
        }
        int mid = (start + end) / 2;
        T left_result = query_helper(2 * node, start, mid, l, r);
        T right_result = query_helper(2 * node + 1, mid + 1, end, l, r);
        return combine(left_result, right_result);
    }
    
public:
    SegmentTree(const std::vector<T>& data, 
                std::function<T(T, T)> comb_func, 
                T id) 
        : arr(data), n(data.size()), combine(comb_func), identity(id) {
        tree.resize(4 * n);
        build(1, 0, n - 1);
    }
    
    // 单点更新
    void update(int idx, T val) {
        update_helper(1, 0, n - 1, idx, val);
    }
    
    // 区间查询
    T query(int l, int r) {
        return query_helper(1, 0, n - 1, l, r);
    }
    
    // 获取原数组
    const std::vector<T>& get_array() const {
        return arr;
    }
    
    // 打印线段树结构
    void print_tree() {
        std::cout << "线段树结构: ";
        for (int i = 1; i < std::min(static_cast<int>(tree.size()), 15); ++i) {
            std::cout << tree[i] << " ";
        }
        std::cout << "\n";
    }
};

// 懒惰传播线段树
template<typename T>
class LazySegmentTree {
private:
    std::vector<T> tree;
    std::vector<T> lazy;
    std::vector<T> arr;
    int n;
    std::function<T(T, T)> combine;
    std::function<T(T, T, int)> apply_lazy;  // 应用懒惰标记的函数
    T identity;
    T lazy_identity;
    
    void build(int node, int start, int end) {
        if (start == end) {
            tree[node] = arr[start];
        } else {
            int mid = (start + end) / 2;
            build(2 * node, start, mid);
            build(2 * node + 1, mid + 1, end);
            tree[node] = combine(tree[2 * node], tree[2 * node + 1]);
        }
    }
    
    void push(int node, int start, int end) {
        if (lazy[node] != lazy_identity) {
            tree[node] = apply_lazy(tree[node], lazy[node], end - start + 1);
            if (start != end) {
                lazy[2 * node] = combine(lazy[2 * node], lazy[node]);
                lazy[2 * node + 1] = combine(lazy[2 * node + 1], lazy[node]);
            }
            lazy[node] = lazy_identity;
        }
    }
    
    void update_range_helper(int node, int start, int end, int l, int r, T val) {
        push(node, start, end);
        if (start > r || end < l) {
            return;
        }
        if (start >= l && end <= r) {
            lazy[node] = combine(lazy[node], val);
            push(node, start, end);
            return;
        }
        int mid = (start + end) / 2;
        update_range_helper(2 * node, start, mid, l, r, val);
        update_range_helper(2 * node + 1, mid + 1, end, l, r, val);
        push(2 * node, start, mid);
        push(2 * node + 1, mid + 1, end);
        tree[node] = combine(tree[2 * node], tree[2 * node + 1]);
    }
    
    T query_helper(int node, int start, int end, int l, int r) {
        if (start > r || end < l) {
            return identity;
        }
        push(node, start, end);
        if (start >= l && end <= r) {
            return tree[node];
        }
        int mid = (start + end) / 2;
        T left_result = query_helper(2 * node, start, mid, l, r);
        T right_result = query_helper(2 * node + 1, mid + 1, end, l, r);
        return combine(left_result, right_result);
    }
    
public:
    LazySegmentTree(const std::vector<T>& data,
                    std::function<T(T, T)> comb_func,
                    std::function<T(T, T, int)> apply_func,
                    T id, T lazy_id)
        : arr(data), n(data.size()), combine(comb_func), 
          apply_lazy(apply_func), identity(id), lazy_identity(lazy_id) {
        tree.resize(4 * n);
        lazy.resize(4 * n, lazy_identity);
        build(1, 0, n - 1);
    }
    
    // 区间更新
    void update_range(int l, int r, T val) {
        update_range_helper(1, 0, n - 1, l, r, val);
    }
    
    // 区间查询
    T query(int l, int r) {
        return query_helper(1, 0, n - 1, l, r);
    }
    
    // 单点查询
    T query_point(int idx) {
        return query(idx, idx);
    }
};

// 动态开点线段树
template<typename T>
class DynamicSegmentTree {
private:
    struct Node {
        T value;
        std::shared_ptr<Node> left, right;
        Node(T val = T{}) : value(val), left(nullptr), right(nullptr) {}
    };
    
    std::shared_ptr<Node> root;
    int min_range, max_range;
    std::function<T(T, T)> combine;
    T identity;
    
    void update_helper(std::shared_ptr<Node>& node, int start, int end, int idx, T val) {
        if (!node) {
            node = std::make_shared<Node>(identity);
        }
        
        if (start == end) {
            node->value = val;
            return;
        }
        
        int mid = start + (end - start) / 2;
        if (idx <= mid) {
            update_helper(node->left, start, mid, idx, val);
        } else {
            update_helper(node->right, mid + 1, end, idx, val);
        }
        
        T left_val = node->left ? node->left->value : identity;
        T right_val = node->right ? node->right->value : identity;
        node->value = combine(left_val, right_val);
    }
    
    T query_helper(std::shared_ptr<Node> node, int start, int end, int l, int r) {
        if (!node || start > r || end < l) {
            return identity;
        }
        
        if (start >= l && end <= r) {
            return node->value;
        }
        
        int mid = start + (end - start) / 2;
        T left_result = query_helper(node->left, start, mid, l, r);
        T right_result = query_helper(node->right, mid + 1, end, l, r);
        return combine(left_result, right_result);
    }
    
public:
    DynamicSegmentTree(int min_r, int max_r, 
                      std::function<T(T, T)> comb_func, T id)
        : min_range(min_r), max_range(max_r), combine(comb_func), identity(id) {
        root = nullptr;
    }
    
    void update(int idx, T val) {
        update_helper(root, min_range, max_range, idx, val);
    }
    
    T query(int l, int r) {
        return query_helper(root, min_range, max_range, l, r);
    }
};

// 可持久化线段树(主席树)
template<typename T>
class PersistentSegmentTree {
private:
    struct Node {
        T value;
        std::shared_ptr<Node> left, right;
        Node(T val = T{}) : value(val), left(nullptr), right(nullptr) {}
        Node(const Node& other) : value(other.value), left(other.left), right(other.right) {}
    };
    
    std::vector<std::shared_ptr<Node>> roots;
    int n;
    std::function<T(T, T)> combine;
    T identity;
    
    std::shared_ptr<Node> build(const std::vector<T>& arr, int start, int end) {
        auto node = std::make_shared<Node>();
        if (start == end) {
            node->value = arr[start];
        } else {
            int mid = (start + end) / 2;
            node->left = build(arr, start, mid);
            node->right = build(arr, mid + 1, end);
            node->value = combine(node->left->value, node->right->value);
        }
        return node;
    }
    
    std::shared_ptr<Node> update_helper(std::shared_ptr<Node> node, 
                                       int start, int end, int idx, T val) {
        auto new_node = std::make_shared<Node>(*node);
        
        if (start == end) {
            new_node->value = val;
        } else {
            int mid = (start + end) / 2;
            if (idx <= mid) {
                new_node->left = update_helper(node->left, start, mid, idx, val);
            } else {
                new_node->right = update_helper(node->right, mid + 1, end, idx, val);
            }
            new_node->value = combine(new_node->left->value, new_node->right->value);
        }
        
        return new_node;
    }
    
    T query_helper(std::shared_ptr<Node> node, int start, int end, int l, int r) {
        if (!node || start > r || end < l) {
            return identity;
        }
        
        if (start >= l && end <= r) {
            return node->value;
        }
        
        int mid = (start + end) / 2;
        T left_result = query_helper(node->left, start, mid, l, r);
        T right_result = query_helper(node->right, mid + 1, end, l, r);
        return combine(left_result, right_result);
    }
    
public:
    PersistentSegmentTree(const std::vector<T>& data,
                         std::function<T(T, T)> comb_func, T id)
        : n(data.size()), combine(comb_func), identity(id) {
        roots.push_back(build(data, 0, n - 1));
    }
    
    // 创建新版本
    int update(int version, int idx, T val) {
        auto new_root = update_helper(roots[version], 0, n - 1, idx, val);
        roots.push_back(new_root);
        return roots.size() - 1;
    }
    
    // 查询指定版本
    T query(int version, int l, int r) {
        return query_helper(roots[version], 0, n - 1, l, r);
    }
    
    // 获取版本数量
    int get_version_count() const {
        return roots.size();
    }
};

// 二维线段树
template<typename T>
class SegmentTree2D {
private:
    std::vector<std::vector<T>> tree;
    std::vector<std::vector<T>> matrix;
    int n, m;
    std::function<T(T, T)> combine;
    T identity;
    
    void build_y(int vx, int lx, int rx, int vy, int ly, int ry) {
        if (ly == ry) {
            if (lx == rx) {
                tree[vx][vy] = matrix[lx][ly];
            } else {
                tree[vx][vy] = combine(tree[2*vx][vy], tree[2*vx+1][vy]);
            }
        } else {
            int my = (ly + ry) / 2;
            build_y(vx, lx, rx, 2*vy, ly, my);
            build_y(vx, lx, rx, 2*vy+1, my+1, ry);
            tree[vx][vy] = combine(tree[vx][2*vy], tree[vx][2*vy+1]);
        }
    }
    
    void build_x(int vx, int lx, int rx) {
        if (lx != rx) {
            int mx = (lx + rx) / 2;
            build_x(2*vx, lx, mx);
            build_x(2*vx+1, mx+1, rx);
        }
        build_y(vx, lx, rx, 1, 0, m-1);
    }
    
    void update_y(int vx, int lx, int rx, int vy, int ly, int ry, int x, int y, T val) {
        if (ly == ry) {
            if (lx == rx) {
                tree[vx][vy] = val;
            } else {
                tree[vx][vy] = combine(tree[2*vx][vy], tree[2*vx+1][vy]);
            }
        } else {
            int my = (ly + ry) / 2;
            if (y <= my) {
                update_y(vx, lx, rx, 2*vy, ly, my, x, y, val);
            } else {
                update_y(vx, lx, rx, 2*vy+1, my+1, ry, x, y, val);
            }
            tree[vx][vy] = combine(tree[vx][2*vy], tree[vx][2*vy+1]);
        }
    }
    
    void update_x(int vx, int lx, int rx, int x, int y, T val) {
        if (lx != rx) {
            int mx = (lx + rx) / 2;
            if (x <= mx) {
                update_x(2*vx, lx, mx, x, y, val);
            } else {
                update_x(2*vx+1, mx+1, rx, x, y, val);
            }
        }
        update_y(vx, lx, rx, 1, 0, m-1, x, y, val);
    }
    
    T query_y(int vx, int vy, int ly, int ry, int y1, int y2) {
        if (y1 > y2) return identity;
        if (y1 == ly && y2 == ry) {
            return tree[vx][vy];
        }
        int my = (ly + ry) / 2;
        return combine(
            query_y(vx, 2*vy, ly, my, y1, std::min(y2, my)),
            query_y(vx, 2*vy+1, my+1, ry, std::max(y1, my+1), y2)
        );
    }
    
    T query_x(int vx, int lx, int rx, int x1, int x2, int y1, int y2) {
        if (x1 > x2) return identity;
        if (x1 == lx && x2 == rx) {
            return query_y(vx, 1, 0, m-1, y1, y2);
        }
        int mx = (lx + rx) / 2;
        return combine(
            query_x(2*vx, lx, mx, x1, std::min(x2, mx), y1, y2),
            query_x(2*vx+1, mx+1, rx, std::max(x1, mx+1), x2, y1, y2)
        );
    }
    
public:
    SegmentTree2D(const std::vector<std::vector<T>>& data,
                  std::function<T(T, T)> comb_func, T id)
        : matrix(data), n(data.size()), m(data[0].size()), 
          combine(comb_func), identity(id) {
        tree.resize(4 * n);
        for (int i = 0; i < 4 * n; ++i) {
            tree[i].resize(4 * m);
        }
        build_x(1, 0, n-1);
    }
    
    void update(int x, int y, T val) {
        matrix[x][y] = val;
        update_x(1, 0, n-1, x, y, val);
    }
    
    T query(int x1, int y1, int x2, int y2) {
        return query_x(1, 0, n-1, x1, x2, y1, y2);
    }
};

// 线段树应用类
class SegmentTreeApplications {
public:
    // RMQ问题(区间最值查询)
    template<typename T>
    static auto create_rmq_tree(const std::vector<T>& data, bool find_min = true) {
        if (find_min) {
            return SegmentTree<T>(data, 
                [](T a, T b) { return std::min(a, b); },
                std::numeric_limits<T>::max());
        } else {
            return SegmentTree<T>(data,
                [](T a, T b) { return std::max(a, b); },
                std::numeric_limits<T>::min());
        }
    }
    
    // 区间和查询
    template<typename T>
    static auto create_sum_tree(const std::vector<T>& data) {
        return SegmentTree<T>(data,
            [](T a, T b) { return a + b; },
            T{});
    }
    
    // 区间加法更新的懒惰线段树
    template<typename T>
    static auto create_range_add_tree(const std::vector<T>& data) {
        return LazySegmentTree<T>(data,
            [](T a, T b) { return a + b; },  // 合并函数
            [](T node_val, T lazy_val, int len) { return node_val + lazy_val * len; },  // 应用懒惰标记
            T{}, T{});
    }
    
    // 区间赋值更新的懒惰线段树
    template<typename T>
    static auto create_range_assign_tree(const std::vector<T>& data, T null_value) {
        return LazySegmentTree<T>(data,
            [](T a, T b) { return a + b; },
            [null_value](T node_val, T lazy_val, int len) { 
                return lazy_val == null_value ? node_val : lazy_val * len; 
            },
            T{}, null_value);
    }
    
    // 逆序对统计
    static long long count_inversions(std::vector<int>& arr) {
        // 离散化
        std::vector<int> sorted_arr = arr;
        std::sort(sorted_arr.begin(), sorted_arr.end());
        sorted_arr.erase(std::unique(sorted_arr.begin(), sorted_arr.end()), sorted_arr.end());
        
        int n = arr.size();
        int m = sorted_arr.size();
        
        // 创建计数线段树
        std::vector<int> count(m, 0);
        auto tree = create_sum_tree(count);
        
        long long inversions = 0;
        
        for (int i = n - 1; i >= 0; --i) {
            int pos = std::lower_bound(sorted_arr.begin(), sorted_arr.end(), arr[i]) - sorted_arr.begin();
            
            // 查询比当前元素小的元素个数
            if (pos > 0) {
                inversions += tree.query(0, pos - 1);
            }
            
            // 更新计数
            tree.update(pos, tree.query(pos, pos) + 1);
        }
        
        return inversions;
    }
};

// 性能测试类
class SegmentTreeTester {
public:
    template<typename T>
    static void performance_test(int n, int queries) {
        std::cout << "\n线段树性能测试 (n=" << n << ", queries=" << queries << ")\n";
        
        // 生成测试数据
        std::vector<T> data(n);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<T> dis(1, 1000);
        
        for (int i = 0; i < n; ++i) {
            data[i] = dis(gen);
        }
        
        // 测试区间最小值查询
        auto start = std::chrono::high_resolution_clock::now();
        auto rmq_tree = SegmentTreeApplications::create_rmq_tree(data, true);
        
        std::uniform_int_distribution<int> idx_dis(0, n - 1);
        
        for (int i = 0; i < queries; ++i) {
            int l = idx_dis(gen);
            int r = idx_dis(gen);
            if (l > r) std::swap(l, r);
            rmq_tree.query(l, r);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "RMQ查询用时: " << duration.count() << " 微秒\n";
        
        // 测试更新操作
        start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < queries; ++i) {
            int idx = idx_dis(gen);
            T val = dis(gen);
            rmq_tree.update(idx, val);
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "更新操作用时: " << duration.count() << " 微秒\n";
    }
    
    static void correctness_test() {
        std::cout << "\n线段树正确性测试\n";
        
        std::vector<int> data = {1, 3, 5, 7, 9, 11};
        auto sum_tree = SegmentTreeApplications::create_sum_tree(data);
        
        // 测试区间和查询
        assert(sum_tree.query(0, 2) == 9);  // 1 + 3 + 5
        assert(sum_tree.query(1, 4) == 24); // 3 + 5 + 7 + 9
        assert(sum_tree.query(0, 5) == 36); // 全部元素和
        
        // 测试更新
        sum_tree.update(2, 10);  // 将索引2的值从5改为10
        assert(sum_tree.query(0, 2) == 14); // 1 + 3 + 10
        assert(sum_tree.query(1, 4) == 29); // 3 + 10 + 7 + 9
        
        std::cout << "所有测试通过！\n";
    }
};

} // namespace segment_tree

#endif // SEGMENT_TREE_H