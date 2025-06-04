#ifndef TOPOLOGICAL_SORT_H
#define TOPOLOGICAL_SORT_H

#include "graph_representation.h"
#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <algorithm>

using namespace std;

// 拓扑排序结果
struct TopologicalSortResult {
    vector<int> order;           // 拓扑排序序列
    bool has_cycle;              // 是否存在环
    vector<int> cycle;           // 如果存在环，记录环中的顶点
    
    TopologicalSortResult() : has_cycle(false) {}
    
    // 检查排序是否有效
    bool is_valid() const {
        return !has_cycle && !order.empty();
    }
    
    // 打印结果
    void print() const {
        if (has_cycle) {
            cout << "Graph has cycle, topological sort is not possible!" << endl;
            if (!cycle.empty()) {
                cout << "Cycle detected: ";
                for (size_t i = 0; i < cycle.size(); ++i) {
                    cout << cycle[i];
                    if (i < cycle.size() - 1) cout << " -> ";
                }
                cout << " -> " << cycle[0] << endl;
            }
        } else {
            cout << "Topological Sort: ";
            for (size_t i = 0; i < order.size(); ++i) {
                cout << order[i];
                if (i < order.size() - 1) cout << " -> ";
            }
            cout << endl;
        }
    }
};

// Kahn算法实现（基于入度）
template<typename T>
class KahnTopologicalSort {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    KahnTopologicalSort(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 执行拓扑排序
    TopologicalSortResult sort() const {
        TopologicalSortResult result;
        int vertices = graph.get_vertex_count();
        
        if (vertices == 0) {
            return result;
        }
        
        // 计算每个顶点的入度
        vector<int> in_degree(vertices, 0);
        for (int u = 0; u < vertices; ++u) {
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                in_degree[edge.first]++;
            }
        }
        
        // 将所有入度为0的顶点加入队列
        queue<int> zero_in_degree;
        for (int i = 0; i < vertices; ++i) {
            if (in_degree[i] == 0) {
                zero_in_degree.push(i);
            }
        }
        
        // Kahn算法主循环
        while (!zero_in_degree.empty()) {
            int u = zero_in_degree.front();
            zero_in_degree.pop();
            result.order.push_back(u);
            
            // 移除从u出发的所有边
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                in_degree[v]--;
                
                // 如果v的入度变为0，加入队列
                if (in_degree[v] == 0) {
                    zero_in_degree.push(v);
                }
            }
        }
        
        // 检查是否存在环
        if (result.order.size() != vertices) {
            result.has_cycle = true;
            result.order.clear();
            
            // 找到环中的顶点
            for (int i = 0; i < vertices; ++i) {
                if (in_degree[i] > 0) {
                    result.cycle.push_back(i);
                }
            }
        }
        
        return result;
    }
    
    // 检查图是否为DAG（有向无环图）
    bool is_dag() const {
        auto result = sort();
        return !result.has_cycle;
    }
    
    // 获取所有可能的拓扑排序
    vector<vector<int>> all_topological_sorts() const {
        vector<vector<int>> all_sorts;
        int vertices = graph.get_vertex_count();
        
        if (vertices == 0) {
            return all_sorts;
        }
        
        vector<int> in_degree(vertices, 0);
        for (int u = 0; u < vertices; ++u) {
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                in_degree[edge.first]++;
            }
        }
        
        vector<int> current_sort;
        vector<bool> visited(vertices, false);
        
        all_topological_sorts_util(in_degree, visited, current_sort, all_sorts);
        
        return all_sorts;
    }
    
private:
    void all_topological_sorts_util(vector<int>& in_degree, vector<bool>& visited,
                                   vector<int>& current_sort, vector<vector<int>>& all_sorts) const {
        int vertices = graph.get_vertex_count();
        
        // 找到所有入度为0且未访问的顶点
        vector<int> candidates;
        for (int i = 0; i < vertices; ++i) {
            if (!visited[i] && in_degree[i] == 0) {
                candidates.push_back(i);
            }
        }
        
        // 如果没有候选顶点，检查是否完成
        if (candidates.empty()) {
            if (current_sort.size() == vertices) {
                all_sorts.push_back(current_sort);
            }
            return;
        }
        
        // 尝试每个候选顶点
        for (int u : candidates) {
            // 选择顶点u
            visited[u] = true;
            current_sort.push_back(u);
            
            // 更新入度
            vector<int> affected_vertices;
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                in_degree[v]--;
                affected_vertices.push_back(v);
            }
            
            // 递归
            all_topological_sorts_util(in_degree, visited, current_sort, all_sorts);
            
            // 回溯
            visited[u] = false;
            current_sort.pop_back();
            for (int v : affected_vertices) {
                in_degree[v]++;
            }
        }
    }
};

