#ifndef STRONGLY_CONNECTED_COMPONENTS_H
#define STRONGLY_CONNECTED_COMPONENTS_H

#include "graph_representation.h"
#include <vector>
#include <stack>
#include <algorithm>
#include <unordered_set>

using namespace std;

// 强连通分量结果
struct SCCResult {
    vector<vector<int>> components;     // 强连通分量列表
    vector<int> component_id;           // 每个顶点所属的分量ID
    int num_components;                 // 分量数量
    
    SCCResult(int vertices = 0) : component_id(vertices, -1), num_components(0) {}
    
    // 添加强连通分量
    void add_component(const vector<int>& component) {
        components.push_back(component);
        for (int vertex : component) {
            component_id[vertex] = num_components;
        }
        num_components++;
    }
    
    // 检查两个顶点是否在同一强连通分量中
    bool in_same_component(int u, int v) const {
        return component_id[u] != -1 && component_id[u] == component_id[v];
    }
    
    // 获取顶点所在的强连通分量
    vector<int> get_component(int vertex) const {
        if (component_id[vertex] == -1) {
            return {};
        }
        return components[component_id[vertex]];
    }
    
    // 打印结果
    void print() const {
        cout << "Strongly Connected Components:" << endl;
        cout << "Number of components: " << num_components << endl;
        
        for (size_t i = 0; i < components.size(); ++i) {
            cout << "Component " << i << ": ";
            for (size_t j = 0; j < components[i].size(); ++j) {
                cout << components[i][j];
                if (j < components[i].size() - 1) cout << ", ";
            }
            cout << endl;
        }
    }
    
    // 获取统计信息
    struct Statistics {
        int total_components;
        int largest_component_size;
        int smallest_component_size;
        double average_component_size;
        vector<int> component_sizes;
        
        void print() const {
            cout << "SCC Statistics:" << endl;
            cout << "Total components: " << total_components << endl;
            cout << "Largest component size: " << largest_component_size << endl;
            cout << "Smallest component size: " << smallest_component_size << endl;
            cout << "Average component size: " << average_component_size << endl;
        }
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_components = num_components;
        
        if (num_components == 0) {
            stats.largest_component_size = 0;
            stats.smallest_component_size = 0;
            stats.average_component_size = 0.0;
            return stats;
        }
        
        stats.component_sizes.reserve(components.size());
        for (const auto& comp : components) {
            stats.component_sizes.push_back(comp.size());
        }
        
        stats.largest_component_size = *max_element(stats.component_sizes.begin(),
                                                   stats.component_sizes.end());
        stats.smallest_component_size = *min_element(stats.component_sizes.begin(),
                                                    stats.component_sizes.end());
        
        int total_vertices = 0;
        for (int size : stats.component_sizes) {
            total_vertices += size;
        }
        stats.average_component_size = static_cast<double>(total_vertices) / num_components;
        
        return stats;
    }
};

