#include "binary_tree.h"
#include "binary_search_tree.h"
#include "avl_tree.h"
#include "red_black_tree.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// 性能测试辅助函数
template<typename Func>
double measure_time(Func&& func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    return duration.count() / 1000.0; // 返回毫秒
}

// 生成随机数据
vector<int> generate_random_data(int size, int min_val = 1, int max_val = 10000) {
    vector<int> data;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min_val, max_val);
    
    for (int i = 0; i < size; ++i) {
        data.push_back(dis(gen));
    }
    
    return data;
}

void test_basic_binary_tree() {
    cout << "\n=== 基础二叉树测试 ===\n";
    
    // 从层序数组构建二叉树
    vector<int> level_order = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    BinaryTree<int> tree(level_order);
    
    cout << "从层序数组构建的二叉树:" << endl;
    tree.print_tree();
    
    cout << "\n树的基本信息:" << endl;
    cout << "大小: " << tree.size() << endl;
    cout << "高度: " << tree.height() << endl;
    cout << "是否平衡: " << (tree.is_balanced() ? "是" : "否") << endl;
    cout << "是否完全二叉树: " << (tree.is_complete() ? "是" : "否") << endl;
    cout << "是否满二叉树: " << (tree.is_full() ? "是" : "否") << endl;
    
    cout << "\n遍历结果:" << endl;
    auto preorder = tree.preorder();
    auto inorder = tree.inorder();
    auto postorder = tree.postorder();
    auto levelorder = tree.level_order();
    
    cout << "前序遍历: ";
    for (int val : preorder) cout << val << " ";
    cout << endl;
    
    cout << "中序遍历: ";
    for (int val : inorder) cout << val << " ";
    cout << endl;
    
    cout << "后序遍历: ";
    for (int val : postorder) cout << val << " ";
    cout << endl;
    
    cout << "层序遍历: ";
    for (int val : levelorder) cout << val << " ";
    cout << endl;
    
    // 查找操作
    cout << "\n查找操作:" << endl;
    cout << "查找值5: " << (tree.contains(5) ? "找到" : "未找到") << endl;
    cout << "查找值10: " << (tree.contains(10) ? "找到" : "未找到") << endl;
    
    // 路径操作
    cout << "\n路径操作:" << endl;
    auto paths = tree.all_paths();
    cout << "所有根到叶子的路径:" << endl;
    for (const auto& path : paths) {
        for (int val : path) cout << val << " ";
        cout << endl;
    }
    
    auto path_to_8 = tree.path_to_node(8);
    if (!path_to_8.empty()) {
        cout << "到节点8的路径: ";
        for (int val : path_to_8) cout << val << " ";
        cout << endl;
    }
    
    // 最低公共祖先
    cout << "\n最低公共祖先:" << endl;
    TreeNode<int>* lca = tree.lowest_common_ancestor(4, 5);
    if (lca) {
        cout << "节点4和5的LCA: " << lca->data << endl;
    }
    
    cout << "树的直径: " << tree.diameter() << endl;
}

