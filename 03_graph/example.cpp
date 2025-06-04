#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>

#include "graph_representation.h"
#include "graph_traversal.h"
#include "shortest_path.h"
#include "minimum_spanning_tree.h"
#include "topological_sort.h"
#include "strongly_connected_components.h"
#include "network_flow.h"

using namespace std;
using namespace chrono;

// 测试图的表示方法
void test_graph_representation() {
    cout << "\n=== 图的表示方法测试 ===" << endl;
    
    // 测试邻接矩阵
    cout << "\n1. 邻接矩阵表示:" << endl;
    AdjacencyMatrixGraph<int> matrix_graph(5, false);
    matrix_graph.add_edge(0, 1, 10);
    matrix_graph.add_edge(0, 2, 6);
    matrix_graph.add_edge(0, 3, 5);
    matrix_graph.add_edge(1, 3, 15);
    matrix_graph.add_edge(2, 3, 4);
    
    cout << "图的邻接矩阵:" << endl;
    matrix_graph.print();
    cout << "顶点数: " << matrix_graph.get_vertex_count() << endl;
    cout << "边数: " << matrix_graph.get_edge_count() << endl;
    
    // 测试邻接表
    cout << "\n2. 邻接表表示:" << endl;
    AdjacencyListGraph<int> list_graph(5, false);
    list_graph.add_edge(0, 1, 10);
    list_graph.add_edge(0, 2, 6);
    list_graph.add_edge(0, 3, 5);
    list_graph.add_edge(1, 3, 15);
    list_graph.add_edge(2, 3, 4);
    
    cout << "图的邻接表:" << endl;
    list_graph.print();
    cout << "顶点数: " << list_graph.get_vertex_count() << endl;
    cout << "边数: " << list_graph.get_edge_count() << endl;
    
    // 测试边列表
    cout << "\n3. 边列表表示:" << endl;
    EdgeListGraph<int> edge_graph(5, false);
    edge_graph.add_edge(0, 1, 10);
    edge_graph.add_edge(0, 2, 6);
    edge_graph.add_edge(0, 3, 5);
    edge_graph.add_edge(1, 3, 15);
    edge_graph.add_edge(2, 3, 4);
    
    cout << "图的边列表:" << endl;
    edge_graph.print();
    cout << "顶点数: " << edge_graph.get_vertex_count() << endl;
    cout << "边数: " << edge_graph.get_edge_count() << endl;
    
    // 测试特殊图的创建
    cout << "\n4. 特殊图的创建:" << endl;
    auto complete_graph = create_complete_graph<int>(4, 1);
    cout << "完全图 K4:" << endl;
    complete_graph.print();
    
    auto cycle_graph = create_cycle_graph<int>(5, 1);
    cout << "\n环图 C5:" << endl;
    cycle_graph.print();
    
    auto path_graph = create_path_graph<int>(4, 1);
    cout << "\n路径图 P4:" << endl;
    path_graph.print();
}

