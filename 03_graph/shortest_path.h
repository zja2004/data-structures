#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "graph_representation.h"
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include <cmath>

using namespace std;

const double INF = numeric_limits<double>::infinity();
const int INT_INF = numeric_limits<int>::max();

// 最短路径结果
struct ShortestPathResult {
    vector<double> distances;      // 到各顶点的最短距离
    vector<int> predecessors;      // 前驱节点
    bool has_negative_cycle;       // 是否存在负权环
    int source;                    // 源顶点
    
    ShortestPathResult(int vertices, int src = -1) 
        : distances(vertices, INF), predecessors(vertices, -1), 
          has_negative_cycle(false), source(src) {}
    
    // 重构路径
    vector<int> get_path(int target) const {
        if (distances[target] == INF) {
            return {}; // 无路径
        }
        
        vector<int> path;
        for (int v = target; v != -1; v = predecessors[v]) {
            path.push_back(v);
        }
        reverse(path.begin(), path.end());
        return path;
    }
    
    // 获取距离
    double get_distance(int target) const {
        return distances[target];
    }
};

// 所有对最短路径结果
struct AllPairsShortestPath {
    vector<vector<double>> distances;     // 距离矩阵
    vector<vector<int>> next;             // 下一跳节点
    bool has_negative_cycle;              // 是否存在负权环
    
    AllPairsShortestPath(int vertices) 
        : distances(vertices, vector<double>(vertices, INF)),
          next(vertices, vector<int>(vertices, -1)),
          has_negative_cycle(false) {
        // 初始化对角线为0
        for (int i = 0; i < vertices; ++i) {
            distances[i][i] = 0;
        }
    }
    
    // 重构路径
    vector<int> get_path(int source, int target) const {
        if (distances[source][target] == INF) {
            return {}; // 无路径
        }
        
        vector<int> path;
        int current = source;
        path.push_back(current);
        
        while (current != target) {
            current = next[current][target];
            if (current == -1) {
                return {}; // 路径重构失败
            }
            path.push_back(current);
        }
        
        return path;
    }
    
    // 获取距离
    double get_distance(int source, int target) const {
        return distances[source][target];
    }
};

// Dijkstra算法实现
template<typename T>
class DijkstraAlgorithm {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    DijkstraAlgorithm(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 单源最短路径
    ShortestPathResult shortest_path(int source) const {
        int vertices = graph.get_vertex_count();
        ShortestPathResult result(vertices, source);
        
        if (source < 0 || source >= vertices) {
            return result;
        }
        
        // 优先队列：(距离, 顶点)
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
        vector<bool> visited(vertices, false);
        
        result.distances[source] = 0;
        pq.push({0, source});
        
        while (!pq.empty()) {
            double dist = pq.top().first;
            int u = pq.top().second;
            pq.pop();
            
            if (visited[u]) continue;
            visited[u] = true;
            
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                double weight = static_cast<double>(edge.second);
                
                if (weight < 0) {
                    throw invalid_argument("Dijkstra algorithm cannot handle negative weights");
                }
                
                double new_dist = dist + weight;
                if (new_dist < result.distances[v]) {
                    result.distances[v] = new_dist;
                    result.predecessors[v] = u;
                    pq.push({new_dist, v});
                }
            }
        }
        
        return result;
    }
    
    // 查找两点间最短路径
    vector<int> find_path(int source, int target) const {
        auto result = shortest_path(source);
        return result.get_path(target);
    }
    
    // 查找两点间最短距离
    double find_distance(int source, int target) const {
        auto result = shortest_path(source);
        return result.get_distance(target);
    }
};

// Bellman-Ford算法实现
template<typename T>
class BellmanFordAlgorithm {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    BellmanFordAlgorithm(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 单源最短路径（可处理负权边）
    ShortestPathResult shortest_path(int source) const {
        int vertices = graph.get_vertex_count();
        ShortestPathResult result(vertices, source);
        
        if (source < 0 || source >= vertices) {
            return result;
        }
        
        result.distances[source] = 0;
        
        // 松弛操作，重复V-1次
        for (int i = 0; i < vertices - 1; ++i) {
            bool updated = false;
            
            for (int u = 0; u < vertices; ++u) {
                if (result.distances[u] == INF) continue;
                
                for (const auto& edge : graph.get_adjacent_edges(u)) {
                    int v = edge.first;
                    double weight = static_cast<double>(edge.second);
                    double new_dist = result.distances[u] + weight;
                    
                    if (new_dist < result.distances[v]) {
                        result.distances[v] = new_dist;
                        result.predecessors[v] = u;
                        updated = true;
                    }
                }
            }
            
            if (!updated) break; // 提前终止优化
        }
        
        // 检测负权环
        for (int u = 0; u < vertices; ++u) {
            if (result.distances[u] == INF) continue;
            
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                double weight = static_cast<double>(edge.second);
                
                if (result.distances[u] + weight < result.distances[v]) {
                    result.has_negative_cycle = true;
                    return result;
                }
            }
        }
        
        return result;
    }
    
