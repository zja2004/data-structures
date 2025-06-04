# 树形数据结构 (Tree Data Structures)

树是一种重要的非线性数据结构，在计算机科学中有着广泛的应用。本章将详细介绍各种树形数据结构的实现和应用。

## 📚 目录结构

```
02_tree/
├── README.md                    # 本文件
├── binary_tree/                 # 二叉树
│   ├── README.md
│   ├── binary_tree.h
│   ├── binary_search_tree.h
│   ├── avl_tree.h
│   ├── red_black_tree.h
│   └── example.cpp
├── heap/                        # 堆
│   ├── README.md
│   ├── binary_heap.h
│   ├── leftist_heap.h
│   ├── binomial_heap.h
│   └── example.cpp
├── trie/                        # 字典树
│   ├── README.md
│   ├── trie.h
│   ├── compressed_trie.h
│   └── example.cpp
├── segment_tree/                # 线段树
│   ├── README.md
│   ├── segment_tree.h
│   ├── lazy_segment_tree.h
│   └── example.cpp
├── fenwick_tree/                # 树状数组
│   ├── README.md
│   ├── fenwick_tree.h
│   └── example.cpp
└── b_tree/                      # B树
    ├── README.md
    ├── b_tree.h
    ├── b_plus_tree.h
    └── example.cpp
```

## 🌳 树的基本概念

### 基本术语

- **节点 (Node)**: 树的基本单位，包含数据和指向子节点的指针
- **根节点 (Root)**: 树的顶层节点，没有父节点
- **叶子节点 (Leaf)**: 没有子节点的节点
- **内部节点 (Internal Node)**: 有子节点的节点
- **父节点 (Parent)**: 某节点的上一层节点
- **子节点 (Child)**: 某节点的下一层节点
- **兄弟节点 (Sibling)**: 具有相同父节点的节点
- **祖先 (Ancestor)**: 从根到某节点路径上的所有节点
- **后代 (Descendant)**: 某节点子树中的所有节点

### 树的性质

- **深度 (Depth)**: 从根节点到某节点的路径长度
- **高度 (Height)**: 从某节点到叶子节点的最长路径长度
- **层数 (Level)**: 节点的深度 + 1
- **度 (Degree)**: 节点的子节点个数
- **树的度**: 树中所有节点度的最大值

### 树的分类

1. **按子节点数量**:
   - 二叉树：每个节点最多有两个子节点
   - 多叉树：每个节点可以有多个子节点

2. **按结构特性**:
   - 满二叉树：所有叶子节点都在同一层，且每个内部节点都有两个子节点
   - 完全二叉树：除最后一层外，其他层都被完全填满，最后一层从左到右填充
   - 平衡二叉树：任意节点的左右子树高度差不超过1

3. **按应用场景**:
   - 搜索树：支持高效查找操作
   - 堆：支持优先队列操作
   - 字典树：用于字符串匹配
   - 线段树：用于区间查询

## 🔍 各种树结构对比

| 数据结构 | 查找 | 插入 | 删除 | 空间复杂度 | 主要应用 |
|---------|------|------|------|-----------|----------|
| 二叉搜索树 | O(h) | O(h) | O(h) | O(n) | 动态查找 |
| AVL树 | O(log n) | O(log n) | O(log n) | O(n) | 平衡查找 |
| 红黑树 | O(log n) | O(log n) | O(log n) | O(n) | STL实现 |
| 二叉堆 | O(n) | O(log n) | O(log n) | O(n) | 优先队列 |
| 字典树 | O(m) | O(m) | O(m) | O(ALPHABET_SIZE * N * M) | 字符串匹配 |
| 线段树 | O(log n) | O(log n) | O(log n) | O(n) | 区间查询 |
| 树状数组 | O(log n) | O(log n) | O(log n) | O(n) | 前缀和查询 |
| B树 | O(log n) | O(log n) | O(log n) | O(n) | 数据库索引 |

*注：h为树的高度，n为节点数，m为字符串长度*

## 🚀 树的遍历方法

