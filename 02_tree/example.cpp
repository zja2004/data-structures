#include "binary_tree/binary_tree.h"
#include "binary_tree/binary_search_tree.h"
#include "binary_tree/avl_tree.h"
#include "binary_tree/red_black_tree.h"
#include "heap/binary_heap.h"
#include "heap/binomial_heap.h"
#include "heap/fibonacci_heap.h"
#include "trie/trie.h"
#include "trie/compressed_trie.h"
#include "trie/ac_automaton.h"
#include "segment_tree/segment_tree.h"
#include "segment_tree/lazy_segment_tree.h"
#include "fenwick_tree/fenwick_tree.h"
#include "fenwick_tree/fenwick_tree_2d.h"
#include "btree/btree.h"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
#include <cassert>
#include <queue>

using namespace std;
using namespace chrono;

// 性能测试工具类
class TreePerformanceTester {
public:
    template<typename Func>
    static double measure_time(Func func, const string& description) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        cout << description << ": " << duration << " 微秒\n";
        return duration;
    }
    
    static void print_separator(const string& title) {
        cout << "\n" << string(60, '=') << "\n";
        cout << "  " << title << "\n";
        cout << string(60, '=') << "\n";
    }
};

// 二叉树基础测试
void test_binary_tree() {
    TreePerformanceTester::print_separator("二叉树基础测试");
    
    cout << "\n1. 二叉树构建和遍历\n";
    
    // 手动构建二叉树
    //       1
    //      / \
    //     2   3
    //    / \
    //   4   5
    
    BinaryTree<int> tree;
    auto root = tree.create_node(1);
    auto left = tree.create_node(2);
    auto right = tree.create_node(3);
    auto left_left = tree.create_node(4);
    auto left_right = tree.create_node(5);
    
    tree.set_root(root);
    tree.set_left_child(root, left);
    tree.set_right_child(root, right);
    tree.set_left_child(left, left_left);
    tree.set_right_child(left, left_right);
    
    cout << "树的结构:\n";
    cout << "       1\n";
    cout << "      / \\\n";
    cout << "     2   3\n";
    cout << "    / \\\n";
    cout << "   4   5\n\n";
    
    cout << "前序遍历: ";
    tree.preorder_traversal([](int val) { cout << val << " "; });
    cout << "\n";
    
    cout << "中序遍历: ";
    tree.inorder_traversal([](int val) { cout << val << " "; });
    cout << "\n";
    
    cout << "后序遍历: ";
    tree.postorder_traversal([](int val) { cout << val << " "; });
    cout << "\n";
    
    cout << "层序遍历: ";
    tree.level_order_traversal([](int val) { cout << val << " "; });
    cout << "\n";
    
    cout << "树的高度: " << tree.height() << "\n";
    cout << "节点数量: " << tree.size() << "\n";
}

// 二叉搜索树测试
void test_binary_search_tree() {
    TreePerformanceTester::print_separator("二叉搜索树测试");
    
    cout << "\n1. BST基本操作\n";
    BinarySearchTree<int> bst;
    
    vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35, 45};
    
    cout << "插入元素: ";
    for (int val : values) {
        bst.insert(val);
        cout << val << " ";
    }
    cout << "\n";
    
    cout << "中序遍历(应该是有序的): ";
    bst.inorder([](int val) { cout << val << " "; });
    cout << "\n";
    
    // 查找测试
    vector<int> search_values = {25, 55, 80, 100};
    cout << "\n查找测试:\n";
    for (int val : search_values) {
        cout << "查找 " << val << ": " << (bst.search(val) ? "找到" : "未找到") << "\n";
    }
    
    cout << "\n最小值: " << bst.find_min() << "\n";
    cout << "最大值: " << bst.find_max() << "\n";
    
    // 删除测试
    cout << "\n删除节点30后的中序遍历: ";
    bst.remove(30);
    bst.inorder([](int val) { cout << val << " "; });
    cout << "\n";
}

