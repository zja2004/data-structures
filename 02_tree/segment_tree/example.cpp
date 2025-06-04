#include "segment_tree.h"
#include "lazy_segment_tree.h"
#include "dynamic_segment_tree.h"
#include "persistent_segment_tree.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cassert>

using namespace std;
using namespace chrono;

// 测试基础线段树
void test_basic_segment_tree() {
    cout << "=== 测试基础线段树 ===" << endl;
    
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    
    // 测试求和线段树
    auto sum_tree = build_sum_segment_tree(arr);
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    cout << "区间 [1, 4] 的和: " << sum_tree.query(1, 4) << endl;  // 3+5+7+9 = 24
    cout << "区间 [0, 5] 的和: " << sum_tree.query(0, 5) << endl;  // 1+3+5+7+9+11 = 36
    
    // 单点更新
    sum_tree.update(2, 10);  // 将 arr[2] 从 5 改为 10
    cout << "更新 arr[2] = 10 后:" << endl;
    cout << "区间 [1, 4] 的和: " << sum_tree.query(1, 4) << endl;  // 3+10+7+9 = 29
    
    // 测试最大值线段树
    auto max_tree = build_max_segment_tree(arr);
    cout << "\n区间 [1, 4] 的最大值: " << max_tree.query(1, 4) << endl;
    
    // 测试最小值线段树
    auto min_tree = build_min_segment_tree(arr);
    cout << "区间 [1, 4] 的最小值: " << min_tree.query(1, 4) << endl;
    
    // 测试 GCD 线段树
    vector<int> gcd_arr = {12, 18, 24, 30};
    auto gcd_tree = build_gcd_segment_tree(gcd_arr);
    cout << "\nGCD 数组: ";
    for (int x : gcd_arr) cout << x << " ";
    cout << endl;
    cout << "区间 [0, 3] 的 GCD: " << gcd_tree.query(0, 3) << endl;  // gcd(12,18,24,30) = 6
    
    // 测试最大子数组和线段树
    vector<int> subarray_arr = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
    MaxSubarraySegmentTree subarray_tree(subarray_arr);
    cout << "\n最大子数组和数组: ";
    for (int x : subarray_arr) cout << x << " ";
    cout << endl;
    
    auto result = subarray_tree.query(0, 8);
    cout << "最大子数组和: " << result.max_sum << endl;
    cout << "最大前缀和: " << result.max_prefix << endl;
    cout << "最大后缀和: " << result.max_suffix << endl;
    cout << "总和: " << result.total_sum << endl;
    
    cout << "基础线段树测试完成!\n" << endl;
}

// 测试懒惰传播线段树
void test_lazy_segment_tree() {
    cout << "=== 测试懒惰传播线段树 ===" << endl;
    
    vector<int> arr = {1, 2, 3, 4, 5, 6};
    
    // 测试区间赋值求和树
    auto assign_sum_tree = build_range_assign_sum_tree(arr);
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    cout << "区间 [1, 4] 的和: " << assign_sum_tree.query(1, 4) << endl;  // 2+3+4+5 = 14
    
    // 区间赋值
    assign_sum_tree.update_range(1, 3, 10);  // 将 [1,3] 区间赋值为 10
    cout << "区间 [1, 3] 赋值为 10 后:" << endl;
    cout << "区间 [0, 5] 的和: " << assign_sum_tree.query(0, 5) << endl;  // 1+10+10+10+5+6 = 42
    
    // 测试区间加法求和树
    auto add_sum_tree = build_range_add_sum_tree(arr);
    cout << "\n区间加法测试:" << endl;
    cout << "区间 [1, 4] 的和: " << add_sum_tree.query(1, 4) << endl;  // 2+3+4+5 = 14
    
    // 区间加法
    add_sum_tree.update_range(1, 3, 5);  // 将 [1,3] 区间每个元素加 5
    cout << "区间 [1, 3] 每个元素加 5 后:" << endl;
    cout << "区间 [0, 5] 的和: " << add_sum_tree.query(0, 5) << endl;  // 1+(2+5)+(3+5)+(4+5)+5+6 = 36
    
    // 测试区间乘法求和树
    auto multiply_sum_tree = build_range_multiply_sum_tree(arr);
    cout << "\n区间乘法测试:" << endl;
    cout << "区间 [1, 4] 的和: " << multiply_sum_tree.query(1, 4) << endl;
    
    // 区间乘法
    multiply_sum_tree.update_range(1, 3, 2);  // 将 [1,3] 区间每个元素乘 2
    cout << "区间 [1, 3] 每个元素乘 2 后:" << endl;
    cout << "区间 [0, 5] 的和: " << multiply_sum_tree.query(0, 5) << endl;
    
    // 测试复合操作树
    RangeMultiplyAddSumTree<int> composite_tree(arr);
    cout << "\n复合操作测试:" << endl;
    cout << "区间 [1, 4] 的和: " << composite_tree.query(1, 4) << endl;
    
    // 先乘法再加法
    composite_tree.multiply_range(1, 3, 2);  // 乘 2
    composite_tree.add_range(1, 3, 1);       // 加 1
    cout << "区间 [1, 3] 先乘 2 再加 1 后:" << endl;
    cout << "区间 [0, 5] 的和: " << composite_tree.query(0, 5) << endl;
    
    cout << "懒惰传播线段树测试完成!\n" << endl;
}