// 基于DFS的拓扑排序
template<typename T>
class DFSTopologicalSort {
private:
    const AdjacencyListGraph<T>& graph;
    
    enum Color { WHITE, GRAY, BLACK };
    
public:
    DFSTopologicalSort(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 执行拓扑排序
    TopologicalSortResult sort() const {
        TopologicalSortResult result;
        int vertices = graph.get_vertex_count();
        
        if (vertices == 0) {
            return result;
        }
        
        vector<Color> color(vertices, WHITE);
        stack<int> finish_stack;
        
        // 对每个未访问的顶点执行DFS
        for (int i = 0; i < vertices; ++i) {
            if (color[i] == WHITE) {
                if (!dfs_visit(i, color, finish_stack, result)) {
                    return result; // 发现环
                }
            }
        }
        
        // 从栈中弹出顶点得到拓扑排序
        while (!finish_stack.empty()) {
            result.order.push_back(finish_stack.top());
            finish_stack.pop();
        }
        
        return result;
    }
    
    // 检查图是否为DAG
    bool is_dag() const {
        auto result = sort();
        return !result.has_cycle;
    }
    
    // 获取强连通分量（使用Tarjan算法）
    vector<vector<int>> strongly_connected_components() const {
        vector<vector<int>> sccs;
        int vertices = graph.get_vertex_count();
        
        vector<int> disc(vertices, -1);
        vector<int> low(vertices, -1);
        vector<bool> on_stack(vertices, false);
        stack<int> st;
        int time = 0;
        
        for (int i = 0; i < vertices; ++i) {
            if (disc[i] == -1) {
                tarjan_scc(i, disc, low, on_stack, st, sccs, time);
            }
        }
        
        return sccs;
    }
    
private:
    bool dfs_visit(int u, vector<Color>& color, stack<int>& finish_stack,
                   TopologicalSortResult& result) const {
        color[u] = GRAY;
        
        for (const auto& edge : graph.get_adjacent_edges(u)) {
            int v = edge.first;
            
            if (color[v] == GRAY) {
                // 发现后向边，存在环
                result.has_cycle = true;
                find_cycle(u, v, result.cycle);
                return false;
            }
            
            if (color[v] == WHITE && !dfs_visit(v, color, finish_stack, result)) {
                return false;
            }
        }
        
        color[u] = BLACK;
        finish_stack.push(u);
        return true;
    }
    
    void find_cycle(int start, int end, vector<int>& cycle) const {
        // 简单的环检测，实际实现可能需要更复杂的逻辑
        cycle.push_back(start);
        cycle.push_back(end);
    }
    
    void tarjan_scc(int u, vector<int>& disc, vector<int>& low, vector<bool>& on_stack,
                   stack<int>& st, vector<vector<int>>& sccs, int& time) const {
        disc[u] = low[u] = ++time;
        st.push(u);
        on_stack[u] = true;
        
        for (const auto& edge : graph.get_adjacent_edges(u)) {
            int v = edge.first;
            
            if (disc[v] == -1) {
                tarjan_scc(v, disc, low, on_stack, st, sccs, time);
                low[u] = min(low[u], low[v]);
            } else if (on_stack[v]) {
                low[u] = min(low[u], disc[v]);
            }
        }
        
        if (low[u] == disc[u]) {
            vector<int> scc;
            int w;
            do {
                w = st.top();
                st.pop();
                on_stack[w] = false;
                scc.push_back(w);
            } while (w != u);
            sccs.push_back(scc);
        }
    }
};

// 拓扑排序的应用
template<typename T>
class TopologicalApplications {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    TopologicalApplications(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 最长路径（在DAG中）
    vector<T> longest_path(int source) const {
        int vertices = graph.get_vertex_count();
        vector<T> dist(vertices, numeric_limits<T>::lowest());
        
        // 获取拓扑排序
        KahnTopologicalSort<T> kahn(graph);
        auto topo_result = kahn.sort();
        
        if (topo_result.has_cycle) {
            return dist; // 有环，无法计算最长路径
        }
        
        dist[source] = T{};
        
        // 按拓扑顺序处理顶点
        for (int u : topo_result.order) {
            if (dist[u] != numeric_limits<T>::lowest()) {
                for (const auto& edge : graph.get_adjacent_edges(u)) {
                    int v = edge.first;
                    T weight = edge.second;
                    dist[v] = max(dist[v], dist[u] + weight);
                }
            }
        }
        
        return dist;
    }
    
    // 关键路径方法（CPM）
    struct CPMResult {
        vector<T> earliest_start;
        vector<T> latest_start;
        vector<int> critical_path;
        T project_duration;
        
        void print() const {
            cout << "Critical Path Method Results:" << endl;
            cout << "Project Duration: " << project_duration << endl;
            cout << "Critical Path: ";
            for (size_t i = 0; i < critical_path.size(); ++i) {
                cout << critical_path[i];
                if (i < critical_path.size() - 1) cout << " -> ";
            }
            cout << endl;
        }
    };
    
    CPMResult critical_path_method() const {
        CPMResult result;
        int vertices = graph.get_vertex_count();
        
        // 获取拓扑排序
        KahnTopologicalSort<T> kahn(graph);
        auto topo_result = kahn.sort();
        
        if (topo_result.has_cycle) {
            return result; // 有环，无法计算关键路径
        }
        
        result.earliest_start.resize(vertices, T{});
        result.latest_start.resize(vertices, T{});
        
        // 计算最早开始时间
        for (int u : topo_result.order) {
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                T weight = edge.second;
                result.earliest_start[v] = max(result.earliest_start[v],
                                             result.earliest_start[u] + weight);
            }
        }
        
        // 项目总时长
        result.project_duration = *max_element(result.earliest_start.begin(),
                                             result.earliest_start.end());
        
        // 计算最晚开始时间
        fill(result.latest_start.begin(), result.latest_start.end(), result.project_duration);
        
        for (auto it = topo_result.order.rbegin(); it != topo_result.order.rend(); ++it) {
            int u = *it;
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                T weight = edge.second;
                result.latest_start[u] = min(result.latest_start[u],
                                           result.latest_start[v] - weight);
            }
        }
        