// AVL树测试
void test_avl_tree() {
    TreePerformanceTester::print_separator("AVL树测试");
    
    cout << "\n1. AVL树自平衡特性\n";
    AVLTree<int> avl;
    
    // 插入会导致不平衡的序列
    vector<int> sequence = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    cout << "插入递增序列: ";
    for (int val : sequence) {
        avl.insert(val);
        cout << val << " ";
    }
    cout << "\n";
    
    cout << "AVL树高度: " << avl.height() << " (理想高度约为 " 
         << (int)ceil(log2(sequence.size() + 1)) << ")\n";
    
    cout << "中序遍历: ";
    avl.inorder([](int val) { cout << val << " "; });
    cout << "\n";
    
    cout << "\n2. 平衡因子检查\n";
    cout << "验证AVL性质: " << (avl.is_balanced() ? "平衡" : "不平衡") << "\n";
    
    // 删除测试
    cout << "\n删除一些节点后仍保持平衡:\n";
    vector<int> to_remove = {1, 3, 5, 7, 9};
    for (int val : to_remove) {
        avl.remove(val);
        cout << "删除 " << val << ", 高度: " << avl.height() 
             << ", 平衡: " << (avl.is_balanced() ? "是" : "否") << "\n";
    }
}

// 红黑树测试
void test_red_black_tree() {
    TreePerformanceTester::print_separator("红黑树测试");
    
    cout << "\n1. 红黑树基本操作\n";
    RedBlackTree<int> rbt;
    
    vector<int> values = {10, 20, 30, 15, 25, 5, 1, 35};
    
    cout << "插入元素: ";
    for (int val : values) {
        rbt.insert(val);
        cout << val << " ";
    }
    cout << "\n";
    
    cout << "中序遍历: ";
    rbt.inorder([](int val) { cout << val << " "; });
    cout << "\n";
    
    cout << "树的高度: " << rbt.height() << "\n";
    cout << "黑高度: " << rbt.black_height() << "\n";
    
    cout << "\n2. 红黑树性质验证\n";
    cout << "验证红黑树性质: " << (rbt.is_valid_red_black_tree() ? "有效" : "无效") << "\n";
    
    // 删除测试
    cout << "\n删除节点20:\n";
    rbt.remove(20);
    cout << "删除后中序遍历: ";
    rbt.inorder([](int val) { cout << val << " "; });
    cout << "\n";
    cout << "删除后仍然有效: " << (rbt.is_valid_red_black_tree() ? "是" : "否") << "\n";
}

// 堆测试
void test_heap() {
    TreePerformanceTester::print_separator("堆测试");
    
    cout << "\n1. 二叉堆测试\n";
    BinaryHeap<int> max_heap(true);  // 最大堆
    BinaryHeap<int> min_heap(false); // 最小堆
    
    vector<int> values = {4, 10, 3, 5, 1, 8, 9, 2, 7, 6};
    
    cout << "插入元素: ";
    for (int val : values) {
        max_heap.push(val);
        min_heap.push(val);
        cout << val << " ";
    }
    cout << "\n";
    
    cout << "\n最大堆出队序列: ";
    BinaryHeap<int> temp_max = max_heap;
    while (!temp_max.empty()) {
        cout << temp_max.top() << " ";
        temp_max.pop();
    }
    cout << "\n";
    
    cout << "最小堆出队序列: ";
    BinaryHeap<int> temp_min = min_heap;
    while (!temp_min.empty()) {
        cout << temp_min.top() << " ";
        temp_min.pop();
    }
    cout << "\n";
    
    cout << "\n2. 堆排序演示\n";
    vector<int> unsorted = {64, 34, 25, 12, 22, 11, 90};
    cout << "原数组: ";
    for (int val : unsorted) cout << val << " ";
    cout << "\n";
    
    // 使用最大堆进行降序排序
    BinaryHeap<int> sort_heap(true);
    for (int val : unsorted) {
        sort_heap.push(val);
    }
    
    cout << "堆排序结果(降序): ";
    while (!sort_heap.empty()) {
        cout << sort_heap.top() << " ";
        sort_heap.pop();
    }
    cout << "\n";
    
    cout << "\n3. 优先队列应用\n";
    struct Task {
        string name;
        int priority;
        
        bool operator<(const Task& other) const {
            return priority < other.priority;  // 优先级高的先执行
        }
    };
    
    BinaryHeap<Task> task_queue(true);
    task_queue.push({"低优先级任务", 1});
    task_queue.push({"紧急任务", 5});
    task_queue.push({"普通任务", 3});
    task_queue.push({"重要任务", 4});
    
    cout << "任务执行顺序:\n";
    while (!task_queue.empty()) {
        Task task = task_queue.top();
        task_queue.pop();
        cout << "执行: " << task.name << " (优先级: " << task.priority << ")\n";
    }
}