// 测试动态线段树
void test_dynamic_segment_tree() {
    cout << "=== 测试动态线段树 ===" << endl;
    
    // 创建一个支持大范围的动态线段树
    auto dynamic_tree = build_dynamic_sum_tree<long long>(0, 1000000000);
    
    cout << "动态线段树范围: [0, 1000000000]" << endl;
    
    // 稀疏更新
    dynamic_tree.update(100, 5);
    dynamic_tree.update(1000000, 10);
    dynamic_tree.update(500000000, 15);
    
    cout << "更新位置 100, 1000000, 500000000" << endl;
    cout << "查询 [0, 1000000000]: " << dynamic_tree.query(0, 1000000000) << endl;  // 5+10+15 = 30
    cout << "查询 [100, 1000000]: " << dynamic_tree.query(100, 1000000) << endl;    // 5+10 = 15
    
    // 区间更新
    dynamic_tree.update_range(999999, 1000001, 3);
    cout << "区间 [999999, 1000001] 赋值为 3 后:" << endl;
    cout << "查询 [0, 1000000000]: " << dynamic_tree.query(0, 1000000000) << endl;
    
    cout << "节点数量: " << dynamic_tree.node_count() << endl;
    cout << "内存使用: " << dynamic_tree.memory_usage() << " 字节" << endl;
    
    // 测试坐标压缩动态线段树
    cout << "\n坐标压缩动态线段树测试:" << endl;
    vector<long long> coords = {1, 100, 1000, 1000000, 1000000000};
    CompressedDynamicSegmentTree<int> compressed_tree(
        coords,
        [](int a, int b) { return a + b; },
        [](int val, int lazy_val, long long len) { return lazy_val * len; },
        0,
        numeric_limits<int>::min()
    );
    
    compressed_tree.update(1, 5);
    compressed_tree.update(1000000, 10);
    cout << "压缩后的坐标: ";
    for (long long coord : compressed_tree.get_coordinates()) {
        cout << coord << " ";
    }
    cout << endl;
    
    cout << "查询 [1, 1000000]: " << compressed_tree.query(1, 1000000) << endl;
    
    cout << "动态线段树测试完成!\n" << endl;
}