        // 找到关键路径
        for (int i = 0; i < vertices; ++i) {
            if (result.earliest_start[i] == result.latest_start[i]) {
                result.critical_path.push_back(i);
            }
        }
        
        return result;
    }
    
    // 检查任务调度的可行性
    bool is_schedule_feasible(const vector<pair<int, int>>& dependencies) const {
        // 构建依赖图
        AdjacencyListGraph<int> dep_graph(graph.get_vertex_count(), true);
        
        for (const auto& dep : dependencies) {
            dep_graph.add_edge(dep.first, dep.second, 1);
        }
        
        // 检查是否为DAG
        KahnTopologicalSort<int> kahn(dep_graph);
        return kahn.is_dag();
    }
};

// 工具函数
template<typename T>
void compare_topological_algorithms(const AdjacencyListGraph<T>& graph) {
    cout << "Topological Sort Algorithm Comparison:" << endl;
    cout << "=====================================" << endl;
    
    // Kahn算法
    KahnTopologicalSort<T> kahn(graph);
    auto kahn_result = kahn.sort();
    cout << "Kahn Algorithm:" << endl;
    kahn_result.print();
    cout << endl;
    
    // DFS算法
    DFSTopologicalSort<T> dfs(graph);
    auto dfs_result = dfs.sort();
    cout << "DFS Algorithm:" << endl;
    dfs_result.print();
    cout << endl;
    
    // 验证结果一致性
    if (!kahn_result.has_cycle && !dfs_result.has_cycle) {
        cout << "Both algorithms found valid topological orders." << endl;
    } else if (kahn_result.has_cycle && dfs_result.has_cycle) {
        cout << "Both algorithms detected cycles." << endl;
    } else {
        cout << "Warning: Algorithms produced different results!" << endl;
    }
}

// 验证拓扑排序的正确性
template<typename T>
bool verify_topological_sort(const AdjacencyListGraph<T>& graph,
                            const vector<int>& topo_order) {
    int vertices = graph.get_vertex_count();
    
    // 检查是否包含所有顶点
    if (topo_order.size() != vertices) {
        return false;
    }
    
    // 创建位置映射
    vector<int> position(vertices);
    for (size_t i = 0; i < topo_order.size(); ++i) {
        position[topo_order[i]] = i;
    }
    
    // 检查所有边是否满足拓扑顺序
    for (int u = 0; u < vertices; ++u) {
        for (const auto& edge : graph.get_adjacent_edges(u)) {
            int v = edge.first;
            if (position[u] >= position[v]) {
                return false; // 违反拓扑顺序
            }
        }
    }
    
    return true;
}

#endif // TOPOLOGICAL_SORT_H