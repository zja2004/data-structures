#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <functional>

namespace union_find {

// 基础并查集实现
class UnionFind {
private:
    std::vector<int> parent;    // 父节点数组
    std::vector<int> rank;      // 秩数组(树的高度)
    std::vector<int> size;      // 集合大小数组
    int components;             // 连通分量数量
    
public:
    // 构造函数
    explicit UnionFind(int n) : parent(n), rank(n, 0), size(n, 1), components(n) {
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }
    
    // 查找操作(带路径压缩)
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);  // 路径压缩
        }
        return parent[x];
    }
    
    // 合并操作(按秩合并)
    bool union_sets(int x, int y) {
        int root_x = find(x);
        int root_y = find(y);
        
        if (root_x == root_y) {
            return false;  // 已经在同一集合中
        }
        
        // 按秩合并
        if (rank[root_x] < rank[root_y]) {
            parent[root_x] = root_y;
            size[root_y] += size[root_x];
        } else if (rank[root_x] > rank[root_y]) {
            parent[root_y] = root_x;
            size[root_x] += size[root_y];
        } else {
            parent[root_y] = root_x;
            size[root_x] += size[root_y];
            rank[root_x]++;
        }
        
        components--;
        return true;
    }
    
    // 判断是否连通
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
    
    // 获取集合大小
    int get_size(int x) {
        return size[find(x)];
    }
    
    // 获取连通分量数量
    int get_components() const {
        return components;
    }
    
    // 获取所有集合的代表元素
    std::vector<int> get_representatives() {
        std::vector<int> reps;
        for (int i = 0; i < parent.size(); ++i) {
            if (find(i) == i) {
                reps.push_back(i);
            }
        }
        return reps;
    }
    
    // 打印并查集状态
    void print_status() {
        std::cout << "并查集状态:\n";
        std::cout << "连通分量数: " << components << "\n";
        
        std::map<int, std::vector<int>> groups;
        for (int i = 0; i < parent.size(); ++i) {
            groups[find(i)].push_back(i);
        }
        
        int group_id = 1;
        for (const auto& group : groups) {
            std::cout << "组 " << group_id++ << " (代表: " << group.first << "): ";
            for (int member : group.second) {
                std::cout << member << " ";
            }
            std::cout << "(大小: " << group.second.size() << ")\n";
        }
    }
};

// 带权并查集(支持到根节点的距离)
class WeightedUnionFind {
private:
    std::vector<int> parent;
    std::vector<long long> weight;  // 到父节点的权重
    
public:
    explicit WeightedUnionFind(int n) : parent(n), weight(n, 0) {
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }
    
    // 查找根节点并更新权重
    int find(int x) {
        if (parent[x] != x) {
            int original_parent = parent[x];
            parent[x] = find(parent[x]);
            weight[x] += weight[original_parent];
        }
        return parent[x];
    }
    
    // 合并两个集合，指定x到y的权重为w
    bool union_sets(int x, int y, long long w) {
        int root_x = find(x);
        int root_y = find(y);
        
        if (root_x == root_y) {
            return weight[y] - weight[x] == w;  // 检查权重是否一致
        }
        
        parent[root_x] = root_y;
        weight[root_x] = weight[y] - weight[x] - w;
        return true;
    }
    
    // 获取x到y的权重差
    long long get_weight_diff(int x, int y) {
        if (find(x) != find(y)) {
            throw std::runtime_error("元素不在同一连通分量中");
        }
        return weight[y] - weight[x];
    }
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
};

// 可撤销并查集
class UndoableUnionFind {
private:
    struct Operation {
        int x, y;
        int old_parent_x, old_rank_x;
        int old_parent_y, old_rank_y;
        int old_components;
    };
    
    std::vector<int> parent;
    std::vector<int> rank;
    std::vector<Operation> history;
    int components;
    
public:
    explicit UndoableUnionFind(int n) : parent(n), rank(n, 0), components(n) {
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }
    
    int find(int x) {
        while (parent[x] != x) {
            x = parent[x];
        }
        return x;
    }
    
    bool union_sets(int x, int y) {
        int root_x = find(x);
        int root_y = find(y);
        
        Operation op = {x, y, parent[root_x], rank[root_x], 
                       parent[root_y], rank[root_y], components};
        history.push_back(op);
        
        if (root_x == root_y) {
            return false;
        }
        
        if (rank[root_x] < rank[root_y]) {
            parent[root_x] = root_y;
        } else if (rank[root_x] > rank[root_y]) {
            parent[root_y] = root_x;
        } else {
            parent[root_y] = root_x;
            rank[root_x]++;
        }
        
        components--;
        return true;
    }
    
    void undo() {
        if (history.empty()) {
            throw std::runtime_error("没有可撤销的操作");
        }
        
        Operation op = history.back();
        history.pop_back();
        
        int root_x = find(op.x);
        int root_y = find(op.y);
        
        parent[root_x] = op.old_parent_x;
        rank[root_x] = op.old_rank_x;
        parent[root_y] = op.old_parent_y;
        rank[root_y] = op.old_rank_y;
        components = op.old_components;
    }
    
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
    
    int get_components() const {
        return components;
    }
    
    size_t get_history_size() const {
        return history.size();
    }
};

// 动态并查集(支持动态添加节点)
template<typename T>
class DynamicUnionFind {
private:
    std::unordered_map<T, T> parent;
    std::unordered_map<T, int> rank;
    std::unordered_map<T, int> size;
    int components;
    
public:
    DynamicUnionFind() : components(0) {}
    
