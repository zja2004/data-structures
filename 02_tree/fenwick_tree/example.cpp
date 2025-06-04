#include "fenwick_tree.h"
#include "advanced_fenwick_tree.h"
#include "fenwick_tree_2d.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <future>
#include <cassert>

using namespace std;
using namespace chrono;

// 基础树状数组示例
void basic_fenwick_tree_example() {
    cout << "=== 基础树状数组示例 ===" << endl;
    
    // 创建树状数组
    vector<int> arr = {1, 3, 5, 7, 9, 11};
    auto tree = build_fenwick_tree(arr);
    
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    // 前缀和查询
    cout << "前缀和查询:" << endl;
    for (int i = 1; i <= 6; ++i) {
        cout << "prefix_sum(" << i << ") = " << tree.prefix_sum(i) << endl;
    }
    
    // 区间和查询
    cout << "\n区间和查询:" << endl;
    cout << "range_sum(2, 4) = " << tree.range_sum(2, 4) << endl;
    cout << "range_sum(1, 6) = " << tree.range_sum(1, 6) << endl;
    
    // 单点更新
    cout << "\n单点更新 update(3, 10):" << endl;
    tree.update(3, 10);
    cout << "更新后 range_sum(1, 6) = " << tree.range_sum(1, 6) << endl;
    
    // 单点查询
    cout << "\n单点查询:" << endl;
    for (int i = 1; i <= 6; ++i) {
        cout << "query(" << i << ") = " << tree.query(i) << endl;
    }
    
    // 二分查找
    cout << "\n二分查找:" << endl;
    cout << "lower_bound(15) = " << tree.lower_bound(15) << endl;
    cout << "kth_element(3) = " << tree.kth_element(3) << endl;
    
    cout << endl;
}

// 差分树状数组示例
void difference_fenwick_tree_example() {
    cout << "=== 差分树状数组示例 ===" << endl;
    
    vector<int> arr = {1, 2, 3, 4, 5};
    DifferenceFenwickTree<int> diff_tree(arr);
    
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    // 区间更新
    cout << "\n区间更新 range_update(2, 4, 10):" << endl;
    diff_tree.range_update(2, 4, 10);
    
    cout << "更新后的数组: ";
    for (int i = 1; i <= 5; ++i) {
        cout << diff_tree.query(i) << " ";
    }
    cout << endl;
    
    // 再次区间更新
    cout << "\n区间更新 range_update(1, 3, -5):" << endl;
    diff_tree.range_update(1, 3, -5);
    
    cout << "更新后的数组: ";
    for (int i = 1; i <= 5; ++i) {
        cout << diff_tree.query(i) << " ";
    }
    cout << endl << endl;
}

// 范围树状数组示例
void range_fenwick_tree_example() {
    cout << "=== 范围树状数组示例 ===" << endl;
    
    RangeFenwickTree<int> range_tree(6);
    
    // 初始化数组
    vector<int> arr = {1, 2, 3, 4, 5, 6};
    for (int i = 0; i < arr.size(); ++i) {
        range_tree.update(i + 1, arr[i]);
    }
    
    cout << "初始数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    cout << "初始区间和 range_sum(1, 6) = " << range_tree.range_sum(1, 6) << endl;
    
    // 区间更新
    cout << "\n区间更新 range_update(2, 4, 10):" << endl;
    range_tree.range_update(2, 4, 10);
    cout << "更新后区间和 range_sum(1, 6) = " << range_tree.range_sum(1, 6) << endl;
    cout << "区间和 range_sum(2, 4) = " << range_tree.range_sum(2, 4) << endl;
    
    // 单点查询
    cout << "\n单点查询:" << endl;
    for (int i = 1; i <= 6; ++i) {
        cout << "query(" << i << ") = " << range_tree.query(i) << endl;
    }
    
    cout << endl;
}

// 动态第k小元素示例
void dynamic_kth_element_example() {
    cout << "=== 动态第k小元素示例 ===" << endl;
    
    DynamicKthElement<int> kth_tree;
    
    vector<int> elements = {5, 2, 8, 1, 9, 3, 7};
    
    cout << "插入元素: ";
    for (int x : elements) {
        cout << x << " ";
        kth_tree.insert(x);
    }
    cout << endl;
    
    cout << "\n查询第k小元素:" << endl;
    for (int k = 1; k <= kth_tree.size(); ++k) {
        cout << "第" << k << "小元素: " << kth_tree.kth_element(k) << endl;
    }
    
    // 删除元素
    cout << "\n删除元素 5:" << endl;
    kth_tree.erase(5);
    
    cout << "删除后查询第k小元素:" << endl;
    for (int k = 1; k <= kth_tree.size(); ++k) {
        cout << "第" << k << "小元素: " << kth_tree.kth_element(k) << endl;
    }
    
    cout << endl;
}