    // 检测负权环
    bool has_negative_cycle() const {
        // 从任意顶点开始检测
        for (int i = 0; i < graph.get_vertex_count(); ++i) {
            auto result = shortest_path(i);
            if (result.has_negative_cycle) {
                return true;
            }
        }
        return false;
    }
};

// Floyd-Warshall算法实现
template<typename T>
class FloydWarshallAlgorithm {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    FloydWarshallAlgorithm(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 所有对最短路径
    AllPairsShortestPath all_pairs_shortest_path() const {
        int vertices = graph.get_vertex_count();
        AllPairsShortestPath result(vertices);
        
        // 初始化距离矩阵
        for (int u = 0; u < vertices; ++u) {
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                double weight = static_cast<double>(edge.second);
                result.distances[u][v] = weight;
                result.next[u][v] = v;
            }
        }
        
        // Floyd-Warshall核心算法
        for (int k = 0; k < vertices; ++k) {
            for (int i = 0; i < vertices; ++i) {
                for (int j = 0; j < vertices; ++j) {
                    if (result.distances[i][k] != INF && result.distances[k][j] != INF) {
                        double new_dist = result.distances[i][k] + result.distances[k][j];
                        if (new_dist < result.distances[i][j]) {
                            result.distances[i][j] = new_dist;
                            result.next[i][j] = result.next[i][k];
                        }
                    }
                }
            }
        }
        
        // 检测负权环
        for (int i = 0; i < vertices; ++i) {
            if (result.distances[i][i] < 0) {
                result.has_negative_cycle = true;
                break;
            }
        }
        
        return result;
    }
    
    // 获取距离矩阵
    vector<vector<double>> get_distance_matrix() const {
        auto result = all_pairs_shortest_path();
        return result.distances;
    }
};

// A*算法实现
template<typename T>
class AStarAlgorithm {
private:
    const AdjacencyListGraph<T>& graph;
    function<double(int, int)> heuristic; // 启发函数
    
public:
    AStarAlgorithm(const AdjacencyListGraph<T>& g, function<double(int, int)> h)
        : graph(g), heuristic(h) {}
    
    // A*搜索
    vector<int> find_path(int source, int target) const {
        int vertices = graph.get_vertex_count();
        
        if (source < 0 || source >= vertices || target < 0 || target >= vertices) {
            return {};
        }
        
        vector<double> g_score(vertices, INF);  // 实际距离
        vector<double> f_score(vertices, INF);  // 估计总距离
        vector<int> came_from(vertices, -1);    // 前驱节点
        vector<bool> in_open_set(vertices, false);
        
        // 优先队列：(f_score, 顶点)
        priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> open_set;
        
        g_score[source] = 0;
        f_score[source] = heuristic(source, target);
        open_set.push({f_score[source], source});
        in_open_set[source] = true;
        
        while (!open_set.empty()) {
            int current = open_set.top().second;
            open_set.pop();
            in_open_set[current] = false;
            
            if (current == target) {
                // 重构路径
                vector<int> path;
                for (int v = target; v != -1; v = came_from[v]) {
                    path.push_back(v);
                }
                reverse(path.begin(), path.end());
                return path;
            }
            
            for (const auto& edge : graph.get_adjacent_edges(current)) {
                int neighbor = edge.first;
                double weight = static_cast<double>(edge.second);
                double tentative_g_score = g_score[current] + weight;
                
                if (tentative_g_score < g_score[neighbor]) {
                    came_from[neighbor] = current;
                    g_score[neighbor] = tentative_g_score;
                    f_score[neighbor] = g_score[neighbor] + heuristic(neighbor, target);
                    
                    if (!in_open_set[neighbor]) {
                        open_set.push({f_score[neighbor], neighbor});
                        in_open_set[neighbor] = true;
                    }
                }
            }
        }
        
        return {}; // 无路径
    }
};

