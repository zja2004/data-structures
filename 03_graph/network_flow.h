#ifndef NETWORK_FLOW_H
#define NETWORK_FLOW_H

#include "graph_representation.h"
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <unordered_map>

using namespace std;

// 网络流边
template<typename T>
struct FlowEdge {
    int from, to;
    T capacity;
    T flow;
    int reverse_edge_id;  // 反向边在邻接表中的索引
    
    FlowEdge(int f, int t, T cap, T fl = T{}, int rev = -1)
        : from(f), to(t), capacity(cap), flow(fl), reverse_edge_id(rev) {}
    
    // 获取剩余容量
    T residual_capacity() const {
        return capacity - flow;
    }
    
    // 检查是否可以增加流量
    bool can_push_flow(T amount) const {
        return residual_capacity() >= amount;
    }
    
    void print() const {
        cout << from << " -> " << to << " (" << flow << "/" << capacity << ")";
    }
};

// 网络流图
template<typename T>
class FlowNetwork {
private:
    int vertices;
    vector<vector<int>> adj;  // 邻接表，存储边的索引
    vector<FlowEdge<T>> edges;  // 所有边
    
public:
    FlowNetwork(int v) : vertices(v), adj(v) {}
    
    // 添加边
    void add_edge(int from, int to, T capacity) {
        int edge_id = edges.size();
        int reverse_edge_id = edge_id + 1;
        
        // 添加正向边
        edges.emplace_back(from, to, capacity, T{}, reverse_edge_id);
        adj[from].push_back(edge_id);
        
        // 添加反向边（容量为0）
        edges.emplace_back(to, from, T{}, T{}, edge_id);
        adj[to].push_back(reverse_edge_id);
    }
    
    // 获取顶点数
    int get_vertex_count() const {
        return vertices;
    }
    
    // 获取从顶点u出发的所有边的索引
    const vector<int>& get_adjacent_edges(int u) const {
        return adj[u];
    }
    
    // 获取边
    FlowEdge<T>& get_edge(int edge_id) {
        return edges[edge_id];
    }
    
    const FlowEdge<T>& get_edge(int edge_id) const {
        return edges[edge_id];
    }
    
    // 获取所有边
    const vector<FlowEdge<T>>& get_all_edges() const {
        return edges;
    }
    
    // 重置所有边的流量
    void reset_flow() {
        for (auto& edge : edges) {
            edge.flow = T{};
        }
    }
    
    // 打印网络
    void print() const {
        cout << "Flow Network:" << endl;
        for (size_t i = 0; i < edges.size(); i += 2) {
            const auto& edge = edges[i];
            if (edge.capacity > T{}) {
                edge.print();
                cout << endl;
            }
        }
    }
};

// 最大流结果
template<typename T>
struct MaxFlowResult {
    T max_flow_value;
    vector<FlowEdge<T>> flow_edges;  // 有流量的边
    vector<vector<int>> min_cut;     // 最小割：[S集合, T集合]
    bool found_flow;
    
    MaxFlowResult() : max_flow_value(T{}), found_flow(false) {
        min_cut.resize(2);
    }
    
    void print() const {
        cout << "Maximum Flow: " << max_flow_value << endl;
        
        if (!flow_edges.empty()) {
            cout << "Flow edges:" << endl;
            for (const auto& edge : flow_edges) {
                if (edge.flow > T{}) {
                    edge.print();
                    cout << endl;
                }
            }
        }
        
        if (!min_cut[0].empty() || !min_cut[1].empty()) {
            cout << "Minimum cut:" << endl;
            cout << "S = {";
            for (size_t i = 0; i < min_cut[0].size(); ++i) {
                cout << min_cut[0][i];
                if (i < min_cut[0].size() - 1) cout << ", ";
            }
            cout << "}" << endl;
            
            cout << "T = {";
            for (size_t i = 0; i < min_cut[1].size(); ++i) {
                cout << min_cut[1][i];
                if (i < min_cut[1].size() - 1) cout << ", ";
            }
            cout << "}" << endl;
        }
    }
};

// Ford-Fulkerson算法（使用DFS寻找增广路径）
template<typename T>
class FordFulkersonMaxFlow {
private:
    FlowNetwork<T>& network;
    
public:
    FordFulkersonMaxFlow(FlowNetwork<T>& net) : network(net) {}
    