// Tarjan算法实现
template<typename T>
class TarjanSCC {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    TarjanSCC(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 查找强连通分量
    SCCResult find_scc() const {
        int vertices = graph.get_vertex_count();
        SCCResult result(vertices);
        
        if (vertices == 0) {
            return result;
        }
        
        vector<int> disc(vertices, -1);        // 发现时间
        vector<int> low(vertices, -1);         // 最小可达发现时间
        vector<bool> on_stack(vertices, false); // 是否在栈中
        stack<int> st;                         // DFS栈
        int time = 0;
        
        // 对每个未访问的顶点执行DFS
        for (int i = 0; i < vertices; ++i) {
            if (disc[i] == -1) {
                tarjan_dfs(i, disc, low, on_stack, st, result, time);
            }
        }
        
        return result;
    }
    
    // 检查图是否强连通
    bool is_strongly_connected() const {
        auto result = find_scc();
        return result.num_components == 1;
    }
    
    // 查找桥（割边）
    vector<pair<int, int>> find_bridges() const {
        vector<pair<int, int>> bridges;
        int vertices = graph.get_vertex_count();
        
        vector<int> disc(vertices, -1);
        vector<int> low(vertices, -1);
        vector<int> parent(vertices, -1);
        int time = 0;
        
        for (int i = 0; i < vertices; ++i) {
            if (disc[i] == -1) {
                bridge_dfs(i, disc, low, parent, bridges, time);
            }
        }
        
        return bridges;
    }
    
    // 查找割点
    vector<int> find_articulation_points() const {
        vector<int> articulation_points;
        int vertices = graph.get_vertex_count();
        
        vector<int> disc(vertices, -1);
        vector<int> low(vertices, -1);
        vector<int> parent(vertices, -1);
        vector<bool> is_articulation(vertices, false);
        int time = 0;
        
        for (int i = 0; i < vertices; ++i) {
            if (disc[i] == -1) {
                articulation_dfs(i, disc, low, parent, is_articulation, time);
            }
        }
        
        for (int i = 0; i < vertices; ++i) {
            if (is_articulation[i]) {
                articulation_points.push_back(i);
            }
        }
        
        return articulation_points;
    }
    
private:
    void tarjan_dfs(int u, vector<int>& disc, vector<int>& low, vector<bool>& on_stack,
                   stack<int>& st, SCCResult& result, int& time) const {
        // 初始化发现时间和low值
        disc[u] = low[u] = ++time;
        st.push(u);
        on_stack[u] = true;
        
        // 遍历所有相邻顶点
        for (const auto& edge : graph.get_adjacent_edges(u)) {
            int v = edge.first;
            
            if (disc[v] == -1) {
                // 如果v未被访问，递归访问
                tarjan_dfs(v, disc, low, on_stack, st, result, time);
                low[u] = min(low[u], low[v]);
            } else if (on_stack[v]) {
                // 如果v在栈中，更新low值
                low[u] = min(low[u], disc[v]);
            }
        }
        
        // 如果u是强连通分量的根
        if (low[u] == disc[u]) {
            vector<int> component;
            int w;
            do {
                w = st.top();
                st.pop();
                on_stack[w] = false;
                component.push_back(w);
            } while (w != u);
            
            result.add_component(component);
        }
    }
    
    void bridge_dfs(int u, vector<int>& disc, vector<int>& low, vector<int>& parent,
                   vector<pair<int, int>>& bridges, int& time) const {
        disc[u] = low[u] = ++time;
        
        for (const auto& edge : graph.get_adjacent_edges(u)) {
            int v = edge.first;
            
            if (disc[v] == -1) {
                parent[v] = u;
                bridge_dfs(v, disc, low, parent, bridges, time);
                
                low[u] = min(low[u], low[v]);
                
                // 如果low[v] > disc[u]，则(u,v)是桥
                if (low[v] > disc[u]) {
                    bridges.push_back({u, v});
                }
            } else if (v != parent[u]) {
                low[u] = min(low[u], disc[v]);
            }
        }
    }
    
    void articulation_dfs(int u, vector<int>& disc, vector<int>& low, vector<int>& parent,
                         vector<bool>& is_articulation, int& time) const {
        int children = 0;
        disc[u] = low[u] = ++time;
        
        for (const auto& edge : graph.get_adjacent_edges(u)) {
            int v = edge.first;
            
            if (disc[v] == -1) {
                children++;
                parent[v] = u;
                articulation_dfs(v, disc, low, parent, is_articulation, time);
                
                low[u] = min(low[u], low[v]);
                
                // 检查割点条件
                if (parent[u] == -1 && children > 1) {
                    is_articulation[u] = true;
                }
                
                if (parent[u] != -1 && low[v] >= disc[u]) {
                    is_articulation[u] = true;
                }
            } else if (v != parent[u]) {
                low[u] = min(low[u], disc[v]);
            }
        }
    }
};

// Kosaraju算法实现
template<typename T>
class KosarajuSCC {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    KosarajuSCC(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 查找强连通分量
    SCCResult find_scc() const {
        int vertices = graph.get_vertex_count();
        SCCResult result(vertices);
        
        if (vertices == 0) {
            return result;
        }
        
        // 第一步：在原图上执行DFS，记录完成时间
        vector<bool> visited(vertices, false);
        stack<int> finish_order;
        
        for (int i = 0; i < vertices; ++i) {
            if (!visited[i]) {
                dfs_first_pass(i, visited, finish_order);
            }
        }
        
        // 第二步：构建转置图
        AdjacencyListGraph<T> transpose_graph = create_transpose();
        
        // 第三步：在转置图上按完成时间的逆序执行DFS
        fill(visited.begin(), visited.end(), false);
        
        while (!finish_order.empty()) {
            int v = finish_order.top();
            finish_order.pop();
            
            if (!visited[v]) {
                vector<int> component;
                dfs_second_pass(v, visited, component, transpose_graph);
                if (!component.empty()) {
                    result.add_component(component);
                }
            }
        }
        
        return result;
    }
    
    // 检查图是否强连通
    bool is_strongly_connected() const {
        auto result = find_scc();
        return result.num_components == 1;
    }
    
private:
    void dfs_first_pass(int u, vector<bool>& visited, stack<int>& finish_order) const {
        visited[u] = true;
        
        for (const auto& edge : graph.get_adjacent_edges(u)) {
            int v = edge.first;
            if (!visited[v]) {
                dfs_first_pass(v, visited, finish_order);
            }
        }
        
        finish_order.push(u);
    }
    
    void dfs_second_pass(int u, vector<bool>& visited, vector<int>& component,
                        const AdjacencyListGraph<T>& transpose_graph) const {
        visited[u] = true;
        component.push_back(u);
        
        for (const auto& edge : transpose_graph.get_adjacent_edges(u)) {
            int v = edge.first;
            if (!visited[v]) {
                dfs_second_pass(v, visited, component, transpose_graph);
            }
        }
    }
    
    AdjacencyListGraph<T> create_transpose() const {
        int vertices = graph.get_vertex_count();
        AdjacencyListGraph<T> transpose(vertices, true);
        
        for (int u = 0; u < vertices; ++u) {
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                T weight = edge.second;
                transpose.add_edge(v, u, weight);
            }
        }
        
        return transpose;
    }
};

// 强连通分量的应用
template<typename T>
class SCCApplications {
private:
    const AdjacencyListGraph<T>& graph;
    
public:
    SCCApplications(const AdjacencyListGraph<T>& g) : graph(g) {}
    
