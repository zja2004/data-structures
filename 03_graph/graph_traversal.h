#ifndef GRAPH_TRAVERSAL_H
#define GRAPH_TRAVERSAL_H

#include "graph_representation.h"
#include <vector>
#include <queue>
#include <stack>
#include <functional>
#include <unordered_set>
#include <algorithm>

using namespace std;

// DFS遍历结果
struct DFSResult {
    vector<int> discovery_time;    // 发现时间
    vector<int> finish_time;       // 完成时间
    vector<int> parent;            // 父节点
    vector<bool> visited;          // 访问状态
    vector<int> traversal_order;   // 遍历顺序
    int time_counter;
    
    DFSResult(int vertices) : discovery_time(vertices, -1), finish_time(vertices, -1),
                             parent(vertices, -1), visited(vertices, false), time_counter(0) {}
};

// BFS遍历结果
struct BFSResult {
    vector<int> distance;          // 距离起点的距离
    vector<int> parent;            // 父节点
    vector<bool> visited;          // 访问状态
    vector<int> traversal_order;   // 遍历顺序
    
    BFSResult(int vertices) : distance(vertices, -1), parent(vertices, -1),
                             visited(vertices, false) {}
};

// 深度优先搜索类
template<typename T>
class DepthFirstSearch {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    DepthFirstSearch(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 基本DFS遍历
    void dfs(int start, function<void(int)> visit) const {
        if (start < 0 || start >= graph.get_vertex_count()) return;
        
        vector<bool> visited(graph.get_vertex_count(), false);
        dfs_recursive(start, visited, visit);
    }
    
    // 完整DFS（遍历所有连通分量）
    DFSResult dfs_complete() const {
        DFSResult result(graph.get_vertex_count());
        
        for (int i = 0; i < graph.get_vertex_count(); ++i) {
            if (!result.visited[i]) {
                dfs_recursive_complete(i, result);
            }
        }
        
        return result;
    }
    
    // 迭代版本DFS
    void dfs_iterative(int start, function<void(int)> visit) const {
        if (start < 0 || start >= graph.get_vertex_count()) return;
        
        vector<bool> visited(graph.get_vertex_count(), false);
        stack<int> stk;
        
        stk.push(start);
        
        while (!stk.empty()) {
            int vertex = stk.top();
            stk.pop();
            
            if (!visited[vertex]) {
                visited[vertex] = true;
                visit(vertex);
                
                // 按相反顺序添加邻居，保证遍历顺序
                auto neighbors = graph.get_neighbors(vertex);
                for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {
                    if (!visited[*it]) {
                        stk.push(*it);
                    }
                }
            }
        }
    }
    
    // 查找路径
    vector<int> find_path(int start, int end) const {
        if (start < 0 || start >= graph.get_vertex_count() ||
            end < 0 || end >= graph.get_vertex_count()) {
            return {};
        }
        
        vector<bool> visited(graph.get_vertex_count(), false);
        vector<int> parent(graph.get_vertex_count(), -1);
        
        if (dfs_find_path(start, end, visited, parent)) {
            return reconstruct_path(parent, start, end);
        }
        
        return {};
    }
    
    // 检测环（无向图）
    bool has_cycle_undirected() const {
        vector<bool> visited(graph.get_vertex_count(), false);
        
        for (int i = 0; i < graph.get_vertex_count(); ++i) {
            if (!visited[i]) {
                if (has_cycle_undirected_util(i, -1, visited)) {
                    return true;
                }
            }
        }
        return false;
    }
    
    // 检测环（有向图）
    bool has_cycle_directed() const {
        vector<int> color(graph.get_vertex_count(), 0); // 0: white, 1: gray, 2: black
        
        for (int i = 0; i < graph.get_vertex_count(); ++i) {
            if (color[i] == 0) {
                if (has_cycle_directed_util(i, color)) {
                    return true;
                }
            }
        }
        return false;
    }
    
    // 获取连通分量
    vector<vector<int>> get_connected_components() const {
        vector<vector<int>> components;
        vector<bool> visited(graph.get_vertex_count(), false);
        
        for (int i = 0; i < graph.get_vertex_count(); ++i) {
            if (!visited[i]) {
                vector<int> component;
                dfs_component(i, visited, component);
                components.push_back(component);
            }
        }
        
        return components;
    }
    
private:
    void dfs_recursive(int vertex, vector<bool>& visited, function<void(int)> visit) const {
        visited[vertex] = true;
        visit(vertex);
        
        for (const auto& edge : graph.get_adjacent_edges(vertex)) {
            if (!visited[edge.first]) {
                dfs_recursive(edge.first, visited, visit);
            }
        }
    }
    
    void dfs_recursive_complete(int vertex, DFSResult& result) const {
        result.visited[vertex] = true;
        result.discovery_time[vertex] = result.time_counter++;
        result.traversal_order.push_back(vertex);
        
        for (const auto& edge : graph.get_adjacent_edges(vertex)) {
            if (!result.visited[edge.first]) {
                result.parent[edge.first] = vertex;
                dfs_recursive_complete(edge.first, result);
            }
        }
        
        result.finish_time[vertex] = result.time_counter++;
    }
    
    bool dfs_find_path(int current, int target, vector<bool>& visited, vector<int>& parent) const {
        if (current == target) return true;
        
        visited[current] = true;
        
        for (const auto& edge : graph.get_adjacent_edges(current)) {
            if (!visited[edge.first]) {
                parent[edge.first] = current;
                if (dfs_find_path(edge.first, target, visited, parent)) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    bool has_cycle_undirected_util(int vertex, int parent, vector<bool>& visited) const {
        visited[vertex] = true;
        
        for (const auto& edge : graph.get_adjacent_edges(vertex)) {
            int neighbor = edge.first;
            if (!visited[neighbor]) {
                if (has_cycle_undirected_util(neighbor, vertex, visited)) {
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
        
        for (const auto& edge : graph.get_adjacent_edges(vertex)) {
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
    
    void dfs_component(int vertex, vector<bool>& visited, vector<int>& component) const {
        visited[vertex] = true;
        component.push_back(vertex);
        
        for (const auto& edge : graph.get_adjacent_edges(vertex)) {
            if (!visited[edge.first]) {
                dfs_component(edge.first, visited, component);
            }
        }
    }
    
    vector<int> reconstruct_path(const vector<int>& parent, int start, int end) const {
        vector<int> path;
        for (int v = end; v != -1; v = parent[v]) {
            path.push_back(v);
        }
        reverse(path.begin(), path.end());
        return path[0] == start ? path : vector<int>();
    }
};

// 广度优先搜索类
template<typename T>
class BreadthFirstSearch {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    BreadthFirstSearch(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 基本BFS遍历
    void bfs(int start, function<void(int)> visit) const {
        if (start < 0 || start >= graph.get_vertex_count()) return;
        
        vector<bool> visited(graph.get_vertex_count(), false);
        queue<int> q;
        
        visited[start] = true;
        q.push(start);
        
        while (!q.empty()) {
            int vertex = q.front();
            q.pop();
            visit(vertex);
            
            for (const auto& edge : graph.get_adjacent_edges(vertex)) {
                if (!visited[edge.first]) {
                    visited[edge.first] = true;
                    q.push(edge.first);
                }
            }
        }
    }
    
    // 完整BFS（返回详细信息）
    BFSResult bfs_complete(int start) const {
        BFSResult result(graph.get_vertex_count());
        
        if (start < 0 || start >= graph.get_vertex_count()) {
            return result;
        }
        
        queue<int> q;
        result.visited[start] = true;
        result.distance[start] = 0;
        q.push(start);
        
        while (!q.empty()) {
            int vertex = q.front();
            q.pop();
            result.traversal_order.push_back(vertex);
            
            for (const auto& edge : graph.get_adjacent_edges(vertex)) {
                if (!result.visited[edge.first]) {
                    result.visited[edge.first] = true;
                    result.distance[edge.first] = result.distance[vertex] + 1;
                    result.parent[edge.first] = vertex;
                    q.push(edge.first);
                }
            }
        }
        
        return result;
    }
    
    // 查找最短路径（无权图）
    vector<int> find_shortest_path(int start, int end) const {
        if (start < 0 || start >= graph.get_vertex_count() ||
            end < 0 || end >= graph.get_vertex_count()) {
            return {};
        }
        
        BFSResult result = bfs_complete(start);
        
        if (!result.visited[end]) {
            return {}; // 无路径
        }
        
        return reconstruct_path(result.parent, start, end);
    }
    
    // 计算最短距离
    int shortest_distance(int start, int end) const {
        if (start < 0 || start >= graph.get_vertex_count() ||
            end < 0 || end >= graph.get_vertex_count()) {
            return -1;
        }
        
        BFSResult result = bfs_complete(start);
        return result.visited[end] ? result.distance[end] : -1;
    }
    
    // 层次遍历
    vector<vector<int>> level_order_traversal(int start) const {
        vector<vector<int>> levels;
        
        if (start < 0 || start >= graph.get_vertex_count()) {
            return levels;
        }
        
        vector<bool> visited(graph.get_vertex_count(), false);
        queue<int> q;
        
        visited[start] = true;
        q.push(start);
        
        while (!q.empty()) {
            int level_size = q.size();
            vector<int> current_level;
            
            for (int i = 0; i < level_size; ++i) {
                int vertex = q.front();
                q.pop();
                current_level.push_back(vertex);
                
                for (const auto& edge : graph.get_adjacent_edges(vertex)) {
                    if (!visited[edge.first]) {
                        visited[edge.first] = true;
                        q.push(edge.first);
                    }
                }
            }
            
            levels.push_back(current_level);
        }
        
        return levels;
    }
    
    // 检查二分图
    bool is_bipartite() const {
        vector<int> color(graph.get_vertex_count(), -1);
        
        for (int i = 0; i < graph.get_vertex_count(); ++i) {
            if (color[i] == -1) {
                if (!is_bipartite_util(i, color)) {
                    return false;
                }
            }
        }
        return true;
    }
    
private:
    vector<int> reconstruct_path(const vector<int>& parent, int start, int end) const {
        vector<int> path;
        for (int v = end; v != -1; v = parent[v]) {
            path.push_back(v);
        }
        reverse(path.begin(), path.end());
        return path[0] == start ? path : vector<int>();
    }
    
    bool is_bipartite_util(int start, vector<int>& color) const {
        queue<int> q;
        color[start] = 0;
        q.push(start);
        
        while (!q.empty()) {
            int vertex = q.front();
            q.pop();
            
            for (const auto& edge : graph.get_adjacent_edges(vertex)) {
                if (color[edge.first] == -1) {
                    color[edge.first] = 1 - color[vertex];
                    q.push(edge.first);
                } else if (color[edge.first] == color[vertex]) {
                    return false;
                }
            }
        }
        return true;
    }
};

// 工具函数
template<typename T>
void print_traversal_result(const vector<int>& traversal, const string& name) {
    cout << name << ": ";
    for (int vertex : traversal) {
        cout << vertex << " ";
    }
    cout << endl;
}

template<typename T>
void print_path(const vector<int>& path, const string& name) {
    cout << name << ": ";
    if (path.empty()) {
        cout << "No path found";
    } else {
        for (size_t i = 0; i < path.size(); ++i) {
            if (i > 0) cout << " -> ";
            cout << path[i];
        }
    }
    cout << endl;
}

template<typename T>
void print_connected_components(const vector<vector<int>>& components) {
    cout << "Connected Components:" << endl;
    for (size_t i = 0; i < components.size(); ++i) {
        cout << "Component " << i + 1 << ": ";
        for (int vertex : components[i]) {
            cout << vertex << " ";
        }
        cout << endl;
    }
}

template<typename T>
void print_level_order(const vector<vector<int>>& levels) {
    cout << "Level Order Traversal:" << endl;
    for (size_t i = 0; i < levels.size(); ++i) {
        cout << "Level " << i << ": ";
        for (int vertex : levels[i]) {
            cout << vertex << " ";
        }
        cout << endl;
    }
}

// 图遍历统计信息
struct TraversalStats {
    int vertices_visited;
    int edges_traversed;
    int connected_components;
    bool has_cycle;
    bool is_bipartite;
    int diameter; // 图的直径（最长最短路径）
};

template<typename T>
TraversalStats analyze_graph(const AdjacencyListGraph<T>& graph) {
    TraversalStats stats;
    
    DepthFirstSearch<T> dfs(graph);
    BreadthFirstSearch<T> bfs(graph);
    
    // 连通分量
    auto components = dfs.get_connected_components();
    stats.connected_components = components.size();
    
    // 访问的顶点数
    stats.vertices_visited = 0;
    for (const auto& component : components) {
        stats.vertices_visited += component.size();
    }
    
    // 边数
    stats.edges_traversed = graph.get_edges().size();
    
    // 环检测
    stats.has_cycle = dfs.has_cycle_undirected() || dfs.has_cycle_directed();
    
    // 二分图检测
    stats.is_bipartite = bfs.is_bipartite();
    
    // 计算直径
    stats.diameter = 0;
    for (int i = 0; i < graph.get_vertex_count(); ++i) {
        for (int j = i + 1; j < graph.get_vertex_count(); ++j) {
            int dist = bfs.shortest_distance(i, j);
            if (dist > stats.diameter) {
                stats.diameter = dist;
            }
        }
    }
    
    return stats;
}

#endif // GRAPH_TRAVERSAL_H