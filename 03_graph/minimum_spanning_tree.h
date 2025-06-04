#ifndef MINIMUM_SPANNING_TREE_H
#define MINIMUM_SPANNING_TREE_H

#include "graph_representation.h"
#include <vector>
#include <algorithm>
#include <queue>
#include <numeric>
#include <functional>

using namespace std;

// 并查集（Union-Find）数据结构
class UnionFind {
private:
    vector<int> parent;
    vector<int> rank;
    int components;
    
public:
    UnionFind(int n) : parent(n), rank(n, 0), components(n) {
        iota(parent.begin(), parent.end(), 0);
    }
    
    // 查找根节点（路径压缩）
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    
    // 合并两个集合（按秩合并）
    bool unite(int x, int y) {
        int root_x = find(x);
        int root_y = find(y);
        
        if (root_x == root_y) {
            return false; // 已经在同一集合中
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
    
    // 检查两个元素是否在同一集合中
    bool connected(int x, int y) {
        return find(x) == find(y);
    }
    
    // 获取连通分量数
    int get_components() const {
        return components;
    }
};

// 最小生成树结果
template<typename T>
struct MST_Result {
    vector<Edge<T>> edges;         // MST的边
    T total_weight;                // 总权重
    bool is_connected;             // 图是否连通
    
    MST_Result() : total_weight(T{}), is_connected(false) {}
    
    // 添加边
    void add_edge(const Edge<T>& edge) {
        edges.push_back(edge);
        total_weight += edge.weight;
    }
    
    // 获取边数
    int edge_count() const {
        return edges.size();
    }
    
    // 打印MST
    void print() const {
        cout << "Minimum Spanning Tree:" << endl;
        if (!is_connected) {
            cout << "Graph is not connected!" << endl;
            return;
        }
        
        cout << "Edges:" << endl;
        for (const auto& edge : edges) {
            cout << edge.from << " - " << edge.to << " : " << edge.weight << endl;
        }
        cout << "Total weight: " << total_weight << endl;
        cout << "Number of edges: " << edges.size() << endl;
    }
};

// Kruskal算法实现
template<typename T>
class KruskalAlgorithm {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    KruskalAlgorithm(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 计算最小生成树
    MST_Result<T> find_mst() const {
        MST_Result<T> result;
        int vertices = graph.get_vertex_count();
        
        if (vertices == 0) {
            result.is_connected = true;
            return result;
        }
        
        // 获取所有边并排序
        vector<Edge<T>> edges = graph.get_edges();
        sort(edges.begin(), edges.end());
        
        // 初始化并查集
        UnionFind uf(vertices);
        
        // Kruskal算法主循环
        for (const auto& edge : edges) {
            if (uf.unite(edge.from, edge.to)) {
                result.add_edge(edge);
                
                // 如果已经有V-1条边，MST完成
                if (result.edge_count() == vertices - 1) {
                    break;
                }
            }
        }
        
        // 检查是否连通
        result.is_connected = (result.edge_count() == vertices - 1);
        
        return result;
    }
    
    // 检查图是否连通
    bool is_connected() const {
        auto mst = find_mst();
        return mst.is_connected;
    }
};

// Prim算法实现
template<typename T>
class PrimAlgorithm {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    PrimAlgorithm(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 计算最小生成树
    MST_Result<T> find_mst(int start = 0) const {
        MST_Result<T> result;
        int vertices = graph.get_vertex_count();
        
        if (vertices == 0) {
            result.is_connected = true;
            return result;
        }
        
        if (start < 0 || start >= vertices) {
            start = 0;
        }
        
        vector<bool> in_mst(vertices, false);
        vector<T> key(vertices, numeric_limits<T>::max());
        vector<int> parent(vertices, -1);
        
        // 优先队列：(权重, 顶点)
        priority_queue<pair<T, int>, vector<pair<T, int>>, greater<pair<T, int>>> pq;
        
        // 从起始顶点开始
        key[start] = T{};
        pq.push({T{}, start});
        
        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();
            
            if (in_mst[u]) continue;
            
            in_mst[u] = true;
            
            // 如果不是起始顶点，添加边到MST
            if (parent[u] != -1) {
                result.add_edge(Edge<T>(parent[u], u, key[u]));
            }
            
            // 更新相邻顶点的key值
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                T weight = edge.second;
                
                if (!in_mst[v] && weight < key[v]) {
                    key[v] = weight;
                    parent[v] = u;
                    pq.push({weight, v});
                }
            }
        }
        
        // 检查是否连通
        result.is_connected = (result.edge_count() == vertices - 1);
        
        return result;
    }
    
    // 使用简单数组实现的Prim算法（适合稠密图）
    MST_Result<T> find_mst_dense(int start = 0) const {
        MST_Result<T> result;
        int vertices = graph.get_vertex_count();
        
        if (vertices == 0) {
            result.is_connected = true;
            return result;
        }
        
        if (start < 0 || start >= vertices) {
            start = 0;
        }
        
        vector<bool> in_mst(vertices, false);
        vector<T> key(vertices, numeric_limits<T>::max());
        vector<int> parent(vertices, -1);
        
        key[start] = T{};
        
        for (int count = 0; count < vertices; ++count) {
            // 找到key值最小的未访问顶点
            int u = -1;
            for (int v = 0; v < vertices; ++v) {
                if (!in_mst[v] && (u == -1 || key[v] < key[u])) {
                    u = v;
                }
            }
            
            if (u == -1 || key[u] == numeric_limits<T>::max()) {
                break; // 图不连通
            }
            
            in_mst[u] = true;
            
            // 如果不是起始顶点，添加边到MST
            if (parent[u] != -1) {
                result.add_edge(Edge<T>(parent[u], u, key[u]));
            }
            
            // 更新相邻顶点的key值
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                T weight = edge.second;
                
                if (!in_mst[v] && weight < key[v]) {
                    key[v] = weight;
                    parent[v] = u;
                }
            }
        }
        
        // 检查是否连通
        result.is_connected = (result.edge_count() == vertices - 1);
        
        return result;
    }
};

// Borůvka算法实现
template<typename T>
class BoruvkaAlgorithm {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    BoruvkaAlgorithm(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 计算最小生成树
    MST_Result<T> find_mst() const {
        MST_Result<T> result;
        int vertices = graph.get_vertex_count();
        
        if (vertices == 0) {
            result.is_connected = true;
            return result;
        }
        
        UnionFind uf(vertices);
        
        while (uf.get_components() > 1 && result.edge_count() < vertices - 1) {
            vector<Edge<T>> cheapest(vertices, Edge<T>(-1, -1, numeric_limits<T>::max()));
            
            // 为每个连通分量找到最便宜的出边
            for (int u = 0; u < vertices; ++u) {
                for (const auto& edge_info : graph.get_adjacent_edges(u)) {
                    int v = edge_info.first;
                    T weight = edge_info.second;
                    
                    int comp_u = uf.find(u);
                    int comp_v = uf.find(v);
                    
                    if (comp_u != comp_v) {
                        if (weight < cheapest[comp_u].weight) {
                            cheapest[comp_u] = Edge<T>(u, v, weight);
                        }
                        if (weight < cheapest[comp_v].weight) {
                            cheapest[comp_v] = Edge<T>(v, u, weight);
                        }
                    }
                }
            }
            
            // 添加最便宜的边
            for (int i = 0; i < vertices; ++i) {
                if (cheapest[i].from != -1 && cheapest[i].to != -1) {
                    if (uf.unite(cheapest[i].from, cheapest[i].to)) {
                        result.add_edge(cheapest[i]);
                    }
                }
            }
        }
        
        // 检查是否连通
        result.is_connected = (result.edge_count() == vertices - 1);
        
        return result;
    }
};

// 最小生成森林（处理不连通图）
template<typename T>
class MinimumSpanningForest {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    MinimumSpanningForest(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 计算最小生成森林
    vector<MST_Result<T>> find_msf() const {
        vector<MST_Result<T>> forest;
        int vertices = graph.get_vertex_count();
        vector<bool> visited(vertices, false);
        
        for (int i = 0; i < vertices; ++i) {
            if (!visited[i]) {
                // 找到连通分量
                vector<int> component;
                dfs_component(i, visited, component);
                
                if (!component.empty()) {
                    // 为这个连通分量构建子图
                    AdjacencyListGraph<T> subgraph = extract_subgraph(component);
                    
                    // 计算MST
                    PrimAlgorithm<T> prim(subgraph);
                    auto mst = prim.find_mst();
                    
                    // 将子图的顶点索引映射回原图
                    MST_Result<T> mapped_mst;
                    for (const auto& edge : mst.edges) {
                        mapped_mst.add_edge(Edge<T>(component[edge.from], component[edge.to], edge.weight));
                    }
                    mapped_mst.is_connected = mst.is_connected;
                    
                    forest.push_back(mapped_mst);
                }
            }
        }
        
        return forest;
    }
    
private:
    void dfs_component(int vertex, vector<bool>& visited, vector<int>& component) const {
        visited[vertex] = true;
        component.push_back(vertex);
        
        for (const auto& edge : graph.get_adjacent_edges(vertex)) {
            if (!visited[edge.first]) {
                dfs_component(edge.first, visited, component);
            }
        }
    }
    
    AdjacencyListGraph<T> extract_subgraph(const vector<int>& vertices) const {
        AdjacencyListGraph<T> subgraph(vertices.size(), false);
        
        // 创建顶点映射
        unordered_map<int, int> vertex_map;
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertex_map[vertices[i]] = i;
        }
        
        // 添加边
        for (size_t i = 0; i < vertices.size(); ++i) {
            int original_vertex = vertices[i];
            for (const auto& edge : graph.get_adjacent_edges(original_vertex)) {
                auto it = vertex_map.find(edge.first);
                if (it != vertex_map.end()) {
                    subgraph.add_edge(i, it->second, edge.second);
                }
            }
        }
        
        return subgraph;
    }
};

// 工具函数
template<typename T>
void print_mst_comparison(const AdjacencyListGraph<T>& graph) {
    cout << "MST Algorithm Comparison:" << endl;
    cout << "========================" << endl;
    
    // Kruskal算法
    KruskalAlgorithm<T> kruskal(graph);
    auto kruskal_result = kruskal.find_mst();
    cout << "Kruskal Algorithm:" << endl;
    cout << "Total weight: " << kruskal_result.total_weight << endl;
    cout << "Number of edges: " << kruskal_result.edge_count() << endl;
    cout << "Is connected: " << (kruskal_result.is_connected ? "Yes" : "No") << endl;
    cout << endl;
    
    // Prim算法
    PrimAlgorithm<T> prim(graph);
    auto prim_result = prim.find_mst();
    cout << "Prim Algorithm:" << endl;
    cout << "Total weight: " << prim_result.total_weight << endl;
    cout << "Number of edges: " << prim_result.edge_count() << endl;
    cout << "Is connected: " << (prim_result.is_connected ? "Yes" : "No") << endl;
    cout << endl;
    
    // Borůvka算法
    BoruvkaAlgorithm<T> boruvka(graph);
    auto boruvka_result = boruvka.find_mst();
    cout << "Borůvka Algorithm:" << endl;
    cout << "Total weight: " << boruvka_result.total_weight << endl;
    cout << "Number of edges: " << boruvka_result.edge_count() << endl;
    cout << "Is connected: " << (boruvka_result.is_connected ? "Yes" : "No") << endl;
}

template<typename T>
MST_Result<T> find_mst_auto(const AdjacencyListGraph<T>& graph) {
    int vertices = graph.get_vertex_count();
    int edges = graph.get_edges().size();
    
    // 根据图的密度选择算法
    if (edges > vertices * vertices / 4) {
        // 稠密图，使用Prim算法
        PrimAlgorithm<T> prim(graph);
        return prim.find_mst_dense();
    } else {
        // 稀疏图，使用Kruskal算法
        KruskalAlgorithm<T> kruskal(graph);
        return kruskal.find_mst();
    }
}

// 验证MST的正确性
template<typename T>
bool verify_mst(const AdjacencyListGraph<T>& graph, const MST_Result<T>& mst) {
    int vertices = graph.get_vertex_count();
    
    // 检查边数
    if (mst.is_connected && mst.edge_count() != vertices - 1) {
        return false;
    }
    
    // 检查是否形成树（无环且连通）
    AdjacencyListGraph<T> mst_graph(vertices, false);
    for (const auto& edge : mst.edges) {
        mst_graph.add_edge(edge.from, edge.to, edge.weight);
    }
    
    // 检查连通性
    if (mst.is_connected && !mst_graph.is_connected()) {
        return false;
    }
    
    // 检查是否有环
    if (mst_graph.has_cycle_undirected()) {
        return false;
    }
    
    return true;
}

#endif // MINIMUM_SPANNING_TREE_H