    // 构建凝聚图（condensation graph）
    AdjacencyListGraph<T> build_condensation_graph() const {
        TarjanSCC<T> tarjan(graph);
        auto scc_result = tarjan.find_scc();
        
        AdjacencyListGraph<T> condensation(scc_result.num_components, true);
        unordered_set<pair<int, int>, hash<pair<int, int>>> added_edges;
        
        int vertices = graph.get_vertex_count();
        for (int u = 0; u < vertices; ++u) {
            for (const auto& edge : graph.get_adjacent_edges(u)) {
                int v = edge.first;
                T weight = edge.second;
                
                int comp_u = scc_result.component_id[u];
                int comp_v = scc_result.component_id[v];
                
                if (comp_u != comp_v) {
                    pair<int, int> edge_pair = {comp_u, comp_v};
                    if (added_edges.find(edge_pair) == added_edges.end()) {
                        condensation.add_edge(comp_u, comp_v, weight);
                        added_edges.insert(edge_pair);
                    }
                }
            }
        }
        
        return condensation;
    }
    
    // 检查图的可达性
    vector<vector<bool>> compute_reachability() const {
        int vertices = graph.get_vertex_count();
        vector<vector<bool>> reachable(vertices, vector<bool>(vertices, false));
        
        TarjanSCC<T> tarjan(graph);
        auto scc_result = tarjan.find_scc();
        
        // 同一强连通分量内的顶点互相可达
        for (const auto& component : scc_result.components) {
            for (int u : component) {
                for (int v : component) {
                    reachable[u][v] = true;
                }
            }
        }
        
        // 构建凝聚图并计算可达性
        auto condensation = build_condensation_graph();
        int num_components = scc_result.num_components;
        vector<vector<bool>> comp_reachable(num_components, vector<bool>(num_components, false));
        
        // 使用Floyd-Warshall算法计算凝聚图的可达性
        for (int i = 0; i < num_components; ++i) {
            comp_reachable[i][i] = true;
        }
        
        for (int i = 0; i < num_components; ++i) {
            for (const auto& edge : condensation.get_adjacent_edges(i)) {
                comp_reachable[i][edge.first] = true;
            }
        }
        
        for (int k = 0; k < num_components; ++k) {
            for (int i = 0; i < num_components; ++i) {
                for (int j = 0; j < num_components; ++j) {
                    comp_reachable[i][j] = comp_reachable[i][j] || 
                                         (comp_reachable[i][k] && comp_reachable[k][j]);
                }
            }
        }
        
        // 将分量间的可达性映射回原顶点
        for (int u = 0; u < vertices; ++u) {
            for (int v = 0; v < vertices; ++v) {
                int comp_u = scc_result.component_id[u];
                int comp_v = scc_result.component_id[v];
                if (comp_reachable[comp_u][comp_v]) {
                    reachable[u][v] = true;
                }
            }
        }
        
        return reachable;
    }
    