### 深度优先遍历 (DFS)

1. **前序遍历 (Preorder)**: 根 → 左 → 右
   - 应用：复制树、表达式求值

2. **中序遍历 (Inorder)**: 左 → 根 → 右
   - 应用：二叉搜索树的有序输出

3. **后序遍历 (Postorder)**: 左 → 右 → 根
   - 应用：删除树、计算目录大小

### 广度优先遍历 (BFS)

1. **层序遍历 (Level Order)**: 逐层从左到右
   - 应用：打印树的层次结构、求树的宽度

## 📈 应用场景

### 1. 文件系统
```
根目录/
├── home/
│   ├── user1/
│   └── user2/
├── etc/
└── var/
    └── log/
```

### 2. 组织结构
```
CEO
├── CTO
│   ├── 开发部
│   └── 测试部
└── CFO
    └── 财务部
```

### 3. 表达式树
```
    +
   / \
  *   3
 / \
a   b
```
表示：(a * b) + 3

### 4. 决策树
```
年龄 < 30?
├── 是 → 收入 > 5万?
│   ├── 是 → 批准贷款
│   └── 否 → 拒绝贷款
└── 否 → 批准贷款
```

## 🛠️ 实现要点

### 1. 节点设计
```cpp
template<typename T>
struct TreeNode {
    T data;
    TreeNode* left;
    TreeNode* right;
    TreeNode* parent;  // 可选
    
    TreeNode(const T& value) 
        : data(value), left(nullptr), right(nullptr), parent(nullptr) {}
};
```

### 2. 内存管理
- 使用智能指针避免内存泄漏
- 实现正确的拷贝构造和赋值操作
- 递归删除时注意栈溢出

### 3. 平衡维护
- AVL树：通过旋转维护高度平衡
- 红黑树：通过颜色和旋转维护近似平衡
- B树：通过分裂和合并维护平衡

### 4. 迭代器设计
- 中序迭代器：实现有序遍历
- 层序迭代器：实现广度优先遍历
- 考虑const和非const版本

## 🎯 学习建议

### 初学者路径
1. **二叉树基础** → 理解树的基本概念和遍历
2. **二叉搜索树** → 掌握查找树的基本操作
3. **堆** → 学习优先队列的实现
4. **平衡树** → 理解自平衡的重要性

### 进阶路径
1. **字典树** → 掌握字符串处理技巧
2. **线段树** → 学习区间查询优化
3. **B树** → 了解数据库索引原理
4. **高级变种** → 研究特殊应用场景

### 实践建议
1. **手动模拟**: 在纸上画出树的操作过程
2. **可视化工具**: 使用在线工具观察树的变化
3. **性能测试**: 比较不同树结构的性能差异
4. **实际应用**: 尝试解决具体的算法问题

## 🔧 调试技巧

### 1. 可视化输出
```cpp
void print_tree(TreeNode* root, int depth = 0) {
    if (!root) return;
    
    print_tree(root->right, depth + 1);
    
    for (int i = 0; i < depth; ++i) {
        std::cout << "    ";
    }
    std::cout << root->data << std::endl;
    
    print_tree(root->left, depth + 1);
}
```

### 2. 结构验证
```cpp
bool is_valid_bst(TreeNode* root, int min_val, int max_val) {
    if (!root) return true;
    
    if (root->data <= min_val || root->data >= max_val) {
        return false;
    }
    
    return is_valid_bst(root->left, min_val, root->data) &&
           is_valid_bst(root->right, root->data, max_val);
}
```

### 3. 内存检查
- 使用valgrind检测内存泄漏
- 确保每个new都有对应的delete
- 注意递归深度避免栈溢出

## 📚 参考资料

- 《算法导论》- 树结构的经典教材
- 《数据结构与算法分析》- 详细的实现分析
- 《STL源码剖析》- 了解工业级实现
- LeetCode树相关题目 - 实践练习

---

**下一步**: 选择感兴趣的树结构开始学习，建议从[二叉树](binary_tree/README.md)开始！