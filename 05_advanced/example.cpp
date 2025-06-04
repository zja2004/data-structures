#include "union_find.h"
#include "segment_tree.h"
#include "fenwick_tree.h"
#include "trie.h"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;
using namespace union_find;
using namespace segment_tree;
using namespace fenwick_tree;
using namespace trie;

// 并查集测试
void test_union_find() {
    cout << "\n=== 并查集测试 ===\n";
    
    // 基础并查集测试
    cout << "\n1. 基础并查集测试\n";
    UnionFind uf(10);
    
    uf.unite(0, 1);
    uf.unite(2, 3);
    uf.unite(1, 2);
    
    cout << "连通性测试:\n";
    cout << "0和3连通: " << (uf.connected(0, 3) ? "是" : "否") << "\n";
    cout << "0和4连通: " << (uf.connected(0, 4) ? "是" : "否") << "\n";
    cout << "连通分量数: " << uf.count() << "\n";
    
    // 加权并查集测试
    cout << "\n2. 加权并查集测试\n";
    WeightedUnionFind wuf(5);
    
    wuf.unite(0, 1, 3);  // 0到1的距离为3
    wuf.unite(1, 2, 2);  // 1到2的距离为2
    
    cout << "0到2的距离: " << wuf.distance(0, 2) << "\n";
    
    // 可撤销并查集测试
    cout << "\n3. 可撤销并查集测试\n";
    UndoableUnionFind uuf(5);
    
    uuf.unite(0, 1);
    uuf.unite(2, 3);
    cout << "操作后连通分量数: " << uuf.count() << "\n";
    
    uuf.undo();
    cout << "撤销一次后连通分量数: " << uuf.count() << "\n";
    
    // 应用测试：朋友圈问题
    cout << "\n4. 应用测试：朋友圈问题\n";
    vector<vector<int>> friends = {
        {1, 1, 0},
        {1, 1, 0},
        {0, 0, 1}
    };
    
    UnionFindApplications app;
    int circles = app.friend_circles(friends);
    cout << "朋友圈数量: " << circles << "\n";
}

// 线段树测试
void test_segment_tree() {
    cout << "\n=== 线段树测试 ===\n";
    
    // 基础线段树测试
    cout << "\n1. 基础线段树测试（区间和）\n";
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    SegmentTree st(arr);
    
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << "\n";
    
    cout << "区间[1,4]的和: " << st.query(1, 4) << "\n";
    
    st.update(2, 10);
    cout << "更新arr[2]=10后，区间[1,4]的和: " << st.query(1, 4) << "\n";
    
    // 懒惰传播线段树测试
    cout << "\n2. 懒惰传播线段树测试\n";
    LazySegmentTree lst(arr);
    
    cout << "区间[1,3]加5后:\n";
    lst.range_update(1, 3, 5);
    
    for (int i = 0; i < arr.size(); ++i) {
        cout << "arr[" << i << "] = " << lst.query(i, i) << "\n";
    }
    
    // 动态线段树测试
    cout << "\n3. 动态线段树测试\n";
    DynamicSegmentTree dst(0, 1000000);
    
    dst.update(100, 50);
    dst.update(500, 30);
    dst.update(1000, 20);
    
    cout << "区间[50,600]的和: " << dst.query(50, 600) << "\n";
    
    // 应用测试：RMQ问题
    cout << "\n4. 应用测试：RMQ问题\n";
    vector<int> rmq_arr = {2, 4, 3, 1, 6, 7, 8, 9, 1, 7};
    SegmentTreeApplications app;
    
    cout << "数组: ";
    for (int x : rmq_arr) cout << x << " ";
    cout << "\n";
    
    cout << "区间[2,7]的最小值: " << app.range_min_query(rmq_arr, 2, 7) << "\n";
}