// 测试可持久化线段树
void test_persistent_segment_tree() {
    cout << "=== 测试可持久化线段树 ===" << endl;
    
    vector<int> arr = {1, 2, 3, 4, 5};
    PersistentSumSegmentTree<int> persistent_tree(arr);
    
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    cout << "版本 0 - 区间 [0, 4] 的和: " << persistent_tree.query(0, 0, 4) << endl;  // 15
    
    // 创建新版本
    int v1 = persistent_tree.update(0, 2, 10);  // 将 arr[2] 从 3 改为 10
    cout << "版本 " << v1 << " - 更新 arr[2] = 10" << endl;
    cout << "版本 " << v1 << " - 区间 [0, 4] 的和: " << persistent_tree.query(v1, 0, 4) << endl;  // 22
    
    int v2 = persistent_tree.update(v1, 1, 20);  // 将 arr[1] 从 2 改为 20
    cout << "版本 " << v2 << " - 更新 arr[1] = 20" << endl;
    cout << "版本 " << v2 << " - 区间 [0, 4] 的和: " << persistent_tree.query(v2, 0, 4) << endl;  // 40
    
    // 查询历史版本
    cout << "\n历史版本查询:" << endl;
    cout << "版本 0 - 区间 [0, 4] 的和: " << persistent_tree.query(0, 0, 4) << endl;   // 15
    cout << "版本 1 - 区间 [0, 4] 的和: " << persistent_tree.query(v1, 0, 4) << endl; // 22
    cout << "版本 2 - 区间 [0, 4] 的和: " << persistent_tree.query(v2, 0, 4) << endl; // 40
    
    // 合并版本
    int v3 = persistent_tree.merge_versions(0, v2);
    cout << "\n合并版本 0 和版本 " << v2 << " 得到版本 " << v3 << endl;
    cout << "版本 " << v3 << " - 区间 [0, 4] 的和: " << persistent_tree.query(v3, 0, 4) << endl;
    
    // 创建分支
    int v4 = persistent_tree.create_branch(0);
    int v5 = persistent_tree.update(v4, 0, 100);
    cout << "\n从版本 0 创建分支，更新 arr[0] = 100" << endl;
    cout << "版本 " << v5 << " - 区间 [0, 4] 的和: " << persistent_tree.query(v5, 0, 4) << endl;
    
    // 打印版本信息
    cout << "\n版本信息:" << endl;
    persistent_tree.print_all_versions();
    
    // 测试可持久化数组
    cout << "\n可持久化数组测试:" << endl;
    PersistentArray<int> persistent_array(arr);
    
    cout << "版本 0 - arr[2]: " << persistent_array.get(0, 2) << endl;  // 3
    
    int arr_v1 = persistent_array.set(0, 2, 100);
    cout << "版本 " << arr_v1 << " - arr[2]: " << persistent_array.get(arr_v1, 2) << endl;  // 100
    cout << "版本 0 - arr[2]: " << persistent_array.get(0, 2) << endl;  // 3 (历史版本不变)
    
    // 测试可持久化栈
    cout << "\n可持久化栈测试:" << endl;
    PersistentStack<int> persistent_stack;
    
    int stack_v1 = persistent_stack.push(0, 10);
    int stack_v2 = persistent_stack.push(stack_v1, 20);
    int stack_v3 = persistent_stack.push(stack_v2, 30);
    
    cout << "版本 " << stack_v3 << " - 栈顶: " << persistent_stack.top(stack_v3) << endl;  // 30
    cout << "版本 " << stack_v3 << " - 栈大小: " << persistent_stack.size(stack_v3) << endl;  // 3
    
    int stack_v4 = persistent_stack.pop(stack_v3);
    cout << "版本 " << stack_v4 << " - 栈顶: " << persistent_stack.top(stack_v4) << endl;  // 20
    cout << "版本 " << stack_v4 << " - 栈大小: " << persistent_stack.size(stack_v4) << endl;  // 2
    
    // 历史版本仍然可用
    cout << "版本 " << stack_v3 << " - 栈顶: " << persistent_stack.top(stack_v3) << endl;  // 30
    
    cout << "可持久化线段树测试完成!\n" << endl;
}