// 测试图的遍历算法
void test_graph_traversal() {
    cout << "\n=== 图的遍历算法测试 ===" << endl;
    
    // 创建测试图
    AdjacencyListGraph<int> graph(6, false);
    graph.add_edge(0, 1, 1);
    graph.add_edge(0, 2, 1);
    graph.add_edge(1, 3, 1);
    graph.add_edge(2, 4, 1);
    graph.add_edge(3, 5, 1);
    graph.add_edge(4, 5, 1);
    
    cout << "测试图:" << endl;
    graph.print();
    
    // 深度优先搜索
    cout << "\n1. 深度优先搜索 (DFS):" << endl;
    DepthFirstSearch<int> dfs(graph);
    
    auto dfs_result = dfs.traverse(0);
    cout << "DFS遍历序列: ";
    print_traversal_result(dfs_result);
    
    cout << "\n路径查找 (0 -> 5): ";
    auto path = dfs.find_path(0, 5);
    print_path(path);
    
    cout << "\n连通分量:" << endl;
    auto components = dfs.find_connected_components();
    print_connected_components(components);
    
    cout << "\n环检测: " << (dfs.has_cycle() ? "存在环" : "无环") << endl;
    
    // 广度优先搜索
    cout << "\n2. 广度优先搜索 (BFS):" << endl;
    BreadthFirstSearch<int> bfs(graph);
    
    auto bfs_result = bfs.traverse(0);
    cout << "BFS遍历序列: ";
    print_traversal_result(bfs_result);
    
    cout << "\n最短路径 (0 -> 5): ";
    auto shortest_path = bfs.shortest_path(0, 5);
    print_path(shortest_path);
    
    cout << "\n距离计算:" << endl;
    auto distances = bfs.calculate_distances(0);
    for (size_t i = 0; i < distances.size(); ++i) {
        cout << "距离到顶点 " << i << ": " << distances[i] << endl;
    }
    
    cout << "\n层序遍历:" << endl;
    auto levels = bfs.level_order_traversal(0);
    print_level_order_traversal(levels);
    
    cout << "\n二分图检测: " << (bfs.is_bipartite() ? "是二分图" : "不是二分图") << endl;
    
    // 图分析
    cout << "\n3. 图分析:" << endl;
    auto stats = analyze_graph(graph);
    cout << "顶点数: " << stats.vertex_count << endl;
    cout << "边数: " << stats.edge_count << endl;
    cout << "连通分量数: " << stats.connected_components << endl;
    cout << "是否连通: " << (stats.is_connected ? "是" : "否") << endl;
    cout << "是否有环: " << (stats.has_cycle ? "是" : "否") << endl;
    cout << "是否二分图: " << (stats.is_bipartite ? "是" : "否") << endl;
    cout << "平均度数: " << fixed << setprecision(2) << stats.average_degree << endl;
}

// 测试最短路径算法
void test_shortest_path() {
    cout << "\n=== 最短路径算法测试 ===" << endl;
    
    // 创建测试图
    AdjacencyListGraph<int> graph(5, true);
    graph.add_edge(0, 1, 10);
    graph.add_edge(0, 4, 5);
    graph.add_edge(1, 2, 1);
    graph.add_edge(1, 4, 2);
    graph.add_edge(2, 3, 4);
    graph.add_edge(3, 2, 6);
    graph.add_edge(3, 0, 7);
    graph.add_edge(4, 1, 3);
    graph.add_edge(4, 2, 9);
    graph.add_edge(4, 3, 2);
    
    cout << "测试图:" << endl;
    graph.print();
    
    // Dijkstra算法
    cout << "\n1. Dijkstra算法:" << endl;
    DijkstraAlgorithm<int> dijkstra(graph);
    auto dijkstra_result = dijkstra.shortest_path(0);
    dijkstra_result.print();
    
    cout << "\n单源最短路径 (0 -> 3): ";
    auto single_path = dijkstra.shortest_path(0, 3);
    print_shortest_path_result(single_path);
    
    // Bellman-Ford算法
    cout << "\n2. Bellman-Ford算法:" << endl;
    BellmanFordAlgorithm<int> bellman_ford(graph);
    auto bf_result = bellman_ford.shortest_path(0);
    bf_result.print();
    
    // Floyd-Warshall算法
    cout << "\n3. Floyd-Warshall算法:" << endl;
    FloydWarshallAlgorithm<int> floyd_warshall(graph);
    auto fw_result = floyd_warshall.all_pairs_shortest_path();
    fw_result.print();
    
    // A*算法（使用简单的启发式函数）
    cout << "\n4. A*算法:" << endl;
    AStarAlgorithm<int> astar(graph);
    auto astar_result = astar.shortest_path(0, 3, manhattan_heuristic<int>);
    astar_result.print();
    
    // Johnson算法
    cout << "\n5. Johnson算法:" << endl;
    JohnsonAlgorithm<int> johnson(graph);
    auto johnson_result = johnson.all_pairs_shortest_path();
    johnson_result.print();
}