    void add_element(const T& x) {
        if (parent.find(x) == parent.end()) {
            parent[x] = x;
            rank[x] = 0;
            size[x] = 1;
            components++;
        }
    }
    
    T find(const T& x) {
        if (parent.find(x) == parent.end()) {
            add_element(x);
        }
        
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    
    bool union_sets(const T& x, const T& y) {
        T root_x = find(x);
        T root_y = find(y);
        
        if (root_x == root_y) {
            return false;
        }
        
        if (rank[root_x] < rank[root_y]) {
            parent[root_x] = root_y;
            size[root_y] += size[root_x];
        } else if (rank[root_x] > rank[root_y]) {
            parent[root_y] = root_x;
            size[root_x] += size[root_y];
        } else {
            parent[root_y] = root_x;
            size[root_x] += size[root_y];
            rank[root_x]++;
        }
        
        components--;
        return true;
    }
    
    bool connected(const T& x, const T& y) {
        return find(x) == find(y);
    }
    
    int get_size(const T& x) {
        return size[find(x)];
    }
    
    int get_components() const {
        return components;
    }
    
    std::vector<T> get_all_elements() {
        std::vector<T> elements;
        for (const auto& pair : parent) {
            elements.push_back(pair.first);
        }
        return elements;
    }
};

// 并查集应用类
class UnionFindApplications {
public:
    // Kruskal最小生成树算法
    struct Edge {
        int u, v, weight;
        bool operator<(const Edge& other) const {
            return weight < other.weight;
        }
    };
    
    static std::vector<Edge> kruskal_mst(int n, std::vector<Edge>& edges) {
        std::sort(edges.begin(), edges.end());
        UnionFind uf(n);
        std::vector<Edge> mst;
        
        for (const Edge& edge : edges) {
            if (uf.union_sets(edge.u, edge.v)) {
                mst.push_back(edge);
                if (mst.size() == n - 1) {
                    break;
                }
            }
        }
        
        return mst;
    }
    
    // 判断图是否连通
    static bool is_graph_connected(int n, const std::vector<std::pair<int, int>>& edges) {
        UnionFind uf(n);
        for (const auto& edge : edges) {
            uf.union_sets(edge.first, edge.second);
        }
        return uf.get_components() == 1;
    }
    
    // 计算添加边后的连通分量数量
    static std::vector<int> count_components_after_edges(
        int n, const std::vector<std::pair<int, int>>& edges) {
        UnionFind uf(n);
        std::vector<int> result;
        
        for (const auto& edge : edges) {
            uf.union_sets(edge.first, edge.second);
            result.push_back(uf.get_components());
        }
        
        return result;
    }
    
    // 朋友圈问题
    static int find_circle_num(const std::vector<std::vector<int>>& is_connected) {
        int n = is_connected.size();
        UnionFind uf(n);
        
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (is_connected[i][j] == 1) {
                    uf.union_sets(i, j);
                }
            }
        }
        
        return uf.get_components();
    }
    
    // 岛屿数量问题
    static int num_islands(std::vector<std::vector<char>>& grid) {
        if (grid.empty() || grid[0].empty()) return 0;
        
        int m = grid.size(), n = grid[0].size();
        UnionFind uf(m * n);
        int water_count = 0;
        
        auto get_index = [n](int i, int j) { return i * n + j; };
        
        std::vector<std::pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
        
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] == '0') {
                    water_count++;
                    continue;
                }
                
                for (const auto& dir : directions) {
                    int ni = i + dir.first;
                    int nj = j + dir.second;
                    
                    if (ni >= 0 && ni < m && nj >= 0 && nj < n && grid[ni][nj] == '1') {
                        uf.union_sets(get_index(i, j), get_index(ni, nj));
                    }
                }
            }
        }
        
        return uf.get_components() - water_count;
    }
};

// 性能测试类
class UnionFindTester {
public:
    static void performance_test(int n, int operations) {
        std::cout << "\n并查集性能测试 (n=" << n << ", operations=" << operations << ")\n";
        
        // 测试基础并查集
        auto start = std::chrono::high_resolution_clock::now();
        UnionFind uf(n);
        
        // 随机合并操作
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, n - 1);
        
        for (int i = 0; i < operations; ++i) {
            int x = dis(gen);
            int y = dis(gen);
            uf.union_sets(x, y);
        }
        
        // 随机查询操作
        for (int i = 0; i < operations; ++i) {
            int x = dis(gen);
            int y = dis(gen);
            uf.connected(x, y);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "基础并查集用时: " << duration.count() << " 微秒\n";
        std::cout << "最终连通分量数: " << uf.get_components() << "\n";
    }
    
    static void correctness_test() {
        std::cout << "\n并查集正确性测试\n";
        
        UnionFind uf(10);
        
        // 测试初始状态
        assert(uf.get_components() == 10);
        assert(!uf.connected(0, 1));
        
        // 测试合并操作
        uf.union_sets(0, 1);
        assert(uf.connected(0, 1));
        assert(uf.get_components() == 9);
        
        uf.union_sets(2, 3);
        uf.union_sets(1, 3);
        assert(uf.connected(0, 2));
        assert(uf.get_components() == 7);
        
        // 测试集合大小
        assert(uf.get_size(0) == 4);
        assert(uf.get_size(4) == 1);
        
        std::cout << "所有测试通过！\n";
    }
};

} // namespace union_find

#endif // UNION_FIND_H