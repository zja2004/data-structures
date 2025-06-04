# 图 (Graph)

## 概述

图是一种非线性数据结构，由顶点（Vertex）和边（Edge）组成。图可以用来表示各种实际问题中的关系，如社交网络、交通网络、计算机网络等。图论是计算机科学中的重要分支，在算法设计和问题求解中有着广泛的应用。

## 核心特性

### 基本概念

- **顶点（Vertex/Node）**：图中的基本单元，表示实体
- **边（Edge）**：连接两个顶点的线，表示关系
- **度（Degree）**：与顶点相连的边的数量
- **路径（Path）**：从一个顶点到另一个顶点经过的顶点序列
- **环（Cycle）**：起点和终点相同的路径

### 图的分类

1. **有向图（Directed Graph）**：边有方向性
2. **无向图（Undirected Graph）**：边无方向性
3. **加权图（Weighted Graph）**：边带有权值
4. **简单图（Simple Graph）**：不含自环和重边
5. **完全图（Complete Graph）**：任意两个顶点之间都有边
6. **连通图（Connected Graph）**：任意两个顶点之间都有路径

## 图的表示方法

### 1. 邻接矩阵（Adjacency Matrix）

```cpp
// 使用二维数组表示
vector<vector<int>> adj_matrix(n, vector<int>(n, 0));
// adj_matrix[i][j] = 1 表示顶点i和j之间有边
```

**优点**：
- 查询两个顶点是否相邻：O(1)
- 适合稠密图
- 实现简单

**缺点**：
- 空间复杂度：O(V²)
- 不适合稀疏图

### 2. 邻接表（Adjacency List）

```cpp
// 使用向量数组表示
vector<vector<int>> adj_list(n);
// adj_list[i] 存储与顶点i相邻的所有顶点
```

**优点**：
- 空间复杂度：O(V + E)
- 适合稀疏图
- 遍历效率高

**缺点**：
- 查询两个顶点是否相邻：O(degree)

### 3. 边列表（Edge List）

```cpp
// 使用边的集合表示
struct Edge {
    int from, to, weight;
};
vector<Edge> edges;
```

## 图的遍历算法

### 1. 深度优先搜索（DFS）

- **时间复杂度**：O(V + E)
- **空间复杂度**：O(V)
- **应用**：连通性检测、拓扑排序、强连通分量

### 2. 广度优先搜索（BFS）

- **时间复杂度**：O(V + E)
- **空间复杂度**：O(V)
- **应用**：最短路径、层次遍历

## 最短路径算法

### 1. Dijkstra算法

- **适用**：非负权重的单源最短路径
- **时间复杂度**：O((V + E) log V)
- **空间复杂度**：O(V)

### 2. Bellman-Ford算法

- **适用**：可处理负权重的单源最短路径
- **时间复杂度**：O(VE)
- **空间复杂度**：O(V)

### 3. Floyd-Warshall算法

- **适用**：所有顶点对之间的最短路径
- **时间复杂度**：O(V³)
- **空间复杂度**：O(V²)

### 4. A*算法

- **适用**：启发式搜索的最短路径
- **特点**：使用启发函数提高搜索效率

## 最小生成树算法

### 1. Kruskal算法

- **策略**：按边权重排序，使用并查集避免环
- **时间复杂度**：O(E log E)
- **空间复杂度**：O(V)

### 2. Prim算法

- **策略**：从任意顶点开始，逐步扩展最小生成树
- **时间复杂度**：O((V + E) log V)
- **空间复杂度**：O(V)

## 拓扑排序

### Kahn算法

- **适用**：有向无环图（DAG）
- **时间复杂度**：O(V + E)
- **应用**：任务调度、依赖关系处理

### DFS拓扑排序

- **策略**：基于DFS的后序遍历
- **时间复杂度**：O(V + E)

## 强连通分量

### Tarjan算法

- **时间复杂度**：O(V + E)
- **特点**：一次DFS完成
- **应用**：图的强连通性分析

### Kosaraju算法

- **时间复杂度**：O(V + E)
- **特点**：两次DFS
- **实现简单**

## 网络流算法

### 1. Ford-Fulkerson算法

- **应用**：最大流问题
- **时间复杂度**：O(E × max_flow)

### 2. Edmonds-Karp算法

- **改进**：使用BFS寻找增广路径
- **时间复杂度**：O(VE²)

### 3. Dinic算法

- **特点**：分层图 + 多路增广
- **时间复杂度**：O(V²E)

## 应用场景

### 1. 社交网络分析

- 好友关系图
- 影响力传播
- 社区发现

### 2. 交通网络

- 路径规划
- 交通流量优化
- 最短路径导航

### 3. 计算机网络

- 网络拓扑
- 路由算法
- 网络流量分析

### 4. 任务调度

- 依赖关系处理
- 关键路径分析
- 资源分配

## 经典算法问题

### 基础问题

1. **图的遍历**：DFS和BFS的实现
2. **连通性检测**：判断图是否连通
3. **环检测**：检测图中是否存在环
4. **二分图检测**：判断图是否为二分图

### 进阶问题

1. **最短路径**：单源和多源最短路径
2. **最小生成树**：连接所有顶点的最小权重树
3. **拓扑排序**：有向无环图的线性排序
4. **强连通分量**：有向图的强连通性分析

### 高级问题

