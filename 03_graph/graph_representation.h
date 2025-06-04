#ifndef GRAPH_REPRESENTATION_H
#define GRAPH_REPRESENTATION_H

#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <functional>
#include <queue>
#include <stack>
#include <limits>

using namespace std;

// 边结构
template<typename T = int>
struct Edge {
    int from;
    int to;
    T weight;
    
    Edge() : from(-1), to(-1), weight(T{}) {}
    Edge(int f, int t, T w = T{}) : from(f), to(t), weight(w) {}
    
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
    
    bool operator>(const Edge& other) const {
        return weight > other.weight;
    }
    
    bool operator==(const Edge& other) const {
        return from == other.from && to == other.to && weight == other.weight;
    }
};

// 邻接矩阵图
template<typename T = int>
class AdjacencyMatrixGraph {
private:
    vector<vector<T>> matrix;
    int vertex_count;
    bool is_directed;
    T default_value;
    T infinity_value;
    
public:
    AdjacencyMatrixGraph(int vertices, bool directed = false, 
                        T default_val = T{}, T inf_val = numeric_limits<T>::max())
        : vertex_count(vertices), is_directed(directed), 
          default_value(default_val), infinity_value(inf_val) {
        matrix.assign(vertices, vector<T>(vertices, default_value));
    }
    
    // 添加边
    void add_edge(int from, int to, T weight = T{1}) {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            throw out_of_range("Vertex index out of range");
        }
        
        matrix[from][to] = weight;
        if (!is_directed) {
            matrix[to][from] = weight;
        }
    }
    
    // 删除边
    void remove_edge(int from, int to) {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            throw out_of_range("Vertex index out of range");
        }
        
        matrix[from][to] = default_value;
        if (!is_directed) {
            matrix[to][from] = default_value;
        }
    }
    
    // 检查边是否存在
    bool has_edge(int from, int to) const {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            return false;
        }
        return matrix[from][to] != default_value;
    }
    
    // 获取边的权重
    T get_weight(int from, int to) const {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            return infinity_value;
        }
        return matrix[from][to];
    }
    
    // 获取顶点数量
    int get_vertex_count() const {
        return vertex_count;
    }
    
    // 获取顶点的度
    int get_degree(int vertex) const {
        if (vertex < 0 || vertex >= vertex_count) {
            return 0;
        }
        
        int degree = 0;
        for (int i = 0; i < vertex_count; ++i) {
            if (matrix[vertex][i] != default_value) {
                degree++;
            }
        }
        return degree;
    }
    
    // 获取邻居列表
    vector<int> get_neighbors(int vertex) const {
        vector<int> neighbors;
        if (vertex < 0 || vertex >= vertex_count) {
            return neighbors;
        }
        
        for (int i = 0; i < vertex_count; ++i) {
            if (matrix[vertex][i] != default_value) {
                neighbors.push_back(i);
            }
        }
        return neighbors;
    }
    
    // 获取所有边
    vector<Edge<T>> get_edges() const {
        vector<Edge<T>> edges;
        for (int i = 0; i < vertex_count; ++i) {
            for (int j = 0; j < vertex_count; ++j) {
                if (matrix[i][j] != default_value) {
                    if (is_directed || i <= j) {
                        edges.emplace_back(i, j, matrix[i][j]);
                    }
                }
            }
        }
        return edges;
    }
    
    // 打印图
    void print() const {
        cout << "Adjacency Matrix (" << (is_directed ? "Directed" : "Undirected") << "):" << endl;
        cout << "   ";
        for (int i = 0; i < vertex_count; ++i) {
            cout << i << " ";
        }
        cout << endl;
        
        for (int i = 0; i < vertex_count; ++i) {
            cout << i << ": ";
            for (int j = 0; j < vertex_count; ++j) {
                if (matrix[i][j] == default_value) {
                    cout << "- ";
                } else {
                    cout << matrix[i][j] << " ";
                }
            }
            cout << endl;
        }
    }
    
    // 检查图是否连通
    bool is_connected() const {
        if (vertex_count == 0) return true;
        
        vector<bool> visited(vertex_count, false);
        dfs_util(0, visited);
        
        for (bool v : visited) {
            if (!v) return false;
        }
        return true;
    }
    
private:
    void dfs_util(int vertex, vector<bool>& visited) const {
        visited[vertex] = true;
        for (int i = 0; i < vertex_count; ++i) {
            if (matrix[vertex][i] != default_value && !visited[i]) {
                dfs_util(i, visited);
            }
        }
    }
};

// 邻接表图
template<typename T = int>
class AdjacencyListGraph {
private:
    vector<vector<pair<int, T>>> adj_list;
    int vertex_count;
    bool is_directed;
    
public:
    AdjacencyListGraph(int vertices, bool directed = false)
        : vertex_count(vertices), is_directed(directed) {
        adj_list.resize(vertices);
    }
    