// 逆序对计数示例
void inversion_count_example() {
    cout << "=== 逆序对计数示例 ===" << endl;
    
    vector<int> arr1 = {2, 3, 8, 6, 1};
    vector<int> arr2 = {1, 2, 3, 4, 5};
    vector<int> arr3 = {5, 4, 3, 2, 1};
    
    cout << "数组1: ";
    for (int x : arr1) cout << x << " ";
    cout << "逆序对数量: " << count_inversions(arr1) << endl;
    
    cout << "数组2: ";
    for (int x : arr2) cout << x << " ";
    cout << "逆序对数量: " << count_inversions(arr2) << endl;
    
    cout << "数组3: ";
    for (int x : arr3) cout << x << " ";
    cout << "逆序对数量: " << count_inversions(arr3) << endl;
    
    cout << endl;
}

// 可撤销树状数组示例
void undoable_fenwick_tree_example() {
    cout << "=== 可撤销树状数组示例 ===" << endl;
    
    vector<int> arr = {1, 2, 3, 4, 5};
    UndoableFenwickTree<int> undo_tree(arr);
    
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    cout << "初始总和: " << undo_tree.range_sum(1, 5) << endl;
    
    // 执行一些更新操作
    cout << "\n执行更新操作:" << endl;
    undo_tree.update(2, 10);
    cout << "update(2, 10), 总和: " << undo_tree.range_sum(1, 5) << endl;
    
    undo_tree.set(4, 20);
    cout << "set(4, 20), 总和: " << undo_tree.range_sum(1, 5) << endl;
    
    undo_tree.update(1, 5);
    cout << "update(1, 5), 总和: " << undo_tree.range_sum(1, 5) << endl;
    
    cout << "历史记录大小: " << undo_tree.history_size() << endl;
    
    // 撤销操作
    cout << "\n撤销操作:" << endl;
    undo_tree.undo();
    cout << "撤销1次, 总和: " << undo_tree.range_sum(1, 5) << endl;
    
    undo_tree.undo(2);
    cout << "撤销2次, 总和: " << undo_tree.range_sum(1, 5) << endl;
    
    cout << "历史记录大小: " << undo_tree.history_size() << endl;
    
    cout << endl;
}

// 可持久化树状数组示例
void persistent_fenwick_tree_example() {
    cout << "=== 可持久化树状数组示例 ===" << endl;
    
    vector<int> arr = {1, 2, 3, 4, 5};
    PersistentFenwickTree<int> persistent_tree(arr);
    
    cout << "原数组: ";
    for (int x : arr) cout << x << " ";
    cout << endl;
    
    int version0 = persistent_tree.version_count() - 1;
    cout << "版本" << version0 << "总和: " << persistent_tree.range_sum(version0, 1, 5) << endl;
    
    // 创建新版本
    cout << "\n创建新版本:" << endl;
    int version1 = persistent_tree.update(version0, 2, 10);
    cout << "版本" << version1 << " (update(2, 10)) 总和: " << persistent_tree.range_sum(version1, 1, 5) << endl;
    
    int version2 = persistent_tree.update(version1, 4, -2);
    cout << "版本" << version2 << " (update(4, -2)) 总和: " << persistent_tree.range_sum(version2, 1, 5) << endl;
    
    int version3 = persistent_tree.update(version0, 1, 100);  // 从版本0分支
    cout << "版本" << version3 << " (从版本0分支, update(1, 100)) 总和: " << persistent_tree.range_sum(version3, 1, 5) << endl;
    
    // 查询不同版本
    cout << "\n查询不同版本:" << endl;
    for (int v = 0; v < persistent_tree.version_count(); ++v) {
        cout << "版本" << v << ": ";
        for (int i = 1; i <= 5; ++i) {
            cout << persistent_tree.query(v, i) << " ";
        }
        cout << "(总和: " << persistent_tree.range_sum(v, 1, 5) << ")" << endl;
    }
    
    cout << endl;
}

