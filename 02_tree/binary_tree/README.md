# 二叉树 (Binary Tree)

二叉树是每个节点最多有两个子节点的树结构，是最重要和最常用的树形数据结构之一。

## 📚 目录

- [基本概念](#基本概念)
- [二叉树类型](#二叉树类型)
- [基本操作](#基本操作)
- [遍历方法](#遍历方法)
- [实现文件](#实现文件)
- [时间复杂度](#时间复杂度)
- [应用场景](#应用场景)
- [练习题目](#练习题目)

## 🌳 基本概念

### 定义
二叉树是一种树形数据结构，其中每个节点最多有两个子节点，通常称为左子节点和右子节点。

### 基本术语
- **根节点**: 树的顶层节点
- **叶子节点**: 没有子节点的节点
- **内部节点**: 至少有一个子节点的节点
- **左子树**: 节点的左子节点及其所有后代
- **右子树**: 节点的右子节点及其所有后代
- **深度**: 从根节点到某节点的边数
- **高度**: 从某节点到叶子节点的最长路径的边数

### 性质
1. 第i层最多有 2^(i-1) 个节点 (i ≥ 1)
2. 深度为k的二叉树最多有 2^k - 1 个节点
3. 对于任何二叉树，叶子节点数 = 度为2的节点数 + 1
4. 具有n个节点的完全二叉树的深度为 ⌊log₂n⌋ + 1

## 🔍 二叉树类型

### 1. 满二叉树 (Full Binary Tree)
- 每个内部节点都恰好有两个子节点
- 所有叶子节点都在同一层

```
      1
    /   \
   2     3
  / \   / \
 4   5 6   7
```

### 2. 完全二叉树 (Complete Binary Tree)
- 除最后一层外，其他层都被完全填满
- 最后一层从左到右填充

```
      1
    /   \
   2     3
  / \   /
 4   5 6
```

### 3. 平衡二叉树 (Balanced Binary Tree)
- 任意节点的左右子树高度差不超过1

### 4. 二叉搜索树 (Binary Search Tree)
- 左子树所有节点值 < 根节点值 < 右子树所有节点值
- 中序遍历得到有序序列

```
      8
    /   \
   3     10
  / \      \
 1   6     14
    / \   /
   4   7 13
```

### 5. 堆 (Heap)
- **最大堆**: 父节点值 ≥ 子节点值
- **最小堆**: 父节点值 ≤ 子节点值

## ⚙️ 基本操作

### 1. 创建和销毁
```cpp
// 创建节点
TreeNode* create_node(int value);

// 销毁树
void destroy_tree(TreeNode* root);
```

### 2. 插入和删除
```cpp
// 插入节点（BST）
TreeNode* insert(TreeNode* root, int value);

// 删除节点（BST）
TreeNode* remove(TreeNode* root, int value);
```

### 3. 查找
```cpp
// 查找节点
TreeNode* search(TreeNode* root, int value);

// 查找最小值
TreeNode* find_min(TreeNode* root);

// 查找最大值
TreeNode* find_max(TreeNode* root);
```

### 4. 属性查询
```cpp
// 计算高度
int height(TreeNode* root);

// 计算节点数
int count_nodes(TreeNode* root);

// 检查是否为平衡树
bool is_balanced(TreeNode* root);
```

## 🔄 遍历方法

### 1. 深度优先遍历 (DFS)

#### 前序遍历 (Preorder): 根 → 左 → 右
```cpp
void preorder(TreeNode* root) {
    if (root) {
        visit(root);
        preorder(root->left);
        preorder(root->right);
    }
}
```

#### 中序遍历 (Inorder): 左 → 根 → 右
```cpp
void inorder(TreeNode* root) {
    if (root) {
        inorder(root->left);
        visit(root);
        inorder(root->right);
    }
}
```

#### 后序遍历 (Postorder): 左 → 右 → 根
```cpp
void postorder(TreeNode* root) {
    if (root) {
        postorder(root->left);
        postorder(root->right);
        visit(root);
    }
}
```

### 2. 广度优先遍历 (BFS)

#### 层序遍历 (Level Order)
```cpp
void level_order(TreeNode* root) {
    if (!root) return;
    
    std::queue<TreeNode*> q;
    q.push(root);
    
    while (!q.empty()) {
        TreeNode* node = q.front();
        q.pop();
        
        visit(node);
        
        if (node->left) q.push(node->left);
        if (node->right) q.push(node->right);
    }
}
```

### 3. 非递归遍历

#### 非递归中序遍历
```cpp
void inorder_iterative(TreeNode* root) {
    std::stack<TreeNode*> stack;
    TreeNode* current = root;
    
    while (current || !stack.empty()) {
        while (current) {
            stack.push(current);
            current = current->left;
        }
        
        current = stack.top();
        stack.pop();
        visit(current);
        
        current = current->right;
    }
}
```

## 📁 实现文件

### 文件结构
```
binary_tree/
├── binary_tree.h           # 基本二叉树实现
├── binary_search_tree.h    # 二叉搜索树实现
├── avl_tree.h             # AVL树实现
├── red_black_tree.h       # 红黑树实现
└── example.cpp            # 使用示例
```

### 核心类

#### 1. BinaryTree
- 基本二叉树操作
- 各种遍历方法
- 树的属性计算

#### 2. BinarySearchTree
- 继承自BinaryTree
- 支持高效查找、插入、删除
- 维护BST性质

#### 3. AVLTree
- 自平衡二叉搜索树
- 通过旋转维护平衡
- 保证O(log n)的操作复杂度

#### 4. RedBlackTree
- 近似平衡的二叉搜索树
- 通过颜色和旋转维护平衡
- STL中map/set的底层实现

## ⏱️ 时间复杂度

| 操作 | 普通二叉树 | 二叉搜索树(平均) | 二叉搜索树(最坏) | AVL树 | 红黑树 |
|------|-----------|----------------|----------------|-------|--------|
| 查找 | O(n) | O(log n) | O(n) | O(log n) | O(log n) |
| 插入 | O(1) | O(log n) | O(n) | O(log n) | O(log n) |
| 删除 | O(n) | O(log n) | O(n) | O(log n) | O(log n) |
| 遍历 | O(n) | O(n) | O(n) | O(n) | O(n) |

### 空间复杂度
- **存储空间**: O(n)
- **递归栈空间**: O(h)，其中h为树的高度
  - 平衡树: O(log n)
  - 退化为链表: O(n)

## 🚀 应用场景

### 1. 表达式树
```
表达式: (a + b) * (c - d)

    *
   / \
  +   -
 / \ / \
a  b c  d
```

### 2. 文件系统
```
根目录
├── home
│   ├── user1
│   └── user2
└── etc
    └── config
```

### 3. 决策树
```
年龄 < 30?
├── 是 → 收入检查
└── 否 → 直接批准
```

### 4. 语法分析树
```
if (x > 0) { print(x); }

      if
     /  \
   >     {
  / \     |
 x   0  print
         |
         x
```

### 5. 数据库索引
- B树和B+树用于数据库索引
- 支持高效的范围查询

## 🎯 经典算法问题

### 1. 树的构造
- 从前序和中序遍历构造二叉树
- 从后序和中序遍历构造二叉树
- 从层序遍历构造完全二叉树

### 2. 路径问题
- 二叉树的最大路径和
- 根到叶子的路径和
- 两节点间的最短路径

### 3. 结构判断
- 判断是否为平衡二叉树
- 判断是否为二叉搜索树
- 判断两棵树是否相同

### 4. 查找问题
- 二叉搜索树中的第k小元素
- 最近公共祖先
- 二叉树的直径

## 📝 练习题目

### 基础题目
1. 实现二叉树的三种遍历（递归和非递归）
2. 计算二叉树的高度和节点数
3. 判断二叉树是否为平衡树
4. 查找二叉树中的最大值和最小值

### 进阶题目
1. 二叉搜索树的插入和删除
2. 将有序数组转换为平衡二叉搜索树
3. 验证二叉搜索树的有效性
4. 二叉树的序列化和反序列化

### 高级题目
1. 实现AVL树的插入和删除
2. 实现红黑树的基本操作
3. 二叉树的Morris遍历
4. 线索二叉树的实现

## 🛠️ 调试技巧

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

### 2. 验证树的性质
```cpp
// 验证BST性质
bool is_valid_bst(TreeNode* root, long min_val, long max_val) {
    if (!root) return true;
    
    if (root->data <= min_val || root->data >= max_val) {
        return false;
    }
    
    return is_valid_bst(root->left, min_val, root->data) &&
           is_valid_bst(root->right, root->data, max_val);
}
```

### 3. 内存管理
```cpp
// 确保正确释放内存
void destroy_tree(TreeNode* root) {
    if (root) {
        destroy_tree(root->left);
        destroy_tree(root->right);
        delete root;
    }
}
```

## 📚 学习建议

### 学习顺序
1. **基本概念** → 理解树的结构和术语
2. **遍历方法** → 掌握各种遍历算法
3. **二叉搜索树** → 学习查找树的操作
4. **平衡树** → 理解自平衡的重要性
5. **高级应用** → 解决实际问题

### 实践建议
1. **手动模拟**: 在纸上画出树的操作过程
2. **代码实现**: 从简单的遍历开始实现
3. **测试验证**: 编写测试用例验证正确性
4. **性能分析**: 比较不同实现的性能差异

### 常见错误
1. **内存泄漏**: 忘记释放节点内存
2. **空指针**: 没有检查节点是否为空
3. **递归深度**: 深度过大导致栈溢出
4. **边界条件**: 没有处理空树的情况

---

**下一步**: 查看具体的实现文件，从[基本二叉树](binary_tree.h)开始学习！