// 树状数组测试
void test_fenwick_tree() {
    cout << "\n=== 树状数组测试 ===\n";
    
    // 基础树状数组测试
    cout << "\n1. 基础树状数组测试\n";
    vector<int> arr = {1, 2, 3, 4, 5};
    FenwickTree ft(arr);
    
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << "\n";
    
    cout << "前缀和[0,3]: " << ft.prefix_sum(3) << "\n";
    cout << "区间和[1,3]: " << ft.range_sum(1, 3) << "\n";
    
    ft.update(2, 5);  // arr[2] += 5
    cout << "更新arr[2]+=5后，区间和[1,3]: " << ft.range_sum(1, 3) << "\n";
    
    // 区间更新树状数组测试
    cout << "\n2. 区间更新树状数组测试\n";
    RangeUpdateFenwickTree ruft(5);
    
    ruft.range_update(1, 3, 2);  // [1,3]区间加2
    
    cout << "区间[1,3]加2后的数组: ";
    for (int i = 0; i < 5; ++i) {
        cout << ruft.point_query(i) << " ";
    }
    cout << "\n";
    
    // 二维树状数组测试
    cout << "\n3. 二维树状数组测试\n";
    FenwickTree2D ft2d(3, 3);
    
    ft2d.update(1, 1, 5);
    ft2d.update(2, 2, 3);
    
    cout << "矩形(0,0)到(2,2)的和: " << ft2d.range_sum(0, 0, 2, 2) << "\n";
    
    // 应用测试：逆序对计数
    cout << "\n4. 应用测试：逆序对计数\n";
    vector<int> inv_arr = {3, 1, 4, 1, 5};
    FenwickTreeApplications app;
    
    cout << "数组: ";
    for (int x : inv_arr) cout << x << " ";
    cout << "\n";
    
    cout << "逆序对数量: " << app.count_inversions(inv_arr) << "\n";
}

// 字典树测试
void test_trie() {
    cout << "\n=== 字典树测试 ===\n";
    
    // 基础字典树测试
    cout << "\n1. 基础字典树测试\n";
    Trie trie;
    
    vector<string> words = {"apple", "app", "application", "apply", "banana"};
    
    cout << "插入单词: ";
    for (const string& word : words) {
        trie.insert(word);
        cout << word << " ";
    }
    cout << "\n";
    
    cout << "\n查找测试:\n";
    vector<string> search_words = {"app", "apple", "appl", "application"};
    for (const string& word : search_words) {
        cout << "查找 '" << word << "': " << (trie.search(word) ? "找到" : "未找到") << "\n";
    }
    
    cout << "\n前缀测试:\n";
    vector<string> prefixes = {"app", "ban", "cat"};
    for (const string& prefix : prefixes) {
        cout << "前缀 '" << prefix << "': " << (trie.starts_with(prefix) ? "存在" : "不存在") << "\n";
    }
    
    // 自动补全测试
    cout << "\n2. 自动补全测试\n";
    auto completions = trie.auto_complete("app", 3);
    cout << "以'app'开头的单词(最多3个): ";
    for (const string& word : completions) {
        cout << word << " ";
    }
    cout << "\n";
    
    // 压缩字典树测试
    cout << "\n3. 压缩字典树测试\n";
    CompressedTrie ctrie;
    
    for (const string& word : words) {
        ctrie.insert(word);
    }
    
    cout << "压缩字典树查找测试:\n";
    for (const string& word : search_words) {
        cout << "查找 '" << word << "': " << (ctrie.search(word) ? "找到" : "未找到") << "\n";
    }
    
    // AC自动机测试
    cout << "\n4. AC自动机测试\n";
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
    
    // 应用测试：最长公共前缀
    cout << "\n5. 应用测试：最长公共前缀\n";
    vector<string> lcp_words = {"flower", "flow", "flight"};
    string lcp = TrieApplications::longest_common_prefix(lcp_words);
    
    cout << "单词组: ";
    for (const string& word : lcp_words) cout << word << " ";
    cout << "\n最长公共前缀: '" << lcp << "'\n";
}

// 性能对比测试
void performance_comparison() {
    cout << "\n=== 性能对比测试 ===\n";
    
    const int n = 100000;
    
    // 并查集性能测试
    cout << "\n1. 并查集性能测试\n";
    UnionFindTester::performance_test(n, n / 10);
    
    // 线段树性能测试
    cout << "\n2. 线段树性能测试\n";
    SegmentTreeTester::performance_test(n, n / 10);
    
    // 树状数组性能测试
    cout << "\n3. 树状数组性能测试\n";
    FenwickTreeTester::performance_test(n, n / 10);
    
    // 字典树性能测试
    cout << "\n4. 字典树性能测试\n";
    TrieTester::performance_test(n / 10, 8);
}

// 正确性测试
void correctness_tests() {
    cout << "\n=== 正确性测试 ===\n";
    
    UnionFindTester::correctness_test();
    SegmentTreeTester::correctness_test();
    FenwickTreeTester::correctness_test();
    TrieTester::correctness_test();
    
    cout << "\n所有正确性测试通过！\n";
}