// 测试最小生成树算法
void test_minimum_spanning_tree() {
    cout << "\n=== 最小生成树算法测试 ===" << endl;
    
    // 创建测试图
    AdjacencyListGraph<int> graph(6, false);
    graph.add_edge(0, 1, 4);
    graph.add_edge(0, 2, 4);
    graph.add_edge(1, 2, 2);
    graph.add_edge(1, 0, 4);
    graph.add_edge(2, 0, 4);
    graph.add_edge(2, 1, 2);
    graph.add_edge(2, 3, 3);
    graph.add_edge(2, 5, 2);
    graph.add_edge(2, 4, 4);
    graph.add_edge(3, 2, 3);
    graph.add_edge(3, 4, 3);
    graph.add_edge(4, 2, 4);
    graph.add_edge(4, 3, 3);
    graph.add_edge(5, 2, 2);
    graph.add_edge(5, 4, 3);
    
    cout << "测试图:" << endl;
    graph.print();
    
    // Kruskal算法
    cout << "\n1. Kruskal算法:" << endl;
    KruskalAlgorithm<int> kruskal(graph);
    auto kruskal_result = kruskal.find_mst();
    kruskal_result.print();
    
    // Prim算法
    cout << "\n2. Prim算法:" << endl;
    PrimAlgorithm<int> prim(graph);
    auto prim_result = prim.find_mst();
    prim_result.print();
    
    // Borůvka算法
    cout << "\n3. Borůvka算法:" << endl;
    BoruvkaAlgorithm<int> boruvka(graph);
    auto boruvka_result = boruvka.find_mst();
    boruvka_result.print();
    
    // 算法比较
    cout << "\n4. 算法比较:" << endl;
    print_mst_comparison(graph);
    
    // 自动选择算法
    cout << "\n5. 自动选择算法:" << endl;
    auto auto_result = find_mst_auto(graph);
    cout << "自动选择结果: 总权重 = " << auto_result.total_weight << endl;
    
    // 验证MST正确性
    cout << "\n6. MST验证:" << endl;
    bool is_valid = verify_mst(graph, kruskal_result);
    cout << "Kruskal MST验证: " << (is_valid ? "正确" : "错误") << endl;
}

// 测试拓扑排序算法
void test_topological_sort() {
    cout << "\n=== 拓扑排序算法测试 ===" << endl;
    
    // 创建DAG
    AdjacencyListGraph<int> dag(6, true);
    dag.add_edge(5, 2, 1);
    dag.add_edge(5, 0, 1);
    dag.add_edge(4, 0, 1);
    dag.add_edge(4, 1, 1);
    dag.add_edge(2, 3, 1);
    dag.add_edge(3, 1, 1);
    
    cout << "测试DAG:" << endl;
    dag.print();
    
    // Kahn算法
    cout << "\n1. Kahn算法:" << endl;
    KahnTopologicalSort<int> kahn(dag);
    auto kahn_result = kahn.sort();
    kahn_result.print();
    
    cout << "是否为DAG: " << (kahn.is_dag() ? "是" : "否") << endl;
    
    // DFS算法
    cout << "\n2. DFS算法:" << endl;
    DFSTopologicalSort<int> dfs_topo(dag);
    auto dfs_result = dfs_topo.sort();
    dfs_result.print();
    
    cout << "是否为DAG: " << (dfs_topo.is_dag() ? "是" : "否") << endl;
    
    // 算法比较
    cout << "\n3. 算法比较:" << endl;
    compare_topological_algorithms(dag);
    
    // 验证拓扑排序
    cout << "\n4. 拓扑排序验证:" << endl;
    if (!kahn_result.has_cycle) {
        bool is_valid = verify_topological_sort(dag, kahn_result.order);
        cout << "Kahn排序验证: " << (is_valid ? "正确" : "错误") << endl;
    }
    
    // 拓扑排序应用
    cout << "\n5. 拓扑排序应用:" << endl;
    TopologicalApplications<int> apps(dag);
    
    // 最长路径
    auto longest_paths = apps.longest_path(5);
    cout << "从顶点5的最长路径:" << endl;
    for (size_t i = 0; i < longest_paths.size(); ++i) {
        if (longest_paths[i] != numeric_limits<int>::lowest()) {
            cout << "到顶点 " << i << ": " << longest_paths[i] << endl;
        }
    }
    
    // 关键路径方法
    auto cpm_result = apps.critical_path_method();
    cpm_result.print();
    
    // 测试有环图
    cout << "\n6. 有环图测试:" << endl;
    AdjacencyListGraph<int> cyclic_graph(3, true);
    cyclic_graph.add_edge(0, 1, 1);
    cyclic_graph.add_edge(1, 2, 1);
    cyclic_graph.add_edge(2, 0, 1);
    
    KahnTopologicalSort<int> kahn_cyclic(cyclic_graph);
    auto cyclic_result = kahn_cyclic.sort();
    cyclic_result.print();
}