void test_binary_search_tree() {
    cout << "\n=== 二叉搜索树测试 ===\n";
    
    BinarySearchTree<int> bst;
    
    // 插入操作
    vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    cout << "插入序列: ";
    for (int val : values) {
        cout << val << " ";
        bst.insert(val);
    }
    cout << endl;
    
    bst.print_structure();
    
    // 基本操作
    cout << "\n基本操作:" << endl;
    cout << "查找30: " << (bst.contains(30) ? "找到" : "未找到") << endl;
    cout << "查找90: " << (bst.contains(90) ? "找到" : "未找到") << endl;
    cout << "最小值: " << bst.min_value() << endl;
    cout << "最大值: " << bst.max_value() << endl;
    
    // 有序序列
    cout << "\n有序序列: ";
    auto sorted = bst.get_sorted_sequence();
    for (int val : sorted) cout << val << " ";
    cout << endl;
    
    // 第k小/大元素
    cout << "\n第k小/大元素:" << endl;
    try {
        cout << "第3小的元素: " << bst.kth_smallest(3) << endl;
        cout << "第3大的元素: " << bst.kth_largest(3) << endl;
    } catch (const exception& e) {
        cout << "错误: " << e.what() << endl;
    }
    
    // 范围查询
    cout << "\n范围查询 [25, 65]: ";
    auto range_result = bst.range_query(25, 65);
    for (int val : range_result) cout << val << " ";
    cout << endl;
    
    cout << "范围内节点数 [25, 65]: " << bst.count_nodes_in_range(25, 65) << endl;
    
    // 删除操作
    cout << "\n删除操作:" << endl;
    cout << "删除叶子节点10" << endl;
    bst.remove(10);
    bst.print_sorted();
    
    cout << "删除有一个子节点的节点25" << endl;
    bst.remove(25);
    bst.print_sorted();
    
    cout << "删除有两个子节点的节点30" << endl;
    bst.remove(30);
    bst.print_sorted();
    
    // 验证BST性质
    cout << "\n验证BST性质: " << (bst.is_valid_bst() ? "有效" : "无效") << endl;
    
    // 平衡操作
    cout << "\n平衡前高度: " << bst.height() << endl;
    bst.balance();
    cout << "平衡后高度: " << bst.height() << endl;
    bst.print_structure();
}

void test_avl_tree() {
    cout << "\n=== AVL树测试 ===\n";
    
    AVLTree<int> avl;
    
    // 插入操作（会触发旋转）
    vector<int> values = {10, 20, 30, 40, 50, 25};
    cout << "插入序列: ";
    for (int val : values) {
        cout << val << " ";
        avl.insert(val);
        cout << "(高度: " << avl.height() << ") ";
    }
    cout << endl;
    
    avl.print_tree_info();
    cout << "\nAVL树结构:" << endl;
    avl.print_tree();
    
    // 验证AVL性质
    cout << "\n验证AVL性质: " << (avl.is_valid_avl() ? "有效" : "无效") << endl;
    
    // 遍历
    cout << "\n中序遍历: ";
    auto inorder = avl.inorder();
    for (int val : inorder) cout << val << " ";
    cout << endl;
    
    // 删除操作
    cout << "\n删除操作:" << endl;
    cout << "删除30" << endl;
    avl.remove(30);
    avl.print_tree();
    
    cout << "删除后验证AVL性质: " << (avl.is_valid_avl() ? "有效" : "无效") << endl;
    
    // 范围查询
    cout << "\n范围查询 [15, 45]: ";
    auto range_result = avl.range_query(15, 45);
    for (int val : range_result) cout << val << " ";
    cout << endl;
}

void test_red_black_tree() {
    cout << "\n=== 红黑树测试 ===\n";
    
    RedBlackTree<int> rbt;
    
    // 插入操作
    vector<int> values = {7, 3, 18, 10, 22, 8, 11, 26, 2, 6, 13};
    cout << "插入序列: ";
    for (int val : values) {
        cout << val << " ";
        rbt.insert(val);
    }
    cout << endl;
    
    rbt.print_tree_info();
    cout << "\n红黑树结构:" << endl;
    rbt.print_tree();
    
    // 验证红黑树性质
    cout << "\n验证红黑树性质: " << (rbt.is_valid_rb_tree() ? "有效" : "无效") << endl;
    
    // 遍历
    cout << "\n中序遍历: ";
    auto inorder = rbt.inorder();
    for (int val : inorder) cout << val << " ";
    cout << endl;
    
    // 查找操作
    cout << "\n查找操作:" << endl;
    cout << "最小值: " << rbt.find_min() << endl;
    cout << "最大值: " << rbt.find_max() << endl;
    
    try {
        cout << "10的前驱: " << rbt.predecessor(10) << endl;
        cout << "10的后继: " << rbt.successor(10) << endl;
    } catch (const exception& e) {
        cout << "错误: " << e.what() << endl;
    }
    
    // 删除操作
    cout << "\n删除操作:" << endl;
    cout << "删除3" << endl;
    rbt.remove(3);
    rbt.print_tree();
    
    cout << "删除后验证红黑树性质: " << (rbt.is_valid_rb_tree() ? "有效" : "无效") << endl;
    
    // 范围查询
    cout << "\n范围查询 [5, 15]: ";
    auto range_result = rbt.range_query(5, 15);
    for (int val : range_result) cout << val << " ";
    cout << endl;
}