// 线程安全树状数组示例
void thread_safe_fenwick_tree_example() {
    cout << "=== 线程安全树状数组示例 ===" << endl;
    
    vector<int> arr(1000, 1);
    ThreadSafeFenwickTree<int> safe_tree(arr);
    
    cout << "初始总和: " << safe_tree.range_sum(1, 1000) << endl;
    
    // 多线程更新
    vector<future<void>> futures;
    
    auto update_func = [&safe_tree](int start, int end, int delta) {
        for (int i = start; i <= end; ++i) {
            safe_tree.update(i, delta);
        }
    };
    
    cout << "\n启动多线程更新..." << endl;
    auto start_time = high_resolution_clock::now();
    
    // 启动4个线程
    futures.push_back(async(launch::async, update_func, 1, 250, 1));
    futures.push_back(async(launch::async, update_func, 251, 500, 2));
    futures.push_back(async(launch::async, update_func, 501, 750, 3));
    futures.push_back(async(launch::async, update_func, 751, 1000, 4));
    
    // 等待所有线程完成
    for (auto& f : futures) {
        f.wait();
    }
    
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time);
    
    cout << "多线程更新完成，耗时: " << duration.count() << "ms" << endl;
    cout << "更新后总和: " << safe_tree.range_sum(1, 1000) << endl;
    
    // 批量查询
    vector<pair<int, int>> ranges = {{1, 100}, {101, 200}, {201, 300}, {301, 400}};
    auto results = safe_tree.batch_query(ranges);
    
    cout << "\n批量查询结果:" << endl;
    for (int i = 0; i < ranges.size(); ++i) {
        cout << "range_sum(" << ranges[i].first << ", " << ranges[i].second << ") = " << results[i] << endl;
    }
    
    cout << endl;
}

// 压缩树状数组示例
void compressed_fenwick_tree_example() {
    cout << "=== 压缩树状数组示例 ===" << endl;
    
    // 稀疏索引
    vector<int> indices = {1, 100, 1000, 10000, 100000};
    CompressedFenwickTree<int> compressed_tree(indices);
    
    cout << "压缩前索引: ";
    for (int idx : indices) cout << idx << " ";
    cout << endl;
    
    cout << "压缩后坐标: ";
    auto coords = compressed_tree.get_coordinates();
    for (int coord : coords) cout << coord << " ";
    cout << endl;
    
    cout << "压缩后大小: " << compressed_tree.compressed_size() << endl;
    
    // 更新操作
    cout << "\n更新操作:" << endl;
    compressed_tree.update(1, 10);
    compressed_tree.update(1000, 20);
    compressed_tree.update(100000, 30);
    
    // 查询操作
    cout << "\n查询操作:" << endl;
    for (int idx : indices) {
        cout << "query(" << idx << ") = " << compressed_tree.query(idx) << endl;
    }
    
    cout << "\n前缀和查询:" << endl;
    cout << "prefix_sum(500) = " << compressed_tree.prefix_sum(500) << endl;
    cout << "prefix_sum(50000) = " << compressed_tree.prefix_sum(50000) << endl;
    cout << "prefix_sum(200000) = " << compressed_tree.prefix_sum(200000) << endl;
    
    cout << endl;
}

// 二维树状数组示例
void fenwick_tree_2d_example() {
    cout << "=== 二维树状数组示例 ===" << endl;
    
    // 创建3x3矩阵
    vector<vector<int>> matrix = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    
    FenwickTree2D<int> tree2d(matrix);
    
    cout << "原矩阵:" << endl;
    tree2d.print_matrix();
    
    // 矩形区域和查询
    cout << "矩形区域和查询:" << endl;
    cout << "range_sum(1,1,2,2) = " << tree2d.range_sum(1, 1, 2, 2) << endl;
    cout << "range_sum(2,2,3,3) = " << tree2d.range_sum(2, 2, 3, 3) << endl;
    cout << "range_sum(1,1,3,3) = " << tree2d.range_sum(1, 1, 3, 3) << endl;
    
    // 单点更新
    cout << "\n单点更新 update(2,2,10):" << endl;
    tree2d.update(2, 2, 10);
    
    cout << "更新后矩阵:" << endl;
    tree2d.print_matrix();
    
    cout << "更新后 range_sum(1,1,3,3) = " << tree2d.range_sum(1, 1, 3, 3) << endl;
    
    cout << endl;
}

// 二维差分树状数组示例
void difference_fenwick_tree_2d_example() {
    cout << "=== 二维差分树状数组示例 ===" << endl;
    
    vector<vector<int>> matrix = {
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1},
        {1, 1, 1, 1}
    };
    
    DifferenceFenwickTree2D<int> diff_tree2d(matrix);
    
    cout << "原矩阵:" << endl;
    diff_tree2d.print_matrix();
    
    // 矩形区域更新
    cout << "矩形区域更新 range_update(2,2,3,3,5):" << endl;
    diff_tree2d.range_update(2, 2, 3, 3, 5);
    
    cout << "更新后矩阵:" << endl;
    diff_tree2d.print_matrix();
    
    // 再次矩形区域更新
    cout << "矩形区域更新 range_update(1,1,2,2,-2):" << endl;
    diff_tree2d.range_update(1, 1, 2, 2, -2);
    
    cout << "更新后矩阵:" << endl;
    diff_tree2d.print_matrix();
    
    cout << endl;
}