// 字典树测试
void test_trie() {
    TreePerformanceTester::print_separator("字典树测试");
    
    cout << "\n1. 基础字典树操作\n";
    Trie trie;
    
    vector<string> words = {"apple", "app", "application", "apply", "banana", "band", "bandana"};
    
    cout << "插入单词: ";
    for (const string& word : words) {
        trie.insert(word);
        cout << word << " ";
    }
    cout << "\n";
    
    cout << "\n查找测试:\n";
    vector<string> search_words = {"app", "apple", "appl", "application", "cat"};
    for (const string& word : search_words) {
        cout << "查找 '" << word << "': " << (trie.search(word) ? "找到" : "未找到") << "\n";
    }
    
    cout << "\n前缀测试:\n";
    vector<string> prefixes = {"app", "ban", "cat", "a"};
    for (const string& prefix : prefixes) {
        cout << "前缀 '" << prefix << "': " << (trie.starts_with(prefix) ? "存在" : "不存在") << "\n";
    }
    
    cout << "\n自动补全测试:\n";
    string prefix = "app";
    auto suggestions = trie.get_words_with_prefix(prefix);
    cout << "以 '" << prefix << "' 开头的单词: ";
    for (const string& word : suggestions) {
        cout << word << " ";
    }
    cout << "\n";
    
    cout << "\n2. AC自动机测试\n";
    AhoCorasick ac;
    vector<string> patterns = {"he", "she", "his", "hers"};
    
    for (const string& pattern : patterns) {
        ac.add_pattern(pattern);
    }
    ac.build();
    
    string text = "ushers";
    auto matches = ac.search(text);
    
    cout << "在文本 '" << text << "' 中找到的模式:\n";
    for (const auto& match : matches) {
        cout << "位置 " << match.first << ": " << ac.get_patterns()[match.second] << "\n";
    }
}

// 线段树测试
void test_segment_tree() {
    TreePerformanceTester::print_separator("线段树测试");
    
    cout << "\n1. 基础线段树(区间和)\n";
    vector<int> arr = {1, 3, 5, 7, 9, 11, 13, 15};
    SegmentTree st(arr);
    
    cout << "原数组: ";
    for (int val : arr) cout << val << " ";
    cout << "\n";
    
    cout << "\n区间查询测试:\n";
    vector<pair<int, int>> queries = {{0, 3}, {2, 5}, {1, 6}, {0, 7}};
    for (auto query : queries) {
        int sum = st.query(query.first, query.second);
        cout << "区间[" << query.first << ", " << query.second << "]的和: " << sum << "\n";
    }
    
    cout << "\n单点更新测试:\n";
    st.update(3, 100);
    cout << "将arr[3]更新为100后:\n";
    for (auto query : queries) {
        int sum = st.query(query.first, query.second);
        cout << "区间[" << query.first << ", " << query.second << "]的和: " << sum << "\n";
    }
    
    cout << "\n2. 懒惰传播线段树(区间更新)\n";
    LazySegmentTree lst(arr);
    
    cout << "区间[1, 4]加上10:\n";
    lst.range_update(1, 4, 10);
    
    cout << "更新后的数组: ";
    for (int i = 0; i < arr.size(); ++i) {
        cout << lst.query(i, i) << " ";
    }
    cout << "\n";
    
    cout << "区间[2, 6]的和: " << lst.query(2, 6) << "\n";
}

// 树状数组测试
void test_fenwick_tree() {
    TreePerformanceTester::print_separator("树状数组测试");
    
    cout << "\n1. 一维树状数组\n";
    vector<int> arr = {2, 1, 1, 3, 2, 3, 4, 5, 6, 7, 8, 9};
    FenwickTree ft(arr);
    
    cout << "原数组: ";
    for (int val : arr) cout << val << " ";
    cout << "\n";
    
    cout << "\n前缀和查询:\n";
    for (int i = 0; i < arr.size(); ++i) {
        cout << "前缀和[0, " << i << "]: " << ft.prefix_sum(i) << "\n";
    }
    
    cout << "\n区间和查询:\n";
    vector<pair<int, int>> ranges = {{1, 3}, {2, 5}, {4, 8}};
    for (auto range : ranges) {
        int sum = ft.range_sum(range.first, range.second);
        cout << "区间[" << range.first << ", " << range.second << "]的和: " << sum << "\n";
    }
    
    cout << "\n单点更新:\n";
    ft.update(3, 5);  // arr[3] += 5
    cout << "arr[3] += 5后，前缀和[0, 5]: " << ft.prefix_sum(5) << "\n";
    
    cout << "\n2. 二维树状数组\n";
    FenwickTree2D ft2d(4, 4);
    
    // 更新一些位置
    ft2d.update(1, 1, 5);
    ft2d.update(2, 2, 3);
    ft2d.update(3, 3, 7);
    
    cout << "矩形(0,0)到(2,2)的和: " << ft2d.range_sum(0, 0, 2, 2) << "\n";
    cout << "矩形(1,1)到(3,3)的和: " << ft2d.range_sum(1, 1, 3, 3) << "\n";
}