// 性能测试
void performance_test() {
    cout << "=== 性能测试 ===" << endl;
    
    const int n = 100000;
    const int queries = 10000;
    
    // 生成随机数据
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 1000);
    uniform_int_distribution<> idx_dis(0, n - 1);
    
    vector<int> arr(n);
    for (int i = 0; i < n; ++i) {
        arr[i] = dis(gen);
    }
    
    // 测试基础线段树性能
    auto start = high_resolution_clock::now();
    auto sum_tree = build_sum_segment_tree(arr);
    auto build_time = high_resolution_clock::now();
    
    long long total = 0;
    for (int i = 0; i < queries; ++i) {
        int l = idx_dis(gen);
        int r = idx_dis(gen);
        if (l > r) swap(l, r);
        total += sum_tree.query(l, r);
    }
    auto query_time = high_resolution_clock::now();
    
    for (int i = 0; i < queries / 10; ++i) {
        int idx = idx_dis(gen);
        int val = dis(gen);
        sum_tree.update(idx, val);
    }
    auto update_time = high_resolution_clock::now();
    
    cout << "基础线段树 (n=" << n << ", queries=" << queries << "):" << endl;
    cout << "  构建时间: " << duration_cast<milliseconds>(build_time - start).count() << " ms" << endl;
    cout << "  查询时间: " << duration_cast<milliseconds>(query_time - build_time).count() << " ms" << endl;
    cout << "  更新时间: " << duration_cast<milliseconds>(update_time - query_time).count() << " ms" << endl;
    cout << "  查询结果总和: " << total << endl;
    
    // 测试懒惰传播线段树性能
    start = high_resolution_clock::now();
    auto lazy_tree = build_range_add_sum_tree(arr);
    build_time = high_resolution_clock::now();
    
    total = 0;
    for (int i = 0; i < queries; ++i) {
        int l = idx_dis(gen);
        int r = idx_dis(gen);
        if (l > r) swap(l, r);
        total += lazy_tree.query(l, r);
    }
    query_time = high_resolution_clock::now();
    
    for (int i = 0; i < queries / 10; ++i) {
        int l = idx_dis(gen);
        int r = idx_dis(gen);
        if (l > r) swap(l, r);
        int val = dis(gen);
        lazy_tree.update_range(l, r, val);
    }
    update_time = high_resolution_clock::now();
    
    cout << "\n懒惰传播线段树 (n=" << n << ", queries=" << queries << "):" << endl;
    cout << "  构建时间: " << duration_cast<milliseconds>(build_time - start).count() << " ms" << endl;
    cout << "  查询时间: " << duration_cast<milliseconds>(query_time - build_time).count() << " ms" << endl;
    cout << "  区间更新时间: " << duration_cast<milliseconds>(update_time - query_time).count() << " ms" << endl;
    cout << "  查询结果总和: " << total << endl;
    
    // 测试动态线段树性能
    start = high_resolution_clock::now();
    auto dynamic_tree = build_dynamic_sum_tree<long long>(0, 1000000);
    build_time = high_resolution_clock::now();
    
    // 稀疏更新
    uniform_int_distribution<> sparse_dis(0, 1000000);
    for (int i = 0; i < queries / 10; ++i) {
        int idx = sparse_dis(gen);
        int val = dis(gen);
        dynamic_tree.update(idx, val);
    }
    auto sparse_update_time = high_resolution_clock::now();
    
    total = 0;
    for (int i = 0; i < queries; ++i) {
        int l = sparse_dis(gen);
        int r = sparse_dis(gen);
        if (l > r) swap(l, r);
        total += dynamic_tree.query(l, r);
    }
    query_time = high_resolution_clock::now();
    
    cout << "\n动态线段树 (range=[0,1000000], queries=" << queries << "):" << endl;
    cout << "  构建时间: " << duration_cast<milliseconds>(build_time - start).count() << " ms" << endl;
    cout << "  稀疏更新时间: " << duration_cast<milliseconds>(sparse_update_time - build_time).count() << " ms" << endl;
    cout << "  查询时间: " << duration_cast<milliseconds>(query_time - sparse_update_time).count() << " ms" << endl;
    cout << "  节点数量: " << dynamic_tree.node_count() << endl;
    cout << "  查询结果总和: " << total << endl;
    
    cout << "性能测试完成!\n" << endl;
}