// Johnson算法实现（稀疏图的所有对最短路径）
template<typename T>
class JohnsonAlgorithm {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    JohnsonAlgorithm(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 所有对最短路径（适用于稀疏图）
    AllPairsShortestPath all_pairs_shortest_path() const {
        int vertices = graph.get_vertex_count();
        AllPairsShortestPath result(vertices);
        
        // 创建扩展图（添加新顶点s）
        AdjacencyListGraph<T> extended_graph(vertices + 1, true);
        
        // 复制原图的边
        for (int u = 0; u < vertices; ++u) {
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                extended_graph.add_edge(u, edge.first, edge.second);
            }
        }
        
        // 从新顶点s到所有顶点添加权重为0的边
        int s = vertices;
        for (int i = 0; i < vertices; ++i) {
            extended_graph.add_edge(s, i, T{0});
        }
        
        // 使用Bellman-Ford算法计算h值
        BellmanFordAlgorithm<T> bellman_ford(extended_graph);
        auto h_result = bellman_ford.shortest_path(s);
        
        if (h_result.has_negative_cycle) {
            result.has_negative_cycle = true;
            return result;
        }
        
        // 重新加权
        AdjacencyListGraph<double> reweighted_graph(vertices, true);
        for (int u = 0; u < vertices; ++u) {
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                double weight = static_cast<double>(edge.second);
                double new_weight = weight + h_result.distances[u] - h_result.distances[v];
                reweighted_graph.add_edge(u, v, new_weight);
            }
        }
        
        // 对每个顶点运行Dijkstra算法
        DijkstraAlgorithm<double> dijkstra(reweighted_graph);
        for (int u = 0; u < vertices; ++u) {
            auto dijkstra_result = dijkstra.shortest_path(u);
            
            for (int v = 0; v < vertices; ++v) {
                if (dijkstra_result.distances[v] != INF) {
                    // 恢复原始权重
                    result.distances[u][v] = dijkstra_result.distances[v] + 
                                           h_result.distances[v] - h_result.distances[u];
                    
                    // 设置下一跳节点
                    if (u != v) {
                        auto path = dijkstra_result.get_path(v);
                        if (path.size() > 1) {
                            result.next[u][v] = path[1];
                        }
                    }
                }
            }
        }
        
        return result;
    }
};

// 工具函数
template<typename T>
void print_shortest_path_result(const ShortestPathResult& result, int target) {
    cout << "Shortest path from " << result.source << " to " << target << ":" << endl;
    
    if (result.has_negative_cycle) {
        cout << "Negative cycle detected!" << endl;
        return;
    }
    
    double distance = result.get_distance(target);
    if (distance == INF) {
        cout << "No path exists" << endl;
        return;
    }
    
    cout << "Distance: " << distance << endl;
    cout << "Path: ";
    
    auto path = result.get_path(target);
    for (size_t i = 0; i < path.size(); ++i) {
        if (i > 0) cout << " -> ";
        cout << path[i];
    }
    cout << endl;
}

template<typename T>
void print_distance_matrix(const vector<vector<double>>& distances) {
    int vertices = distances.size();
    cout << "Distance Matrix:" << endl;
    cout << "     ";
    for (int i = 0; i < vertices; ++i) {
        cout << setw(8) << i;
    }
    cout << endl;
    
    for (int i = 0; i < vertices; ++i) {
        cout << setw(3) << i << ": ";
        for (int j = 0; j < vertices; ++j) {
            if (distances[i][j] == INF) {
                cout << setw(8) << "INF";
            } else {
                cout << setw(8) << fixed << setprecision(1) << distances[i][j];
            }
        }
        cout << endl;
    }
}

// 启发函数示例
struct Point {
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

// 欧几里得距离启发函数
double euclidean_heuristic(const vector<Point>& points, int a, int b) {
    double dx = points[a].x - points[b].x;
    double dy = points[a].y - points[b].y;
    return sqrt(dx * dx + dy * dy);
}

// 曼哈顿距离启发函数
double manhattan_heuristic(const vector<Point>& points, int a, int b) {
    return abs(points[a].x - points[b].x) + abs(points[a].y - points[b].y);
}

#endif // SHORTEST_PATH_H