// B树测试
void test_btree() {
    TreePerformanceTester::print_separator("B树测试");
    
    cout << "\n1. B树基本操作\n";
    BTree<int> btree(3);  // 最小度数为3的B树
    
    vector<int> values = {10, 20, 5, 6, 12, 30, 7, 17, 15, 18, 8, 16};
    
    cout << "插入元素: ";
    for (int val : values) {
        btree.insert(val);
        cout << val << " ";
    }
    cout << "\n";
    
    cout << "\nB树结构:\n";
    btree.print();
    
    cout << "\n查找测试:\n";
    vector<int> search_vals = {6, 15, 25, 30};
    for (int val : search_vals) {
        cout << "查找 " << val << ": " << (btree.search(val) ? "找到" : "未找到") << "\n";
    }
    
    cout << "\n删除测试:\n";
    btree.remove(6);
    cout << "删除6后的B树结构:\n";
    btree.print();
}

// 性能对比测试
void performance_comparison() {
    TreePerformanceTester::print_separator("性能对比测试");
    
    const int n = 100000;
    vector<int> data(n);
    
    // 生成随机数据
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(1, 1000000);
    
    for (int i = 0; i < n; ++i) {
        data[i] = dis(gen);
    }
    
    cout << "\n测试数据规模: " << n << " 个随机整数\n";
    
    cout << "\n1. 插入性能对比\n";
    
    // BST插入
    TreePerformanceTester::measure_time([&]() {
        BinarySearchTree<int> bst;
        for (int val : data) {
            bst.insert(val);
        }
    }, "BST插入");
    
    // AVL树插入
    TreePerformanceTester::measure_time([&]() {
        AVLTree<int> avl;
        for (int val : data) {
            avl.insert(val);
        }
    }, "AVL树插入");
    
    // 红黑树插入
    TreePerformanceTester::measure_time([&]() {
        RedBlackTree<int> rbt;
        for (int val : data) {
            rbt.insert(val);
        }
    }, "红黑树插入");
    
    cout << "\n2. 查找性能对比\n";
    
    // 构建测试用的树
    BinarySearchTree<int> bst;
    AVLTree<int> avl;
    RedBlackTree<int> rbt;
    
    for (int val : data) {
        bst.insert(val);
        avl.insert(val);
        rbt.insert(val);
    }
    
    // 生成查找数据
    vector<int> search_data(10000);
    for (int i = 0; i < 10000; ++i) {
        search_data[i] = dis(gen);
    }
    
    // BST查找
    TreePerformanceTester::measure_time([&]() {
        int found = 0;
        for (int val : search_data) {
            if (bst.search(val)) found++;
        }
    }, "BST查找10000次");
    
    // AVL树查找
    TreePerformanceTester::measure_time([&]() {
        int found = 0;
        for (int val : search_data) {
            if (avl.search(val)) found++;
        }
    }, "AVL树查找10000次");
    
    // 红黑树查找
    TreePerformanceTester::measure_time([&]() {
        int found = 0;
        for (int val : search_data) {
            if (rbt.search(val)) found++;
        }
    }, "红黑树查找10000次");
    
    cout << "\n3. 区间查询性能对比\n";
    
    vector<int> range_data(n);
    for (int i = 0; i < n; ++i) {
        range_data[i] = i + 1;
    }
    
    SegmentTree st(range_data);
    FenwickTree ft(range_data);
    
    // 生成查询区间
    vector<pair<int, int>> queries(1000);
    uniform_int_distribution<int> index_dis(0, n - 1);
    for (int i = 0; i < 1000; ++i) {
        int left = index_dis(gen);
        int right = index_dis(gen);
        if (left > right) swap(left, right);
        queries[i] = {left, right};
    }
    
    // 线段树查询
    TreePerformanceTester::measure_time([&]() {
        long long sum = 0;
        for (auto query : queries) {
            sum += st.query(query.first, query.second);
        }
    }, "线段树区间查询1000次");
    
    // 树状数组查询
    TreePerformanceTester::measure_time([&]() {
        long long sum = 0;
        for (auto query : queries) {
            sum += ft.range_sum(query.first, query.second);
        }
    }, "树状数组区间查询1000次");
}