    // 添加边
    void add_edge(int from, int to, T weight = T{1}) {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            throw out_of_range("Vertex index out of range");
        }
        
        adj_list[from].emplace_back(to, weight);
        if (!is_directed) {
            adj_list[to].emplace_back(from, weight);
        }
    }
    
    // 删除边
    void remove_edge(int from, int to) {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            return;
        }
        
        auto& edges = adj_list[from];
        edges.erase(remove_if(edges.begin(), edges.end(),
                             [to](const pair<int, T>& edge) {
                                 return edge.first == to;
                             }), edges.end());
        
        if (!is_directed) {
            auto& reverse_edges = adj_list[to];
            reverse_edges.erase(remove_if(reverse_edges.begin(), reverse_edges.end(),
                                         [from](const pair<int, T>& edge) {
                                             return edge.first == from;
                                         }), reverse_edges.end());
        }
    }
    
    // 检查边是否存在
    bool has_edge(int from, int to) const {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            return false;
        }
        
        const auto& edges = adj_list[from];
        return find_if(edges.begin(), edges.end(),
                      [to](const pair<int, T>& edge) {
                          return edge.first == to;
                      }) != edges.end();
    }
    
    // 获取边的权重
    T get_weight(int from, int to) const {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            return numeric_limits<T>::max();
        }
        
        const auto& edges = adj_list[from];
        auto it = find_if(edges.begin(), edges.end(),
                         [to](const pair<int, T>& edge) {
                             return edge.first == to;
                         });
        
        return (it != edges.end()) ? it->second : numeric_limits<T>::max();
    }
    
    // 获取顶点数量
    int get_vertex_count() const {
        return vertex_count;
    }
    
    // 获取顶点的度
    int get_degree(int vertex) const {
        if (vertex < 0 || vertex >= vertex_count) {
            return 0;
        }
        return adj_list[vertex].size();
    }
    
    // 获取邻居列表
    vector<int> get_neighbors(int vertex) const {
        vector<int> neighbors;
        if (vertex < 0 || vertex >= vertex_count) {
            return neighbors;
        }
        
        for (const auto& edge : adj_list[vertex]) {
            neighbors.push_back(edge.first);
        }
        return neighbors;
    }
    
    // 获取邻接边列表
    const vector<pair<int, T>>& get_adjacent_edges(int vertex) const {
        static vector<pair<int, T>> empty;
        if (vertex < 0 || vertex >= vertex_count) {
            return empty;
        }
        return adj_list[vertex];
    }
    
    // 获取所有边
    vector<Edge<T>> get_edges() const {
        vector<Edge<T>> edges;
        for (int i = 0; i < vertex_count; ++i) {
            for (const auto& edge : adj_list[i]) {
                if (is_directed || i <= edge.first) {
                    edges.emplace_back(i, edge.first, edge.second);
                }
            }
        }
        return edges;
    }
    
    // 打印图
    void print() const {
        cout << "Adjacency List (" << (is_directed ? "Directed" : "Undirected") << "):" << endl;
        for (int i = 0; i < vertex_count; ++i) {
            cout << i << ": ";
            for (const auto& edge : adj_list[i]) {
                cout << "(" << edge.first << ", " << edge.second << ") ";
            }
            cout << endl;
        }
    }
    
    // DFS遍历
    void dfs(int start, function<void(int)> visit) const {
        if (start < 0 || start >= vertex_count) return;
        
        vector<bool> visited(vertex_count, false);
        dfs_util(start, visited, visit);
    }
    
    // BFS遍历
    void bfs(int start, function<void(int)> visit) const {
        if (start < 0 || start >= vertex_count) return;
        
        vector<bool> visited(vertex_count, false);
        queue<int> q;
        
        visited[start] = true;
        q.push(start);
        
        while (!q.empty()) {
            int vertex = q.front();
            q.pop();
            visit(vertex);
            
            for (const auto& edge : adj_list[vertex]) {
                if (!visited[edge.first]) {
                    visited[edge.first] = true;
                    q.push(edge.first);
                }
            }
        }
    }
    
    // 检查图是否连通
    bool is_connected() const {
        if (vertex_count == 0) return true;
        
        vector<bool> visited(vertex_count, false);
        dfs_util(0, visited, [](int){});
        
        for (bool v : visited) {
            if (!v) return false;
        }
        return true;
    }
    
    // 检测环（无向图）
    bool has_cycle_undirected() const {
        if (is_directed) return false;
        
        vector<bool> visited(vertex_count, false);
        for (int i = 0; i < vertex_count; ++i) {
            if (!visited[i]) {
                if (has_cycle_util(i, -1, visited)) {
                    return true;
                }
            }
        }
        return false;
    }
    
    // 检测环（有向图）
    bool has_cycle_directed() const {
        if (!is_directed) return false;
        
        vector<int> color(vertex_count, 0); // 0: white, 1: gray, 2: black
        for (int i = 0; i < vertex_count; ++i) {
            if (color[i] == 0) {
                if (has_cycle_directed_util(i, color)) {
                    return true;
                }
            }
        }
        return false;
    }
    