    // 计算最大流
    MaxFlowResult<T> max_flow(int source, int sink) {
        MaxFlowResult<T> result;
        network.reset_flow();
        
        int vertices = network.get_vertex_count();
        vector<bool> visited(vertices);
        
        T total_flow = T{};
        
        // 持续寻找增广路径
        while (true) {
            fill(visited.begin(), visited.end(), false);
            T path_flow = dfs_find_path(source, sink, numeric_limits<T>::max(), visited);
            
            if (path_flow == T{}) {
                break;  // 没有更多增广路径
            }
            
            total_flow += path_flow;
        }
        
        result.max_flow_value = total_flow;
        result.found_flow = (total_flow > T{});
        
        // 收集有流量的边
        for (const auto& edge : network.get_all_edges()) {
            if (edge.flow > T{}) {
                result.flow_edges.push_back(edge);
            }
        }
        
        // 计算最小割
        compute_min_cut(source, result);
        
        return result;
    }
    
private:
    T dfs_find_path(int u, int sink, T min_capacity, vector<bool>& visited) {
        if (u == sink) {
            return min_capacity;
        }
        
        visited[u] = true;
        
        for (int edge_id : network.get_adjacent_edges(u)) {
            auto& edge = network.get_edge(edge_id);
            
            if (!visited[edge.to] && edge.residual_capacity() > T{}) {
                T bottleneck = min(min_capacity, edge.residual_capacity());
                T flow = dfs_find_path(edge.to, sink, bottleneck, visited);
                
                if (flow > T{}) {
                    // 更新正向边和反向边的流量
                    edge.flow += flow;
                    network.get_edge(edge.reverse_edge_id).flow -= flow;
                    return flow;
                }
            }
        }
        
        return T{};
    }
    
    void compute_min_cut(int source, MaxFlowResult<T>& result) {
        int vertices = network.get_vertex_count();
        vector<bool> reachable(vertices, false);
        
        // 从源点开始DFS，只走剩余容量大于0的边
        dfs_reachable(source, reachable);
        
        for (int i = 0; i < vertices; ++i) {
            if (reachable[i]) {
                result.min_cut[0].push_back(i);  // S集合
            } else {
                result.min_cut[1].push_back(i);  // T集合
            }
        }
    }
    
    void dfs_reachable(int u, vector<bool>& reachable) {
        reachable[u] = true;
        
        for (int edge_id : network.get_adjacent_edges(u)) {
            const auto& edge = network.get_edge(edge_id);
            
            if (!reachable[edge.to] && edge.residual_capacity() > T{}) {
                dfs_reachable(edge.to, reachable);
            }
        }
    }
};

// Edmonds-Karp算法（使用BFS寻找增广路径）
template<typename T>
class EdmondsKarpMaxFlow {
private:
    FlowNetwork<T>& network;
    
public:
    EdmondsKarpMaxFlow(FlowNetwork<T>& net) : network(net) {}
    
    // 计算最大流
    MaxFlowResult<T> max_flow(int source, int sink) {
        MaxFlowResult<T> result;
        network.reset_flow();
        
        T total_flow = T{};
        
        // 持续寻找增广路径
        while (true) {
            auto path_info = bfs_find_path(source, sink);
            
            if (path_info.first == T{}) {
                break;  // 没有更多增广路径
            }
            
            // 沿路径更新流量
            update_flow_along_path(path_info.second, path_info.first);
            total_flow += path_info.first;
        }
        
        result.max_flow_value = total_flow;
        result.found_flow = (total_flow > T{});
        
        // 收集有流量的边
        for (const auto& edge : network.get_all_edges()) {
            if (edge.flow > T{}) {
                result.flow_edges.push_back(edge);
            }
        }
        
        // 计算最小割
        compute_min_cut(source, result);
        
        return result;
    }
    
private:
    pair<T, vector<int>> bfs_find_path(int source, int sink) {
        int vertices = network.get_vertex_count();
        vector<int> parent(vertices, -1);
        vector<int> parent_edge(vertices, -1);
        vector<bool> visited(vertices, false);
        
        queue<int> q;
        q.push(source);
        visited[source] = true;
        
        while (!q.empty() && !visited[sink]) {
            int u = q.front();
            q.pop();
            
            for (int edge_id : network.get_adjacent_edges(u)) {
                const auto& edge = network.get_edge(edge_id);
                
                if (!visited[edge.to] && edge.residual_capacity() > T{}) {
                    visited[edge.to] = true;
                    parent[edge.to] = u;
                    parent_edge[edge.to] = edge_id;
                    q.push(edge.to);
                }
            }
        }
        
        if (!visited[sink]) {
            return {T{}, {}};
        }
        
        // 重构路径并找到瓶颈容量
        vector<int> path_edges;
        T bottleneck = numeric_limits<T>::max();
        
        int current = sink;
        while (parent[current] != -1) {
            int edge_id = parent_edge[current];
            path_edges.push_back(edge_id);
            bottleneck = min(bottleneck, network.get_edge(edge_id).residual_capacity());
            current = parent[current];
        }
        
        reverse(path_edges.begin(), path_edges.end());
        return {bottleneck, path_edges};
    }
    