// 测试强连通分量算法
void test_strongly_connected_components() {
    cout << "\n=== 强连通分量算法测试 ===" << endl;
    
    // 创建测试图
    AdjacencyListGraph<int> graph(8, true);
    graph.add_edge(0, 1, 1);
    graph.add_edge(1, 2, 1);
    graph.add_edge(2, 0, 1);
    graph.add_edge(2, 3, 1);
    graph.add_edge(3, 4, 1);
    graph.add_edge(4, 5, 1);
    graph.add_edge(4, 7, 1);
    graph.add_edge(5, 6, 1);
    graph.add_edge(6, 4, 1);
    graph.add_edge(6, 7, 1);
    
    cout << "测试图:" << endl;
    graph.print();
    
    // Tarjan算法
    cout << "\n1. Tarjan算法:" << endl;
    TarjanSCC<int> tarjan(graph);
    auto tarjan_result = tarjan.find_scc();
    tarjan_result.print();
    
    auto tarjan_stats = tarjan_result.get_statistics();
    tarjan_stats.print();
    
    cout << "\n是否强连通: " << (tarjan.is_strongly_connected() ? "是" : "否") << endl;
    
    // Kosaraju算法
    cout << "\n2. Kosaraju算法:" << endl;
    KosarajuSCC<int> kosaraju(graph);
    auto kosaraju_result = kosaraju.find_scc();
    kosaraju_result.print();
    
    cout << "\n是否强连通: " << (kosaraju.is_strongly_connected() ? "是" : "否") << endl;
    
    // 算法比较
    cout << "\n3. 算法比较:" << endl;
    compare_scc_algorithms(graph);
    
    // 验证SCC正确性
    cout << "\n4. SCC验证:" << endl;
    bool is_valid = verify_scc(graph, tarjan_result);
    cout << "Tarjan SCC验证: " << (is_valid ? "正确" : "错误") << endl;
    
    // SCC应用
    cout << "\n5. SCC应用:" << endl;
    SCCApplications<int> scc_apps(graph);
    
    // 构建凝聚图
    auto condensation = scc_apps.build_condensation_graph();
    cout << "凝聚图:" << endl;
    condensation.print();
    
    // 可达性计算
    auto reachability = scc_apps.compute_reachability();
    cout << "\n可达性矩阵:" << endl;
    for (size_t i = 0; i < reachability.size(); ++i) {
        for (size_t j = 0; j < reachability[i].size(); ++j) {
            cout << (reachability[i][j] ? "1" : "0") << " ";
        }
        cout << endl;
    }
    
    // 查找桥和割点
    cout << "\n6. 桥和割点:" << endl;
    auto bridges = tarjan.find_bridges();
    cout << "桥: ";
    for (const auto& bridge : bridges) {
        cout << "(" << bridge.first << "," << bridge.second << ") ";
    }
    cout << endl;
    
    auto articulation_points = tarjan.find_articulation_points();
    cout << "割点: ";
    for (int point : articulation_points) {
        cout << point << " ";
    }
    cout << endl;
}