// 实际应用场景演示
void application_scenarios() {
    TreePerformanceTester::print_separator("实际应用场景演示");
    
    cout << "\n1. 文件系统目录树\n";
    
    struct FileNode {
        string name;
        bool is_directory;
        vector<FileNode*> children;
        
        FileNode(const string& n, bool is_dir = false) 
            : name(n), is_directory(is_dir) {}
        
        void add_child(FileNode* child) {
            children.push_back(child);
        }
        
        void print(int depth = 0) {
            cout << string(depth * 2, ' ') << (is_directory ? "[DIR] " : "[FILE] ") << name << "\n";
            for (FileNode* child : children) {
                child->print(depth + 1);
            }
        }
    };
    
    // 构建文件系统树
    FileNode root("root", true);
    FileNode* home = new FileNode("home", true);
    FileNode* user = new FileNode("user", true);
    FileNode* documents = new FileNode("Documents", true);
    FileNode* file1 = new FileNode("readme.txt");
    FileNode* file2 = new FileNode("project.cpp");
    
    root.add_child(home);
    home->add_child(user);
    user->add_child(documents);
    documents->add_child(file1);
    documents->add_child(file2);
    
    cout << "文件系统结构:\n";
    root.print();
    
    cout << "\n2. 表达式求值树\n";
    
    struct ExprNode {
        string value;
        ExprNode* left;
        ExprNode* right;
        
        ExprNode(const string& val) : value(val), left(nullptr), right(nullptr) {}
        
        double evaluate() {
            if (!left && !right) {
                // 叶子节点，返回数值
                return stod(value);
            }
            
            double left_val = left->evaluate();
            double right_val = right->evaluate();
            
            if (value == "+") return left_val + right_val;
            if (value == "-") return left_val - right_val;
            if (value == "*") return left_val * right_val;
            if (value == "/") return left_val / right_val;
            
            return 0;
        }
        
        void print_infix() {
            if (left && right) {
                cout << "(";
                left->print_infix();
                cout << " " << value << " ";
                right->print_infix();
                cout << ")";
            } else {
                cout << value;
            }
        }
    };
    
    // 构建表达式树: (3 + 4) * (5 - 2)
    ExprNode* root_expr = new ExprNode("*");
    ExprNode* left_add = new ExprNode("+");
    ExprNode* right_sub = new ExprNode("-");
    
    left_add->left = new ExprNode("3");
    left_add->right = new ExprNode("4");
    right_sub->left = new ExprNode("5");
    right_sub->right = new ExprNode("2");
    
    root_expr->left = left_add;
    root_expr->right = right_sub;
    
    cout << "表达式: ";
    root_expr->print_infix();
    cout << "\n结果: " << root_expr->evaluate() << "\n";
    
    cout << "\n3. 游戏技能树\n";
    
    struct SkillNode {
        string name;
        int level_required;
        bool unlocked;
        vector<SkillNode*> prerequisites;
        vector<SkillNode*> unlocks;
        
        SkillNode(const string& n, int level) 
            : name(n), level_required(level), unlocked(false) {}
        
        bool can_unlock(int player_level) {
            if (player_level < level_required) return false;
            
            for (SkillNode* prereq : prerequisites) {
                if (!prereq->unlocked) return false;
            }
            return true;
        }
        
        void unlock() {
            unlocked = true;
            cout << "解锁技能: " << name << "\n";
        }
    };
    
    // 构建技能树
    SkillNode* basic_attack = new SkillNode("基础攻击", 1);
    SkillNode* power_strike = new SkillNode("强力打击", 5);
    SkillNode* combo_attack = new SkillNode("连击", 10);
    SkillNode* ultimate = new SkillNode("终极技能", 20);
    
    power_strike->prerequisites.push_back(basic_attack);
    combo_attack->prerequisites.push_back(power_strike);
    ultimate->prerequisites.push_back(combo_attack);
    
    // 模拟玩家升级解锁技能
    vector<SkillNode*> skills = {basic_attack, power_strike, combo_attack, ultimate};
    
    for (int level = 1; level <= 25; level += 5) {
        cout << "\n玩家等级 " << level << ":\n";
        for (SkillNode* skill : skills) {
            if (!skill->unlocked && skill->can_unlock(level)) {
                skill->unlock();
            }
        }
    }
}