private:
    void dfs_util(int vertex, vector<bool>& visited, function<void(int)> visit) const {
        visited[vertex] = true;
        visit(vertex);
        
        for (const auto& edge : adj_list[vertex]) {
            if (!visited[edge.first]) {
                dfs_util(edge.first, visited, visit);
            }
        }
    }
    
    bool has_cycle_util(int vertex, int parent, vector<bool>& visited) const {
        visited[vertex] = true;
        
        for (const auto& edge : adj_list[vertex]) {
            int neighbor = edge.first;
            if (!visited[neighbor]) {
                if (has_cycle_util(neighbor, vertex, visited)) {
                    return true;
                }
            } else if (neighbor != parent) {
                return true;
            }
        }
        return false;
    }
    
    bool has_cycle_directed_util(int vertex, vector<int>& color) const {
        color[vertex] = 1; // gray
        
        for (const auto& edge : adj_list[vertex]) {
            int neighbor = edge.first;
            if (color[neighbor] == 1) {
                return true; // back edge found
            }
            if (color[neighbor] == 0 && has_cycle_directed_util(neighbor, color)) {
                return true;
            }
        }
        
        color[vertex] = 2; // black
        return false;
    }
};

// 边列表图
template<typename T = int>
class EdgeListGraph {
private:
    vector<Edge<T>> edges;
    int vertex_count;
    bool is_directed;
    
public:
    EdgeListGraph(int vertices, bool directed = false)
        : vertex_count(vertices), is_directed(directed) {}
    
    // 添加边
    void add_edge(int from, int to, T weight = T{1}) {
        if (from < 0 || from >= vertex_count || to < 0 || to >= vertex_count) {
            throw out_of_range("Vertex index out of range");
        }
        
        edges.emplace_back(from, to, weight);
        if (!is_directed) {
            edges.emplace_back(to, from, weight);
        }
    }
    
    // 删除边
    void remove_edge(int from, int to) {
        edges.erase(remove_if(edges.begin(), edges.end(),
                             [from, to](const Edge<T>& edge) {
                                 return edge.from == from && edge.to == to;
                             }), edges.end());
        
        if (!is_directed) {
            edges.erase(remove_if(edges.begin(), edges.end(),
                                 [from, to](const Edge<T>& edge) {
                                     return edge.from == to && edge.to == from;
                                 }), edges.end());
        }
    }
    
    // 获取所有边
    const vector<Edge<T>>& get_edges() const {
        return edges;
    }
    
    // 获取顶点数量
    int get_vertex_count() const {
        return vertex_count;
    }
    
    // 获取边数量
    int get_edge_count() const {
        return is_directed ? edges.size() : edges.size() / 2;
    }
    
    // 打印图
    void print() const {
        cout << "Edge List (" << (is_directed ? "Directed" : "Undirected") << "):" << endl;
        for (const auto& edge : edges) {
            if (is_directed || edge.from <= edge.to) {
                cout << edge.from << " -> " << edge.to << " (" << edge.weight << ")" << endl;
            }
        }
    }
    
    // 转换为邻接表
    AdjacencyListGraph<T> to_adjacency_list() const {
        AdjacencyListGraph<T> graph(vertex_count, is_directed);
        for (const auto& edge : edges) {
            if (is_directed || edge.from <= edge.to) {
                graph.add_edge(edge.from, edge.to, edge.weight);
            }
        }
        return graph;
    }
};

// 类型别名
using UnweightedGraph = AdjacencyListGraph<int>;
using WeightedGraph = AdjacencyListGraph<double>;
using UnweightedMatrix = AdjacencyMatrixGraph<int>;
using WeightedMatrix = AdjacencyMatrixGraph<double>;

// 工具函数
template<typename T>
AdjacencyListGraph<T> create_complete_graph(int vertices, bool directed = false) {
    AdjacencyListGraph<T> graph(vertices, directed);
    for (int i = 0; i < vertices; ++i) {
        for (int j = i + 1; j < vertices; ++j) {
            graph.add_edge(i, j, T{1});
        }
    }
    return graph;
}

template<typename T>
AdjacencyListGraph<T> create_cycle_graph(int vertices, bool directed = false) {
    AdjacencyListGraph<T> graph(vertices, directed);
    for (int i = 0; i < vertices; ++i) {
        graph.add_edge(i, (i + 1) % vertices, T{1});
    }
    return graph;
}

template<typename T>
AdjacencyListGraph<T> create_path_graph(int vertices, bool directed = false) {
    AdjacencyListGraph<T> graph(vertices, directed);
    for (int i = 0; i < vertices - 1; ++i) {
        graph.add_edge(i, i + 1, T{1});
    }
    return graph;
}

#endif // GRAPH_REPRESENTATION_H