// 测试网络流算法
void test_network_flow() {
    cout << "\n=== 网络流算法测试 ===" << endl;
    
    // 创建流网络
    FlowNetwork<int> network(6);
    network.add_edge(0, 1, 16);
    network.add_edge(0, 2, 13);
    network.add_edge(1, 2, 10);
    network.add_edge(1, 3, 12);
    network.add_edge(2, 1, 4);
    network.add_edge(2, 4, 14);
    network.add_edge(3, 2, 9);
    network.add_edge(3, 5, 20);
    network.add_edge(4, 3, 7);
    network.add_edge(4, 5, 4);
    
    cout << "流网络:" << endl;
    network.print();
    
    int source = 0, sink = 5;
    
    // Ford-Fulkerson算法
    cout << "\n1. Ford-Fulkerson算法:" << endl;
    FordFulkersonMaxFlow<int> ford_fulkerson(network);
    auto ff_result = ford_fulkerson.max_flow(source, sink);
    ff_result.print();
    
    // Edmonds-Karp算法
    cout << "\n2. Edmonds-Karp算法:" << endl;
    EdmondsKarpMaxFlow<int> edmonds_karp(network);
    auto ek_result = edmonds_karp.max_flow(source, sink);
    ek_result.print();
    
    // Dinic算法
    cout << "\n3. Dinic算法:" << endl;
    DinicMaxFlow<int> dinic(network);
    auto dinic_result = dinic.max_flow(source, sink);
    dinic_result.print();
    
    // 算法比较
    cout << "\n4. 算法比较:" << endl;
    compare_max_flow_algorithms(network, source, sink);
    
    // 最小费用最大流
    cout << "\n5. 最小费用最大流:" << endl;
    MinCostMaxFlow<int, int> min_cost_flow(4);
    min_cost_flow.add_edge(0, 1, 2, 1);
    min_cost_flow.add_edge(0, 2, 2, 2);
    min_cost_flow.add_edge(1, 3, 2, 2);
    min_cost_flow.add_edge(2, 3, 2, 1);
    
    auto mcmf_result = min_cost_flow.min_cost_max_flow(0, 3);
    mcmf_result.print();
}

// 性能测试
void performance_test() {
    cout << "\n=== 性能测试 ===" << endl;
    
    const int vertices = 1000;
    const int edges = 5000;
    
    // 生成随机图
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> vertex_dist(0, vertices - 1);
    uniform_int_distribution<> weight_dist(1, 100);
    
    AdjacencyListGraph<int> large_graph(vertices, false);
    
    for (int i = 0; i < edges; ++i) {
        int u = vertex_dist(gen);
        int v = vertex_dist(gen);
        if (u != v) {
            large_graph.add_edge(u, v, weight_dist(gen));
        }
    }
    
    cout << "测试图: " << vertices << " 个顶点, " << edges << " 条边" << endl;
    
    // 测试DFS性能
    auto start = high_resolution_clock::now();
    DepthFirstSearch<int> dfs(large_graph);
    auto dfs_result = dfs.traverse(0);
    auto end = high_resolution_clock::now();
    auto dfs_time = duration_cast<milliseconds>(end - start);
    cout << "DFS遍历时间: " << dfs_time.count() << " ms" << endl;
    
    // 测试BFS性能
    start = high_resolution_clock::now();
    BreadthFirstSearch<int> bfs(large_graph);
    auto bfs_result = bfs.traverse(0);
    end = high_resolution_clock::now();
    auto bfs_time = duration_cast<milliseconds>(end - start);
    cout << "BFS遍历时间: " << bfs_time.count() << " ms" << endl;
    
    // 测试Dijkstra性能
    start = high_resolution_clock::now();
    DijkstraAlgorithm<int> dijkstra(large_graph);
    auto dijkstra_result = dijkstra.shortest_path(0);
    end = high_resolution_clock::now();
    auto dijkstra_time = duration_cast<milliseconds>(end - start);
    cout << "Dijkstra算法时间: " << dijkstra_time.count() << " ms" << endl;
    
    // 测试MST性能
    start = high_resolution_clock::now();
    KruskalAlgorithm<int> kruskal(large_graph);
    auto mst_result = kruskal.find_mst();
    end = high_resolution_clock::now();
    auto mst_time = duration_cast<milliseconds>(end - start);
    cout << "Kruskal算法时间: " << mst_time.count() << " ms" << endl;
}