// 性能测试
void performance_test() {
    cout << "=== 性能测试 ===" << endl;
    
    const int n = 100000;
    const int operations = 10000;
    
    // 创建随机数据
    vector<int> arr(n, 1);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, n);
    
    // 测试基础树状数组
    cout << "测试基础树状数组 (n=" << n << ", operations=" << operations << "):" << endl;
    
    MonitoredFenwickTree<int> monitored_tree(arr);
    
    auto start = high_resolution_clock::now();
    
    // 执行随机更新和查询
    for (int i = 0; i < operations; ++i) {
        int idx = dis(gen);
        if (i % 2 == 0) {
            monitored_tree.update(idx, 1);
        } else {
            monitored_tree.range_sum(1, idx);
        }
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    cout << "总耗时: " << duration.count() / 1000.0 << "ms" << endl;
    monitored_tree.print_performance();
    
    cout << endl;
}

// 应用示例：股票价格分析
void stock_price_analysis_example() {
    cout << "=== 应用示例：股票价格分析 ===" << endl;
    
    // 模拟股票价格数据
    vector<double> prices = {100.0, 102.5, 101.0, 105.0, 103.5, 107.0, 106.0, 108.5, 110.0, 109.0};
    
    FenwickTree<double> price_tree(prices.size());
    
    // 初始化价格数据
    for (int i = 0; i < prices.size(); ++i) {
        price_tree.update(i + 1, prices[i]);
    }
    
    cout << "股票价格数据: ";
    for (double price : prices) cout << fixed << setprecision(1) << price << " ";
    cout << endl;
    
    // 计算移动平均
    cout << "\n3日移动平均:" << endl;
    for (int i = 3; i <= prices.size(); ++i) {
        double sum = price_tree.range_sum(i - 2, i);
        double avg = sum / 3.0;
        cout << "第" << i << "天: " << fixed << setprecision(2) << avg << endl;
    }
    
    // 计算累计收益
    cout << "\n累计价格总和:" << endl;
    for (int i = 1; i <= prices.size(); ++i) {
        cout << "前" << i << "天总和: " << fixed << setprecision(1) << price_tree.prefix_sum(i) << endl;
    }
    
    cout << endl;
}

// 应用示例：区间统计
void range_statistics_example() {
    cout << "=== 应用示例：区间统计 ===" << endl;
    
    // 学生成绩数据
    vector<int> scores = {85, 92, 78, 96, 88, 91, 83, 89, 94, 87};
    
    FenwickTree<int> score_tree(scores);
    
    cout << "学生成绩: ";
    for (int score : scores) cout << score << " ";
    cout << endl;
    
    // 计算不同区间的平均分
    cout << "\n区间平均分:" << endl;
    
    struct Range {
        int start, end;
        string name;
    };
    
    vector<Range> ranges = {
        {1, 3, "前3名"},
        {4, 7, "中间4名"},
        {8, 10, "后3名"},
        {1, 10, "全体"}
    };
    
    for (const auto& range : ranges) {
        int sum = score_tree.range_sum(range.start, range.end);
        int count = range.end - range.start + 1;
        double avg = static_cast<double>(sum) / count;
        cout << range.name << " (" << range.start << "-" << range.end << "): "
             << "总分=" << sum << ", 平均分=" << fixed << setprecision(1) << avg << endl;
    }
    
    // 成绩更新
    cout << "\n成绩更新 (第5名学生 +5分):" << endl;
    score_tree.update(5, 5);
    
    int new_sum = score_tree.range_sum(1, 10);
    double new_avg = static_cast<double>(new_sum) / 10;
    cout << "更新后全体平均分: " << fixed << setprecision(1) << new_avg << endl;
    
    cout << endl;
}

int main() {
    cout << "树状数组 (Fenwick Tree) 完整示例" << endl;
    cout << "================================" << endl << endl;
    
    try {
        // 基础功能示例
        basic_fenwick_tree_example();
        difference_fenwick_tree_example();
        range_fenwick_tree_example();
        dynamic_kth_element_example();
        inversion_count_example();
        
        // 高级功能示例
        undoable_fenwick_tree_example();
        persistent_fenwick_tree_example();
        thread_safe_fenwick_tree_example();
        compressed_fenwick_tree_example();
        
        // 二维树状数组示例
        fenwick_tree_2d_example();
        difference_fenwick_tree_2d_example();
        
        // 性能测试
        performance_test();
        
        // 应用示例
        stock_price_analysis_example();
        range_statistics_example();
        
        cout << "所有示例执行完成！" << endl;
        
    } catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}