// 应用场景演示
void application_scenarios() {
    cout << "\n=== 应用场景演示 ===\n";
    
    // 图连通性问题
    cout << "\n1. 图连通性问题（并查集）\n";
    vector<pair<int, int>> edges = {{0,1}, {1,2}, {3,4}, {5,6}, {6,7}};
    UnionFind uf(8);
    
    for (auto edge : edges) {
        uf.unite(edge.first, edge.second);
    }
    
    cout << "图的连通分量数: " << uf.count() << "\n";
    
    // 区间查询问题
    cout << "\n2. 区间查询问题（线段树 vs 树状数组）\n";
    vector<int> data(1000);
    for (int i = 0; i < 1000; ++i) {
        data[i] = rand() % 100;
    }
    
    SegmentTree st(data);
    FenwickTree ft(data);
    
    // 比较查询结果
    int left = 100, right = 500;
    int st_result = st.query(left, right);
    int ft_result = ft.range_sum(left, right);
    
    cout << "区间[" << left << "," << right << "]的和:\n";
    cout << "线段树结果: " << st_result << "\n";
    cout << "树状数组结果: " << ft_result << "\n";
    cout << "结果一致: " << (st_result == ft_result ? "是" : "否") << "\n";
    
    // 字符串匹配问题
    cout << "\n3. 字符串匹配问题（字典树 vs AC自动机）\n";
    vector<string> dictionary = {"cat", "car", "card", "care", "careful"};
    string text = "take care of the cat and car";
    
    // 使用字典树
    Trie trie;
    for (const string& word : dictionary) {
        trie.insert(word);
    }
    
    cout << "字典树查找结果:\n";
    istringstream iss(text);
    string word;
    while (iss >> word) {
        if (trie.search(word)) {
            cout << "找到单词: " << word << "\n";
        }
    }
    
    // 使用AC自动机
    AhoCorasick ac;
    for (const string& pattern : dictionary) {
        ac.add_pattern(pattern);
    }
    ac.build();
    
    auto matches = ac.search(text);
    cout << "AC自动机匹配结果:\n";
    for (const auto& match : matches) {
        cout << "位置 " << match.first << ": " << ac.get_patterns()[match.second] << "\n";
    }
}

// 综合测试
void comprehensive_test() {
    cout << "\n=== 综合测试 ===\n";
    
    // 模拟一个复杂的数据处理场景
    cout << "\n模拟场景：社交网络分析\n";
    
    const int users = 1000;
    const int connections = 5000;
    
    // 使用并查集分析用户连通性
    UnionFind social_network(users);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> user_dis(0, users - 1);
    
    cout << "建立 " << connections << " 个用户连接...\n";
    for (int i = 0; i < connections; ++i) {
        int u = user_dis(gen);
        int v = user_dis(gen);
        if (u != v) {
            social_network.unite(u, v);
        }
    }
    
    cout << "社交网络连通分量数: " << social_network.count() << "\n";
    
    // 使用字典树存储用户标签
    Trie user_tags;
    vector<string> tags = {"tech", "music", "sports", "travel", "food", "art"};
    
    cout << "\n用户标签系统:\n";
    for (const string& tag : tags) {
        user_tags.insert(tag);
        cout << "添加标签: " << tag << "\n";
    }
    
    // 自动补全功能
    auto suggestions = user_tags.auto_complete("t", 3);
    cout << "以't'开头的标签建议: ";
    for (const string& suggestion : suggestions) {
        cout << suggestion << " ";
    }
    cout << "\n";
    
    // 使用线段树统计活跃度
    vector<int> daily_activity(365, 0);
    uniform_int_distribution<int> activity_dis(0, 100);
    
    for (int i = 0; i < 365; ++i) {
        daily_activity[i] = activity_dis(gen);
    }
    
    SegmentTree activity_tree(daily_activity);
    
    cout << "\n活跃度统计:\n";
    cout << "第一季度总活跃度: " << activity_tree.query(0, 89) << "\n";
    cout << "第二季度总活跃度: " << activity_tree.query(90, 179) << "\n";
    cout << "全年总活跃度: " << activity_tree.query(0, 364) << "\n";
}

int main() {
    cout << "高级数据结构教程示例\n";
    cout << "===================\n";
    
    try {
        // 基础功能测试
        test_union_find();
        test_segment_tree();
        test_fenwick_tree();
        test_trie();
        
        // 正确性测试
        correctness_tests();
        
        // 应用场景演示
        application_scenarios();
        
        // 性能对比测试
        performance_comparison();
        
        // 综合测试
        comprehensive_test();
        
        cout << "\n\n所有测试完成！\n";
        
    } catch (const exception& e) {
        cerr << "测试过程中发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}