// 综合测试
void comprehensive_test() {
    cout << "\n=== 综合测试 ===" << endl;
    
    // 创建复杂图进行综合测试
    AdjacencyListGraph<int> graph(10, true);
    
    // 添加边构建复杂图
    vector<tuple<int, int, int>> edges = {
        {0, 1, 5}, {0, 2, 3}, {1, 3, 6}, {1, 4, 2},
        {2, 4, 7}, {3, 5, 1}, {4, 5, 4}, {4, 6, 8},
        {5, 7, 2}, {6, 7, 3}, {6, 8, 5}, {7, 9, 1},
        {8, 9, 2}, {9, 0, 4}
    };
    
    for (const auto& edge : edges) {
        graph.add_edge(get<0>(edge), get<1>(edge), get<2>(edge));
    }
    
    cout << "综合测试图:" << endl;
    graph.print();
    
    // 图的基本属性
    cout << "\n图的基本属性:" << endl;
    cout << "顶点数: " << graph.get_vertex_count() << endl;
    cout << "边数: " << graph.get_edge_count() << endl;
    cout << "是否连通: " << (graph.is_connected() ? "是" : "否") << endl;
    cout << "是否有环: " << (graph.has_cycle_directed() ? "是" : "否") << endl;
    
    // 遍历测试
    cout << "\n遍历测试:" << endl;
    DepthFirstSearch<int> dfs(graph);
    auto dfs_traversal = dfs.traverse(0);
    cout << "DFS: ";
    print_traversal_result(dfs_traversal);
    
    BreadthFirstSearch<int> bfs(graph);
    auto bfs_traversal = bfs.traverse(0);
    cout << "BFS: ";
    print_traversal_result(bfs_traversal);
    
    // 最短路径测试
    cout << "\n最短路径测试:" << endl;
    DijkstraAlgorithm<int> dijkstra(graph);
    auto sp_result = dijkstra.shortest_path(0, 9);
    cout << "最短路径 (0 -> 9): ";
    print_shortest_path_result(sp_result);
    
    // 强连通分量测试
    cout << "\n强连通分量测试:" << endl;
    TarjanSCC<int> tarjan(graph);
    auto scc_result = tarjan.find_scc();
    cout << "强连通分量数: " << scc_result.num_components << endl;
    
    // 拓扑排序测试（如果是DAG）
    cout << "\n拓扑排序测试:" << endl;
    KahnTopologicalSort<int> kahn(graph);
    auto topo_result = kahn.sort();
    if (!topo_result.has_cycle) {
        cout << "拓扑排序: ";
        for (size_t i = 0; i < topo_result.order.size(); ++i) {
            cout << topo_result.order[i];
            if (i < topo_result.order.size() - 1) cout << " -> ";
        }
        cout << endl;
    } else {
        cout << "图有环，无法进行拓扑排序" << endl;
    }
}

int main() {
    cout << "图论算法演示程序" << endl;
    cout << "==================" << endl;
    
    try {
        // 基础测试
        test_graph_representation();
        test_graph_traversal();
        test_shortest_path();
        test_minimum_spanning_tree();
        test_topological_sort();
        test_strongly_connected_components();
        test_network_flow();
        
        // 性能测试
        performance_test();
        
        // 综合测试
        comprehensive_test();
        
        cout << "\n所有测试完成！" << endl;
        
    } catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}