// 应用示例：区间最值查询
void range_min_max_example() {
    cout << "=== 应用示例：区间最值查询 ===" << endl;
    
    vector<int> heights = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    cout << "建筑高度: ";
    for (int h : heights) cout << h << " ";
    cout << endl;
    
    auto max_tree = build_max_segment_tree(heights);
    auto min_tree = build_min_segment_tree(heights);
    
    // 查询不同区间的最值
    vector<pair<int, int>> ranges = {{0, 9}, {2, 6}, {1, 3}, {5, 8}};
    
    for (auto [l, r] : ranges) {
        int max_height = max_tree.query(l, r);
        int min_height = min_tree.query(l, r);
        cout << "区间 [" << l << ", " << r << "] - 最高: " << max_height 
             << ", 最低: " << min_height 
             << ", 高度差: " << max_height - min_height << endl;
    }
    
    cout << "区间最值查询示例完成!\n" << endl;
}

// 应用示例：股票价格分析
void stock_price_example() {
    cout << "=== 应用示例：股票价格分析 ===" << endl;
    
    vector<int> prices = {100, 105, 98, 110, 95, 120, 115, 108, 125, 130};
    cout << "股票价格: ";
    for (int p : prices) cout << p << " ";
    cout << endl;
    
    auto max_tree = build_max_segment_tree(prices);
    auto min_tree = build_min_segment_tree(prices);
    
    // 分析不同时间段的价格波动
    vector<pair<int, int>> periods = {{0, 4}, {3, 7}, {5, 9}, {0, 9}};
    
    for (auto [start, end] : periods) {
        int max_price = max_tree.query(start, end);
        int min_price = min_tree.query(start, end);
        double volatility = (double)(max_price - min_price) / min_price * 100;
        
        cout << "时段 [" << start << ", " << end << "] - "
             << "最高价: " << max_price 
             << ", 最低价: " << min_price 
             << ", 波动率: " << fixed << setprecision(2) << volatility << "%" << endl;
    }
    
    cout << "股票价格分析示例完成!\n" << endl;
}

// 应用示例：版本控制系统
void version_control_example() {
    cout << "=== 应用示例：版本控制系统 ===" << endl;
    
    // 模拟文档的不同版本
    vector<int> document = {1, 2, 3, 4, 5};  // 初始文档内容
    PersistentArray<int> version_control(document);
    VersionManager<int> manager(nullptr);  // 简化版本管理
    
    cout << "初始文档: ";
    for (int x : document) cout << x << " ";
    cout << endl;
    
    // 版本 1: 修改第 2 个元素
    int v1 = version_control.set(0, 1, 20);
    cout << "版本 1 - 修改位置 1 为 20" << endl;
    auto v1_doc = version_control.get_array(v1);
    cout << "版本 1 文档: ";
    for (int x : v1_doc) cout << x << " ";
    cout << endl;
    
    // 版本 2: 修改第 4 个元素
    int v2 = version_control.set(v1, 3, 40);
    cout << "版本 2 - 修改位置 3 为 40" << endl;
    auto v2_doc = version_control.get_array(v2);
    cout << "版本 2 文档: ";
    for (int x : v2_doc) cout << x << " ";
    cout << endl;
    
    // 从版本 0 创建分支
    int branch = version_control.create_branch(0);
    int v3 = version_control.set(branch, 0, 100);
    cout << "分支版本 - 修改位置 0 为 100" << endl;
    auto v3_doc = version_control.get_array(v3);
    cout << "分支文档: ";
    for (int x : v3_doc) cout << x << " ";
    cout << endl;
    
    // 查看历史版本
    cout << "\n历史版本对比:" << endl;
    cout << "版本 0: ";
    auto v0_doc = version_control.get_array(0);
    for (int x : v0_doc) cout << x << " ";
    cout << endl;
    
    cout << "版本 1: ";
    for (int x : v1_doc) cout << x << " ";
    cout << endl;
    
    cout << "版本 2: ";
    for (int x : v2_doc) cout << x << " ";
    cout << endl;
    
    cout << "分支版本: ";
    for (int x : v3_doc) cout << x << " ";
    cout << endl;
    
    cout << "版本控制系统示例完成!\n" << endl;
}

int main() {
    try {
        test_basic_segment_tree();
        test_lazy_segment_tree();
        test_dynamic_segment_tree();
        test_persistent_segment_tree();
        
        performance_test();
        
        range_min_max_example();
        stock_price_example();
        version_control_example();
        
        cout << "所有测试完成!" << endl;
        
    } catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}