1. **网络流**：最大流、最小割问题
2. **匹配问题**：二分图最大匹配
3. **着色问题**：图的顶点着色
4. **哈密顿路径**：访问每个顶点恰好一次的路径

## 时间复杂度对比

| 算法 | 时间复杂度 | 空间复杂度 | 适用场景 |
|------|------------|------------|----------|
| DFS | O(V + E) | O(V) | 连通性、拓扑排序 |
| BFS | O(V + E) | O(V) | 最短路径、层次遍历 |
| Dijkstra | O((V + E) log V) | O(V) | 非负权最短路径 |
| Bellman-Ford | O(VE) | O(V) | 负权最短路径 |
| Floyd-Warshall | O(V³) | O(V²) | 所有对最短路径 |
| Kruskal | O(E log E) | O(V) | 最小生成树 |
| Prim | O((V + E) log V) | O(V) | 最小生成树 |
| Tarjan | O(V + E) | O(V) | 强连通分量 |

## 实现文件结构

```
graph/
├── README.md                    # 本文档
├── graph_representation.h       # 图的表示方法
├── graph_traversal.h            # 图的遍历算法
├── shortest_path.h              # 最短路径算法
├── minimum_spanning_tree.h      # 最小生成树算法
├── topological_sort.h           # 拓扑排序算法
├── strongly_connected.h         # 强连通分量算法
├── network_flow.h               # 网络流算法
├── graph_algorithms.h           # 其他图算法
└── example.cpp                  # 使用示例和测试
```

## 使用示例

### 基本图操作

```cpp
#include "graph_representation.h"

// 创建邻接表图
AdjacencyListGraph<int> graph(5);
graph.add_edge(0, 1);
graph.add_edge(1, 2);
graph.add_edge(2, 3);
graph.add_edge(3, 4);

// 遍历图
graph.dfs(0, [](int vertex) {
    cout << vertex << " ";
});
```

### 最短路径

```cpp
#include "shortest_path.h"

// Dijkstra算法
WeightedGraph graph(5);
graph.add_edge(0, 1, 10);
graph.add_edge(1, 2, 20);
graph.add_edge(0, 2, 5);

auto distances = dijkstra(graph, 0);
for (int i = 0; i < distances.size(); ++i) {
    cout << "Distance to " << i << ": " << distances[i] << "\n";
}
```

### 最小生成树

```cpp
#include "minimum_spanning_tree.h"

// Kruskal算法
WeightedGraph graph(4);
graph.add_edge(0, 1, 10);
graph.add_edge(1, 2, 15);
graph.add_edge(2, 3, 4);
graph.add_edge(3, 0, 6);

auto mst = kruskal(graph);
int total_weight = 0;
for (const auto& edge : mst) {
    cout << edge.from << " - " << edge.to << ": " << edge.weight << "\n";
    total_weight += edge.weight;
}
cout << "Total MST weight: " << total_weight << "\n";
```

## 编译和运行

```bash
# 编译示例程序
g++ -std=c++17 -O2 example.cpp -o graph_example

# 运行示例
./graph_example

# 编译时启用调试信息
g++ -std=c++17 -g -DDEBUG example.cpp -o graph_debug
```

## 练习题

### 基础练习

1. 实现图的邻接表和邻接矩阵表示
2. 实现DFS和BFS遍历算法
3. 检测无向图中是否存在环
4. 判断图是否为二分图

### 进阶练习

1. 实现Dijkstra最短路径算法
2. 实现Kruskal最小生成树算法
3. 实现拓扑排序算法
4. 检测有向图中的强连通分量

### 挑战练习

1. 实现网络最大流算法
2. 解决旅行商问题（TSP）
3. 实现图的最小顶点覆盖
4. 解决图着色问题

## 调试技巧

### 1. 图的可视化

```cpp
void print_graph(const AdjacencyListGraph& graph) {
    for (int i = 0; i < graph.vertex_count(); ++i) {
        cout << "Vertex " << i << ": ";
        for (int neighbor : graph.get_neighbors(i)) {
            cout << neighbor << " ";
        }
        cout << "\n";
    }
}
```

### 2. 路径追踪

```cpp
vector<int> reconstruct_path(const vector<int>& parent, int start, int end) {
    vector<int> path;
    for (int v = end; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    reverse(path.begin(), path.end());
    return path;
}
```

### 3. 性能分析

- 使用计时器测量算法执行时间
- 分析不同图规模下的性能表现
- 比较不同算法的效率

## 学习建议

### 学习路径

1. **基础概念**：理解图的基本概念和表示方法
2. **遍历算法**：掌握DFS和BFS的实现和应用
3. **最短路径**：学习各种最短路径算法
4. **生成树**：理解最小生成树的概念和算法
5. **高级算法**：学习网络流、匹配等高级算法

### 实践建议

1. 从简单的图操作开始实现
2. 通过可视化理解算法过程
3. 解决LeetCode上的图相关题目
4. 分析实际应用中的图问题

### 扩展阅读

- 《算法导论》图算法章节
- 《图论及其应用》
- 相关论文：图算法的最新研究进展

## 参考资料

1. Cormen, T. H. et al. "Introduction to Algorithms" - 图算法
2. Sedgewick, R. "Algorithms" - 图和网络
3. West, D. B. "Introduction to Graph Theory"
4. 在线资源：图算法可视化工具