    // 2-SAT问题求解
    struct SATResult {
        bool satisfiable;
        vector<bool> assignment;
        
        void print() const {
            if (satisfiable) {
                cout << "2-SAT is satisfiable." << endl;
                cout << "Assignment: ";
                for (size_t i = 0; i < assignment.size(); ++i) {
                    cout << "x" << i << "=" << (assignment[i] ? "true" : "false");
                    if (i < assignment.size() - 1) cout << ", ";
                }
                cout << endl;
            } else {
                cout << "2-SAT is not satisfiable." << endl;
            }
        }
    };
    
    SATResult solve_2sat(int num_variables) const {
        SATResult result;
        result.satisfiable = false;
        
        // 假设图已经是2-SAT的蕴含图
        TarjanSCC<T> tarjan(graph);
        auto scc_result = tarjan.find_scc();
        
        // 检查是否存在变量x和¬x在同一强连通分量中
        for (int i = 0; i < num_variables; ++i) {
            int x = i;              // 变量x
            int not_x = i + num_variables;  // 变量¬x
            
            if (scc_result.in_same_component(x, not_x)) {
                return result; // 不可满足
            }
        }
        
        result.satisfiable = true;
        result.assignment.resize(num_variables);
        
        // 根据拓扑顺序分配真值
        for (int i = 0; i < num_variables; ++i) {
            int x = i;
            int not_x = i + num_variables;
            
            // 如果¬x的分量ID小于x的分量ID，则x为真
            result.assignment[i] = scc_result.component_id[not_x] < scc_result.component_id[x];
        }
        
        return result;
    }
};

// 工具函数
template<typename T>
void compare_scc_algorithms(const AdjacencyListGraph<T>& graph) {
    cout << "SCC Algorithm Comparison:" << endl;
    cout << "========================" << endl;
    
    // Tarjan算法
    TarjanSCC<T> tarjan(graph);
    auto tarjan_result = tarjan.find_scc();
    cout << "Tarjan Algorithm:" << endl;
    cout << "Number of SCCs: " << tarjan_result.num_components << endl;
    
    // Kosaraju算法
    KosarajuSCC<T> kosaraju(graph);
    auto kosaraju_result = kosaraju.find_scc();
    cout << "Kosaraju Algorithm:" << endl;
    cout << "Number of SCCs: " << kosaraju_result.num_components << endl;
    
    // 验证结果一致性
    if (tarjan_result.num_components == kosaraju_result.num_components) {
        cout << "Both algorithms found the same number of SCCs." << endl;
    } else {
        cout << "Warning: Algorithms produced different results!" << endl;
    }
}

// 验证强连通分量的正确性
template<typename T>
bool verify_scc(const AdjacencyListGraph<T>& graph, const SCCResult& result) {
    int vertices = graph.get_vertex_count();
    
    // 检查每个顶点是否被分配到唯一的分量
    vector<bool> assigned(vertices, false);
    for (const auto& component : result.components) {
        for (int vertex : component) {
            if (vertex < 0 || vertex >= vertices || assigned[vertex]) {
                return false;
            }
            assigned[vertex] = true;
        }
    }
    
    // 检查是否所有顶点都被分配
    for (bool is_assigned : assigned) {
        if (!is_assigned) {
            return false;
        }
    }
    
    // 检查强连通性
    for (const auto& component : result.components) {
        for (int u : component) {
            for (int v : component) {
                if (u != v && !is_reachable(graph, u, v)) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

template<typename T>
bool is_reachable(const AdjacencyListGraph<T>& graph, int start, int target) {
    int vertices = graph.get_vertex_count();
    vector<bool> visited(vertices, false);
    
    function<bool(int)> dfs = [&](int u) -> bool {
        if (u == target) return true;
        visited[u] = true;
        
        for (const auto& edge : graph.get_adjacent_edges(u)) {
            int v = edge.first;
            if (!visited[v] && dfs(v)) {
                return true;
            }
        }
        return false;
    };
    
    return dfs(start);
}

#endif // STRONGLY_CONNECTED_COMPONENTS_H