    void update_flow_along_path(const vector<int>& path_edges, T flow) {
        for (int edge_id : path_edges) {
            auto& edge = network.get_edge(edge_id);
            edge.flow += flow;
            network.get_edge(edge.reverse_edge_id).flow -= flow;
        }
    }
    
    void compute_min_cut(int source, MaxFlowResult<T>& result) {
        int vertices = network.get_vertex_count();
        vector<bool> reachable(vertices, false);
        
        // 从源点开始BFS，只走剩余容量大于0的边
        queue<int> q;
        q.push(source);
        reachable[source] = true;
        
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            
            for (int edge_id : network.get_adjacent_edges(u)) {
                const auto& edge = network.get_edge(edge_id);
                
                if (!reachable[edge.to] && edge.residual_capacity() > T{}) {
                    reachable[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }
        
        for (int i = 0; i < vertices; ++i) {
            if (reachable[i]) {
                result.min_cut[0].push_back(i);  // S集合
            } else {
                result.min_cut[1].push_back(i);  // T集合
            }
        }
    }
};

// Dinic算法
template<typename T>
class DinicMaxFlow {
private:
    FlowNetwork<T>& network;
    
public:
    DinicMaxFlow(FlowNetwork<T>& net) : network(net) {}
    
    // 计算最大流
    MaxFlowResult<T> max_flow(int source, int sink) {
        MaxFlowResult<T> result;
        network.reset_flow();
        
        T total_flow = T{};
        
        // 持续构建层次图并寻找阻塞流
        while (true) {
            vector<int> level = build_level_graph(source, sink);
            
            if (level[sink] == -1) {
                break;  // 无法到达汇点
            }
            
            // 寻找阻塞流
            vector<int> iter(network.get_vertex_count(), 0);
            T blocking_flow;
            
            do {
                blocking_flow = dfs_blocking_flow(source, sink, numeric_limits<T>::max(), level, iter);
                total_flow += blocking_flow;
            } while (blocking_flow > T{});
        }
        
        result.max_flow_value = total_flow;
        result.found_flow = (total_flow > T{});
        
        // 收集有流量的边
        for (const auto& edge : network.get_all_edges()) {
            if (edge.flow > T{}) {
                result.flow_edges.push_back(edge);
            }
        }
        
        // 计算最小割
        compute_min_cut(source, result);
        
        return result;
    }
    
private:
    vector<int> build_level_graph(int source, int sink) {
        int vertices = network.get_vertex_count();
        vector<int> level(vertices, -1);
        
        queue<int> q;
        q.push(source);
        level[source] = 0;
        
        while (!q.empty() && level[sink] == -1) {
            int u = q.front();
            q.pop();
            
            for (int edge_id : network.get_adjacent_edges(u)) {
                const auto& edge = network.get_edge(edge_id);
                
                if (level[edge.to] == -1 && edge.residual_capacity() > T{}) {
                    level[edge.to] = level[u] + 1;
                    q.push(edge.to);
                }
            }
        }
        
        return level;
    }
    
    T dfs_blocking_flow(int u, int sink, T min_capacity, const vector<int>& level, vector<int>& iter) {
        if (u == sink) {
            return min_capacity;
        }
        
        for (int& i = iter[u]; i < network.get_adjacent_edges(u).size(); ++i) {
            int edge_id = network.get_adjacent_edges(u)[i];
            auto& edge = network.get_edge(edge_id);
            
            if (level[edge.to] == level[u] + 1 && edge.residual_capacity() > T{}) {
                T bottleneck = min(min_capacity, edge.residual_capacity());
                T flow = dfs_blocking_flow(edge.to, sink, bottleneck, level, iter);
                
                if (flow > T{}) {
                    edge.flow += flow;
                    network.get_edge(edge.reverse_edge_id).flow -= flow;
                    return flow;
                }
            }
        }
        
        return T{};
    }
    
    void compute_min_cut(int source, MaxFlowResult<T>& result) {
        int vertices = network.get_vertex_count();
        vector<bool> reachable(vertices, false);
        
        // 从源点开始DFS，只走剩余容量大于0的边
        dfs_reachable(source, reachable);
        
        for (int i = 0; i < vertices; ++i) {
            if (reachable[i]) {
                result.min_cut[0].push_back(i);  // S集合
            } else {
                result.min_cut[1].push_back(i);  // T集合
            }
        }
    }
    
    void dfs_reachable(int u, vector<bool>& reachable) {
        reachable[u] = true;
        
        for (int edge_id : network.get_adjacent_edges(u)) {
            const auto& edge = network.get_edge(edge_id);
            
            if (!reachable[edge.to] && edge.residual_capacity() > T{}) {
                dfs_reachable(edge.to, reachable);
            }
        }
    }
};

// 最小费用最大流
template<typename T, typename CostType>
class MinCostMaxFlow {
private:
    struct CostFlowEdge {
        int from, to;
        T capacity, flow;
        CostType cost;
        int reverse_edge_id;
        
        CostFlowEdge(int f, int t, T cap, CostType c, int rev = -1)
            : from(f), to(t), capacity(cap), flow(T{}), cost(c), reverse_edge_id(rev) {}
        
        T residual_capacity() const {
            return capacity - flow;
        }
    };
    
    int vertices;
    vector<vector<int>> adj;
    vector<CostFlowEdge> edges;
    
public:
    MinCostMaxFlow(int v) : vertices(v), adj(v) {}
    
    void add_edge(int from, int to, T capacity, CostType cost) {
        int edge_id = edges.size();
        int reverse_edge_id = edge_id + 1;
        
        edges.emplace_back(from, to, capacity, cost, reverse_edge_id);
        adj[from].push_back(edge_id);
        
        edges.emplace_back(to, from, T{}, -cost, edge_id);
        adj[to].push_back(reverse_edge_id);
    }
    
    struct MinCostMaxFlowResult {
        T max_flow;
        CostType min_cost;
        bool found_flow;
        
        void print() const {
            cout << "Maximum Flow: " << max_flow << endl;
            cout << "Minimum Cost: " << min_cost << endl;
        }
    };
    
    MinCostMaxFlowResult min_cost_max_flow(int source, int sink) {
        MinCostMaxFlowResult result;
        result.max_flow = T{};
        result.min_cost = CostType{};
        result.found_flow = false;
        
        // 使用SPFA算法寻找最短路径
        while (true) {
            vector<CostType> dist(vertices, numeric_limits<CostType>::max());
            vector<int> parent(vertices, -1);
            vector<int> parent_edge(vertices, -1);
            vector<bool> in_queue(vertices, false);
            
            queue<int> q;
            dist[source] = CostType{};
            q.push(source);
            in_queue[source] = true;
            
            // SPFA算法
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                in_queue[u] = false;
                
                for (int edge_id : adj[u]) {
                    const auto& edge = edges[edge_id];
                    
                    if (edge.residual_capacity() > T{} && 
                        dist[u] + edge.cost < dist[edge.to]) {
                        dist[edge.to] = dist[u] + edge.cost;
                        parent[edge.to] = u;
                        parent_edge[edge.to] = edge_id;
                        
                        if (!in_queue[edge.to]) {
                            q.push(edge.to);
                            in_queue[edge.to] = true;
                        }
                    }
                }
            }
            
            if (dist[sink] == numeric_limits<CostType>::max()) {
                break;  // 无法到达汇点
            }
            
            // 找到瓶颈容量
            T bottleneck = numeric_limits<T>::max();
            int current = sink;
            while (parent[current] != -1) {
                int edge_id = parent_edge[current];
                bottleneck = min(bottleneck, edges[edge_id].residual_capacity());
                current = parent[current];
            }
            
            // 更新流量
            current = sink;
            while (parent[current] != -1) {
                int edge_id = parent_edge[current];
                edges[edge_id].flow += bottleneck;
                edges[edges[edge_id].reverse_edge_id].flow -= bottleneck;
                current = parent[current];
            }
            
            result.max_flow += bottleneck;
            result.min_cost += bottleneck * dist[sink];
            result.found_flow = true;
        }
        
        return result;
    }
};

// 工具函数
template<typename T>
void compare_max_flow_algorithms(FlowNetwork<T>& network, int source, int sink) {
    cout << "Max Flow Algorithm Comparison:" << endl;
    cout << "==============================" << endl;
    
    // Ford-Fulkerson算法
    FordFulkersonMaxFlow<T> ford_fulkerson(network);
    auto ff_result = ford_fulkerson.max_flow(source, sink);
    cout << "Ford-Fulkerson: " << ff_result.max_flow_value << endl;
    
    // Edmonds-Karp算法
    EdmondsKarpMaxFlow<T> edmonds_karp(network);
    auto ek_result = edmonds_karp.max_flow(source, sink);
    cout << "Edmonds-Karp: " << ek_result.max_flow_value << endl;
    
    // Dinic算法
    DinicMaxFlow<T> dinic(network);
    auto dinic_result = dinic.max_flow(source, sink);
    cout << "Dinic: " << dinic_result.max_flow_value << endl;
    
    // 验证结果一致性
    if (ff_result.max_flow_value == ek_result.max_flow_value && 
        ek_result.max_flow_value == dinic_result.max_flow_value) {
        cout << "All algorithms found the same maximum flow." << endl;
    } else {
        cout << "Warning: Algorithms produced different results!" << endl;
    }
}

#endif // NETWORK_FLOW_H