// 正确性测试
void correctness_tests() {
    TreePerformanceTester::print_separator("正确性测试");
    
    cout << "\n运行所有树结构的正确性测试...\n";
    
    // BST正确性测试
    {
        BinarySearchTree<int> bst;
        vector<int> values = {5, 3, 7, 2, 4, 6, 8};
        
        for (int val : values) {
            bst.insert(val);
        }
        
        // 验证中序遍历是有序的
        vector<int> inorder_result;
        bst.inorder([&](int val) { inorder_result.push_back(val); });
        
        assert(is_sorted(inorder_result.begin(), inorder_result.end()));
        assert(bst.search(5) && bst.search(3) && bst.search(7));
        assert(!bst.search(10));
        
        cout << "✓ BST测试通过\n";
    }
    
    // AVL树正确性测试
    {
        AVLTree<int> avl;
        for (int i = 1; i <= 15; ++i) {
            avl.insert(i);
        }
        
        assert(avl.is_balanced());
        assert(avl.height() <= 2 * log2(15 + 1));  // AVL树高度限制
        
        cout << "✓ AVL树测试通过\n";
    }
    
    // 堆正确性测试
    {
        BinaryHeap<int> max_heap(true);
        vector<int> values = {3, 1, 4, 1, 5, 9, 2, 6};
        
        for (int val : values) {
            max_heap.push(val);
        }
        
        vector<int> sorted_desc;
        while (!max_heap.empty()) {
            sorted_desc.push_back(max_heap.top());
            max_heap.pop();
        }
        
        assert(is_sorted(sorted_desc.rbegin(), sorted_desc.rend()));
        
        cout << "✓ 堆测试通过\n";
    }
    
    // 线段树正确性测试
    {
        vector<int> arr = {1, 2, 3, 4, 5};
        SegmentTree st(arr);
        
        assert(st.query(0, 4) == 15);  // 全部元素和
        assert(st.query(1, 3) == 9);   // 2+3+4
        
        st.update(2, 10);  // arr[2] = 10
        assert(st.query(1, 3) == 16);  // 2+10+4
        
        cout << "✓ 线段树测试通过\n";
    }
    
    // 树状数组正确性测试
    {
        vector<int> arr = {1, 2, 3, 4, 5};
        FenwickTree ft(arr);
        
        assert(ft.prefix_sum(4) == 15);  // 前缀和
        assert(ft.range_sum(1, 3) == 9); // 区间和
        
        ft.update(2, 5);  // arr[2] += 5
        assert(ft.prefix_sum(4) == 20);
        
        cout << "✓ 树状数组测试通过\n";
    }
    
    cout << "\n所有正确性测试通过！\n";
}

int main() {
    cout << "C++树形数据结构教程示例\n";
    cout << "=========================\n";
    
    try {
        // 基础功能测试
        test_binary_tree();
        test_binary_search_tree();
        test_avl_tree();
        test_red_black_tree();
        test_heap();
        test_trie();
        test_segment_tree();
        test_fenwick_tree();
        test_btree();
        
        // 性能对比测试
        performance_comparison();
        
        // 实际应用场景
        application_scenarios();
        
        // 正确性测试
        correctness_tests();
        
        TreePerformanceTester::print_separator("测试完成");
        cout << "\n所有测试成功完成！\n";
        cout << "\n学习建议:\n";
        cout << "1. 理解不同树结构的特点和适用场景\n";
        cout << "2. 掌握树的遍历方法和应用\n";
        cout << "3. 学会分析树操作的时间复杂度\n";
        cout << "4. 在实际项目中选择合适的树结构\n";
        cout << "5. 深入研究平衡树的旋转操作\n";
        
    } catch (const exception& e) {
        cerr << "测试过程中发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}