void test_tree_construction() {
    cout << "\n=== 树构造测试 ===\n";
    
    // 从前序和中序构造二叉树
    vector<int> preorder = {3, 9, 20, 15, 7};
    vector<int> inorder = {9, 3, 15, 20, 7};
    
    cout << "从前序和中序构造二叉树:" << endl;
    cout << "前序: ";
    for (int val : preorder) cout << val << " ";
    cout << endl;
    cout << "中序: ";
    for (int val : inorder) cout << val << " ";
    cout << endl;
    
    auto tree1 = build_tree_from_preorder_inorder(preorder, inorder);
    BinaryTree<int> bt1(tree1);
    bt1.print_tree();
    
    // 从后序和中序构造二叉树
    vector<int> postorder = {9, 15, 7, 20, 3};
    
    cout << "\n从后序和中序构造二叉树:" << endl;
    cout << "后序: ";
    for (int val : postorder) cout << val << " ";
    cout << endl;
    cout << "中序: ";
    for (int val : inorder) cout << val << " ";
    cout << endl;
    
    auto tree2 = build_tree_from_postorder_inorder(postorder, inorder);
    BinaryTree<int> bt2(tree2);
    bt2.print_tree();
}

void test_iterators() {
    cout << "\n=== 迭代器测试 ===\n";
    
    // BST迭代器
    BinarySearchTree<int> bst;
    vector<int> values = {50, 30, 70, 20, 40, 60, 80};
    for (int val : values) {
        bst.insert(val);
    }
    
    cout << "BST迭代器遍历: ";
    BSTIterator<int> bst_iter(bst.get_root());
    while (bst_iter.has_next()) {
        cout << bst_iter.next() << " ";
    }
    cout << endl;
    
    // AVL迭代器
    AVLTree<int> avl;
    for (int val : values) {
        avl.insert(val);
    }
    
    cout << "AVL迭代器遍历: ";
    AVLIterator<int> avl_iter(avl.get_root());
    while (avl_iter.has_next()) {
        cout << avl_iter.next() << " ";
    }
    cout << endl;
    
    // 红黑树迭代器
    RedBlackTree<int> rbt;
    for (int val : values) {
        rbt.insert(val);
    }
    
    cout << "红黑树迭代器遍历: ";
    RBIterator<int> rb_iter(rbt.get_root(), nullptr); // 简化处理
    // 注意：这里需要传入正确的nil节点，实际使用中需要修改
    cout << "(需要正确的nil节点支持)" << endl;
}

void performance_comparison() {
    cout << "\n=== 性能比较测试 ===\n";
    
    const int test_size = 10000;
    auto random_data = generate_random_data(test_size);
    auto sorted_data = random_data;
    sort(sorted_data.begin(), sorted_data.end());
    
    cout << "测试数据大小: " << test_size << endl;
    
    // BST性能测试
    cout << "\n二叉搜索树性能:" << endl;
    BinarySearchTree<int> bst;
    
    double bst_insert_time = measure_time([&]() {
        for (int val : random_data) {
            bst.insert(val);
        }
    });
    cout << "插入时间: " << bst_insert_time << " ms" << endl;
    
    double bst_search_time = measure_time([&]() {
        for (int i = 0; i < 1000; ++i) {
            bst.contains(random_data[i]);
        }
    });
    cout << "查找时间(1000次): " << bst_search_time << " ms" << endl;
    cout << "BST高度: " << bst.height() << endl;
    
    // AVL性能测试
    cout << "\nAVL树性能:" << endl;
    AVLTree<int> avl;
    
    double avl_insert_time = measure_time([&]() {
        for (int val : random_data) {
            avl.insert(val);
        }
    });
    cout << "插入时间: " << avl_insert_time << " ms" << endl;
    
    double avl_search_time = measure_time([&]() {
        for (int i = 0; i < 1000; ++i) {
            avl.contains(random_data[i]);
        }
    });
    cout << "查找时间(1000次): " << avl_search_time << " ms" << endl;
    cout << "AVL高度: " << avl.height() << endl;
    
    // 红黑树性能测试
    cout << "\n红黑树性能:" << endl;
    RedBlackTree<int> rbt;
    
    double rbt_insert_time = measure_time([&]() {
        for (int val : random_data) {
            rbt.insert(val);
        }
    });
    cout << "插入时间: " << rbt_insert_time << " ms" << endl;
    
    double rbt_search_time = measure_time([&]() {
        for (int i = 0; i < 1000; ++i) {
            rbt.contains(random_data[i]);
        }
    });
    cout << "查找时间(1000次): " << rbt_search_time << " ms" << endl;
    cout << "红黑树高度: " << rbt.height() << endl;
    
    // 性能总结
    cout << "\n性能总结:" << endl;
    cout << "插入性能排序: ";
    vector<pair<string, double>> insert_times = {
        {"BST", bst_insert_time},
        {"AVL", avl_insert_time},
        {"RBT", rbt_insert_time}
    };
    sort(insert_times.begin(), insert_times.end(), 
         [](const auto& a, const auto& b) { return a.second < b.second; });
    
    for (const auto& p : insert_times) {
        cout << p.first << "(" << p.second << "ms) ";
    }
    cout << endl;
    
    cout << "查找性能排序: ";
    vector<pair<string, double>> search_times = {
        {"BST", bst_search_time},
        {"AVL", avl_search_time},
        {"RBT", rbt_search_time}
    };
    sort(search_times.begin(), search_times.end(), 
         [](const auto& a, const auto& b) { return a.second < b.second; });
    
    for (const auto& p : search_times) {
        cout << p.first << "(" << p.second << "ms) ";
    }
    cout << endl;
}

void test_advanced_operations() {
    cout << "\n=== 高级操作测试 ===\n";
    
    // 树的合并
    cout << "树的合并:" << endl;
    BinarySearchTree<int> bst1, bst2;
    
    vector<int> data1 = {10, 5, 15, 2, 7, 12, 18};
    vector<int> data2 = {20, 8, 25, 6, 11, 22, 30};
    
    for (int val : data1) bst1.insert(val);
    for (int val : data2) bst2.insert(val);
    
    cout << "BST1: ";
    bst1.print_sorted();
    cout << "BST2: ";
    bst2.print_sorted();
    
    auto merged_bst = merge_bst(bst1, bst2);
    cout << "合并后: ";
    merged_bst.print_sorted();
    
    // 子树提取
    cout << "\n子树提取:" << endl;
    auto subtree = extract_subtree(merged_bst, 8, 20);
    cout << "提取范围[8, 20]的子树: ";
    subtree.print_sorted();
    
    // AVL树合并
    cout << "\nAVL树合并:" << endl;
    AVLTree<int> avl1, avl2;
    
    for (int val : data1) avl1.insert(val);
    for (int val : data2) avl2.insert(val);
    
    auto merged_avl = merge_avl_trees(avl1, avl2);
    cout << "合并后的AVL树高度: " << merged_avl.height() << endl;
    cout << "合并后的AVL树大小: " << merged_avl.size() << endl;
    
    // 树的比较
    cout << "\n树的比较:" << endl;
    AVLTree<int> avl3;
    for (int val : data1) avl3.insert(val);
    
    cout << "avl1和avl3是否相同: " << (are_identical_avl(avl1, avl3) ? "是" : "否") << endl;
    cout << "avl1和avl2是否相同: " << (are_identical_avl(avl1, avl2) ? "是" : "否") << endl;
}

int main() {
    cout << "二叉树数据结构演示程序" << endl;
    cout << "========================" << endl;
    
    try {
        test_basic_binary_tree();
        test_binary_search_tree();
        test_avl_tree();
        test_red_black_tree();
        test_tree_construction();
        test_iterators();
        test_advanced_operations();
        performance_comparison();
        
        cout << "\n所有测试完成！" << endl;
        
    } catch (const exception& e) {
        cout << "错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}