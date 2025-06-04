#include "heap.h"
#include "advanced_heap.h"
#include "mergeable_heap.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;
using namespace chrono;

// 性能测试辅助函数
template<typename Func>
double measure_time(Func&& func) {
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration<double, milli>(end - start).count();
}

// 生成随机数据
vector<int> generate_random_data(size_t size, int min_val = 1, int max_val = 1000) {
    vector<int> data;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(min_val, max_val);
    
    for (size_t i = 0; i < size; ++i) {
        data.push_back(dis(gen));
    }
    
    return data;
}

void test_binary_heap() {
    cout << "\n=== 二叉堆测试 ===\n";
    
    // 测试最大堆
    cout << "\n--- 最大堆测试 ---\n";
    MaxHeap<int> max_heap;
    
    vector<int> data = {4, 10, 3, 5, 1, 15, 9, 6, 7, 12};
    cout << "插入数据: ";
    for (int val : data) {
        cout << val << " ";
        max_heap.insert(val);
    }
    cout << "\n";
    
    cout << "堆结构:\n";
    max_heap.print_tree();
    
    cout << "\n堆顶元素: " << max_heap.top() << "\n";
    cout << "堆大小: " << max_heap.size() << "\n";
    cout << "是否为有效堆: " << (max_heap.is_valid_heap() ? "是" : "否") << "\n";
    
    cout << "\n依次提取最大元素: ";
    while (!max_heap.empty()) {
        cout << max_heap.extract_top() << " ";
    }
    cout << "\n";
    
    // 测试最小堆
    cout << "\n--- 最小堆测试 ---\n";
    MinHeap<int> min_heap;
    
    // 从数组构建堆
    vector<int> build_data = {20, 15, 8, 10, 5, 7, 6, 2, 9, 1};
    min_heap.build_heap(build_data);
    
    cout << "从数组构建的最小堆:\n";
    min_heap.print_tree();
    
    cout << "\n查找元素 7: " << (min_heap.find(7) != -1 ? "找到" : "未找到") << "\n";
    cout << "查找元素 100: " << (min_heap.find(100) != -1 ? "找到" : "未找到") << "\n";
    
    // 更新元素
    cout << "\n将元素 20 更新为 3\n";
    min_heap.update(0, 3);  // 假设20在索引0
    min_heap.print_tree();
    
    // 删除元素
    cout << "\n删除索引 2 的元素\n";
    min_heap.erase(2);
    min_heap.print_tree();
}

void test_heap_algorithms() {
    cout << "\n=== 堆算法测试 ===\n";
    
    // 堆排序测试
    cout << "\n--- 堆排序测试 ---\n";
    vector<int> sort_data = {64, 34, 25, 12, 22, 11, 90};
    cout << "原始数组: ";
    for (int val : sort_data) {
        cout << val << " ";
    }
    cout << "\n";
    
    heap_sort(sort_data);
    cout << "堆排序后: ";
    for (int val : sort_data) {
        cout << val << " ";
    }
    cout << "\n";
    
    // 查找第k大元素
    cout << "\n--- 查找第k大元素 ---\n";
    vector<int> kth_data = {3, 2, 1, 5, 6, 4};
    cout << "数组: ";
    for (int val : kth_data) {
        cout << val << " ";
    }
    cout << "\n";
    
    cout << "第2大元素: " << find_kth_largest(kth_data, 2) << "\n";
    cout << "第4小元素: " << find_kth_smallest(kth_data, 4) << "\n";
    
    // 合并k个有序数组
    cout << "\n--- 合并k个有序数组 ---\n";
    vector<vector<int>> arrays = {
        {1, 4, 5},
        {1, 3, 4},
        {2, 6}
    };
    
    cout << "输入数组:\n";
    for (const auto& arr : arrays) {
        cout << "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            cout << arr[i];
            if (i < arr.size() - 1) cout << ", ";
        }
        cout << "]\n";
    }
    
    vector<int> merged = merge_k_sorted_arrays(arrays);
    cout << "合并结果: ";
    for (int val : merged) {
        cout << val << " ";
    }
    cout << "\n";
    
    // 数据流中位数
    cout << "\n--- 数据流中位数 ---\n";
    MedianFinder median_finder;
    vector<int> stream = {5, 15, 1, 3, 8, 7, 9, 2, 6};
    
    cout << "数据流: ";
    for (int val : stream) {
        cout << val << " ";
        median_finder.add_num(val);
        cout << "(中位数: " << median_finder.find_median() << ") ";
    }
    cout << "\n";
    
    // 滑动窗口最大值
    cout << "\n--- 滑动窗口最大值 ---\n";
    vector<int> window_data = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    
    cout << "数组: ";
    for (int val : window_data) {
        cout << val << " ";
    }
    cout << "\n窗口大小: " << k << "\n";
    
    vector<int> max_values = sliding_window_maximum(window_data, k);
    cout << "滑动窗口最大值: ";
    for (int val : max_values) {
        cout << val << " ";
    }
    cout << "\n";
}

void test_binomial_heap() {
    cout << "\n=== 二项堆测试 ===\n";
    
    // 测试最小二项堆
    cout << "\n--- 最小二项堆测试 ---\n";
    MinBinomialHeap<int> bin_heap;
    
    vector<int> data = {10, 20, 30, 40, 50, 60, 70, 80};
    cout << "插入数据: ";
    for (int val : data) {
        cout << val << " ";
        bin_heap.insert(val);
    }
    cout << "\n";
    
    cout << "\n堆结构:\n";
    bin_heap.print_heap();
    
    cout << "\n堆顶元素: " << bin_heap.top() << "\n";
    cout << "堆大小: " << bin_heap.size() << "\n";
    cout << "是否为有效堆: " << (bin_heap.is_valid_heap() ? "是" : "否") << "\n";
    
    // 测试合并操作
    cout << "\n--- 二项堆合并测试 ---\n";
    MinBinomialHeap<int> heap1, heap2;
    
    vector<int> data1 = {1, 3, 5, 7};
    vector<int> data2 = {2, 4, 6, 8};
    
    for (int val : data1) {
        heap1.insert(val);
    }
    
    for (int val : data2) {
        heap2.insert(val);
    }
    
    cout << "堆1: ";
    for (int val : data1) {
        cout << val << " ";
    }
    cout << "\n";
    
    cout << "堆2: ";
    for (int val : data2) {
        cout << val << " ";
    }
    cout << "\n";
    
    heap1.merge(heap2);
    cout << "\n合并后的堆:\n";
    heap1.print_heap();
    
    cout << "\n依次提取最小元素: ";
    while (!heap1.empty()) {
        cout << heap1.extract_min() << " ";
    }
    cout << "\n";
    
    // 测试decrease_key操作
    cout << "\n--- decrease_key测试 ---\n";
    MinBinomialHeap<int> dk_heap;
    vector<int> dk_data = {15, 25, 35, 45};
    
    for (int val : dk_data) {
        dk_heap.insert(val);
    }
    
    cout << "初始堆:\n";
    dk_heap.print_heap();
    
    // 注意：这里需要节点指针，实际使用中需要保存insert返回的节点
    cout << "\n执行decrease_key操作...\n";
    // dk_heap.decrease_key(node, 5);  // 需要节点指针
    
    // 测试二项堆排序
    cout << "\n--- 二项堆排序测试 ---\n";
    vector<int> sort_data = {9, 5, 2, 7, 1, 8, 3};
    cout << "原始数组: ";
    for (int val : sort_data) {
        cout << val << " ";
    }
    cout << "\n";
    
    binomial_heap_sort(sort_data);
    cout << "排序后: ";
    for (int val : sort_data) {
        cout << val << " ";
    }
    cout << "\n";
}

void test_fibonacci_heap() {
    cout << "\n=== 斐波那契堆测试 ===\n";
    
    // 测试最小斐波那契堆
    cout << "\n--- 最小斐波那契堆测试 ---\n";
    MinFibonacciHeap<int> fib_heap;
    
    vector<int> data = {23, 7, 3, 17, 24, 18, 52, 38, 39, 41};
    vector<typename MinFibonacciHeap<int>::NodePtr> nodes;
    
    cout << "插入数据: ";
    for (int val : data) {
        cout << val << " ";
        nodes.push_back(fib_heap.insert(val));
    }
    cout << "\n";
    
    cout << "\n堆结构:\n";
    fib_heap.print_heap();
    
    cout << "\n堆顶元素: " << fib_heap.top() << "\n";
    cout << "堆大小: " << fib_heap.size() << "\n";
    cout << "是否为有效堆: " << (fib_heap.is_valid_heap() ? "是" : "否") << "\n";
    
    // 测试decrease_key操作
    cout << "\n--- decrease_key测试 ---\n";
    cout << "将值 24 减少到 1\n";
    // 找到值为24的节点（在实际应用中应该保存节点指针）
    for (size_t i = 0; i < data.size(); ++i) {
        if (data[i] == 24) {
            fib_heap.decrease_key(nodes[i], 1);
            break;
        }
    }
    
    cout << "decrease_key后的堆结构:\n";
    fib_heap.print_heap();
    cout << "新的堆顶: " << fib_heap.top() << "\n";
    
    // 测试extract_min
    cout << "\n提取几个最小元素: ";
    for (int i = 0; i < 3; ++i) {
        cout << fib_heap.extract_min() << " ";
    }
    cout << "\n";
    
    cout << "\n提取后的堆结构:\n";
    fib_heap.print_heap();
    
    // 测试合并操作
    cout << "\n--- 斐波那契堆合并测试 ---\n";
    MinFibonacciHeap<int> heap1, heap2;
    
    vector<int> data1 = {10, 20, 30};
    vector<int> data2 = {15, 25, 35};
    
    for (int val : data1) {
        heap1.insert(val);
    }
    
    for (int val : data2) {
        heap2.insert(val);
    }
    
    cout << "堆1最小值: " << heap1.top() << "\n";
    cout << "堆2最小值: " << heap2.top() << "\n";
    
    heap1.merge(heap2);
    cout << "合并后最小值: " << heap1.top() << "\n";
    cout << "合并后大小: " << heap1.size() << "\n";
    
    // 测试delete_node操作
    cout << "\n--- delete_node测试 ---\n";
    MinFibonacciHeap<int> del_heap;
    vector<typename MinFibonacciHeap<int>::NodePtr> del_nodes;
    
    vector<int> del_data = {5, 10, 15, 20, 25};
    for (int val : del_data) {
        del_nodes.push_back(del_heap.insert(val));
    }
    
    cout << "删除前:\n";
    del_heap.print_heap();
    
    // 删除值为15的节点
    for (size_t i = 0; i < del_data.size(); ++i) {
        if (del_data[i] == 15) {
            del_heap.delete_node(del_nodes[i]);
            break;
        }
    }
    
    cout << "\n删除节点15后:\n";
    del_heap.print_heap();
    
    // 测试斐波那契堆排序
    cout << "\n--- 斐波那契堆排序测试 ---\n";
    vector<int> sort_data = {8, 3, 5, 1, 9, 2, 7};
    cout << "原始数组: ";
    for (int val : sort_data) {
        cout << val << " ";
    }
    cout << "\n";
    
    fibonacci_heap_sort(sort_data);
    cout << "排序后: ";
    for (int val : sort_data) {
        cout << val << " ";
    }
    cout << "\n";
}

void test_priority_queue_with_decrease_key() {
    cout << "\n=== 支持decrease_key的优先队列测试 ===\n";
    
    PriorityQueueWithDecreaseKey<int> pq;
    
    // 插入一些元素
    vector<int> data = {10, 20, 30, 40, 50};
    cout << "插入数据: ";
    for (int val : data) {
        cout << val << " ";
        pq.insert(val);
    }
    cout << "\n";
    
    cout << "当前最小值: " << pq.top() << "\n";
    
    // 测试decrease_key
    cout << "\n将40减少到5\n";
    pq.decrease_key(40, 5);
    cout << "新的最小值: " << pq.top() << "\n";
    
    // 测试delete_key
    cout << "\n删除键值20\n";
    pq.delete_key(20);
    
    cout << "\n依次提取所有元素: ";
    while (!pq.empty()) {
        cout << pq.extract_min() << " ";
    }
    cout << "\n";
}

void test_advanced_heaps() {
    cout << "\n=== 高级堆测试 ===\n";
    
    // 测试D叉堆
    cout << "\n--- D叉堆测试 ---\n";
    DaryHeap<int, 4> quad_heap;
    vector<int> data = {10, 20, 15, 30, 40, 50, 100, 25, 45};
    
    cout << "插入数据到4叉堆: ";
    for (int val : data) {
        cout << val << " ";
        quad_heap.insert(val);
    }
    cout << "\n";
    
    cout << "4叉堆提取最大值: ";
    while (!quad_heap.empty()) {
        cout << quad_heap.extract() << " ";
    }
    cout << "\n";
    
    // 测试索引堆
    cout << "\n--- 索引堆测试 ---\n";
    IndexedHeap<string> heap;
    vector<pair<string, int>> items = {
        {"Alice", 85}, {"Bob", 92}, {"Charlie", 78}, {"David", 96}, {"Eve", 88}
    };
    
    cout << "插入学生成绩:\n";
    for (auto& item : items) {
        int index = heap.insert(item.first, item.second);
        cout << item.first << ": " << item.second << " (索引: " << index << ")\n";
    }
    
    cout << "\n当前最高分: " << heap.top_key() << " (" << heap.top_value() << ")\n";
    
    // 修改Bob的成绩
    cout << "\n将Bob的成绩改为98:\n";
    heap.change_key(1, 98);
    cout << "当前最高分: " << heap.top_key() << " (" << heap.top_value() << ")\n";
    
    // 测试双端堆
    cout << "\n--- 双端堆测试 ---\n";
    DoubleEndedHeap<int> deap;
    vector<int> deap_data = {5, 2, 8, 1, 9, 3, 7, 4, 6};
    
    cout << "插入数据: ";
    for (int val : deap_data) {
        cout << val << " ";
        deap.insert(val);
    }
    cout << "\n";
    
    cout << "\n当前最小值: " << deap.get_min() << "\n";
    cout << "当前最大值: " << deap.get_max() << "\n";
    
    // 测试可撤销堆
    cout << "\n--- 可撤销堆测试 ---\n";
    UndoableHeap<int> undo_heap;
    vector<int> undo_data = {10, 5, 15, 3, 8, 12, 20};
    
    cout << "插入数据: ";
    for (int val : undo_data) {
        cout << val << " ";
        undo_heap.insert(val);
    }
    cout << "\n";
    
    cout << "当前最大值: " << undo_heap.top() << "\n";
    
    // 提取几个元素
    cout << "\n提取元素: ";
    for (int i = 0; i < 3; ++i) {
        cout << undo_heap.extract() << " ";
    }
    cout << "\n";
    
    cout << "当前最大值: " << undo_heap.top() << "\n";
    
    // 撤销操作
    cout << "\n撤销2次操作:\n";
    undo_heap.undo();
    cout << "撤销1次后最大值: " << undo_heap.top() << "\n";
    undo_heap.undo();
    cout << "撤销2次后最大值: " << undo_heap.top() << "\n";
    
    // 测试滑动窗口堆
    cout << "\n--- 滑动窗口堆测试 ---\n";
    vector<int> window_data = {1, 3, -1, -3, 5, 3, 6, 7};
    int window_size = 3;
    
    SlidingWindowHeap<int> window_heap(window_size);
    
    cout << "数组: ";
    for (int val : window_data) {
        cout << val << " ";
    }
    cout << "\n";
    cout << "窗口大小: " << window_size << "\n\n";
    
    cout << "滑动窗口最大值:\n";
    for (int i = 0; i < window_data.size(); ++i) {
        window_heap.add(window_data[i]);
        if (i >= window_size - 1) {
            cout << "窗口[" << (i - window_size + 1) << ", " << i << "]: 最大值 = " 
                 << window_heap.get_max() << "\n";
        }
    }
}

void test_mergeable_heaps() {
    cout << "\n=== 可合并堆测试 ===\n";
    
    // 测试左偏堆
    cout << "\n--- 左偏堆测试 ---\n";
    LeftistHeap<int> leftist1, leftist2;
    
    // 向第一个堆插入数据
    vector<int> data1 = {3, 10, 8, 21, 14, 17, 23};
    cout << "堆1插入: ";
    for (int val : data1) {
        cout << val << " ";
        leftist1.insert(val);
    }
    cout << "\n";
    
    // 向第二个堆插入数据
    vector<int> data2 = {6, 12, 7, 18, 24, 37, 18, 33};
    cout << "堆2插入: ";
    for (int val : data2) {
        cout << val << " ";
        leftist2.insert(val);
    }
    cout << "\n";
    
    cout << "\n合并前:\n";
    cout << "堆1最大值: " << leftist1.top() << ", 大小: " << leftist1.size() << "\n";
    cout << "堆2最大值: " << leftist2.top() << ", 大小: " << leftist2.size() << "\n";
    
    // 合并堆
    leftist1.merge_with(leftist2);
    
    cout << "\n合并后:\n";
    cout << "合并堆最大值: " << leftist1.top() << ", 大小: " << leftist1.size() << "\n";
    cout << "堆2大小: " << leftist2.size() << "\n";
    
    cout << "\n验证左偏性质: " << (leftist1.is_valid_leftist() ? "通过" : "失败") << "\n";
    
    // 测试斜堆
    cout << "\n--- 斜堆测试 ---\n";
    SkewHeap<int> skew1, skew2;
    
    // 向第一个堆插入数据
    vector<int> skew_data1 = {15, 10, 8, 5, 4, 2};
    cout << "堆1插入: ";
    for (int val : skew_data1) {
        cout << val << " ";
        skew1.insert(val);
    }
    cout << "\n";
    
    // 向第二个堆插入数据
    vector<int> skew_data2 = {16, 12, 7, 3, 1};
    cout << "堆2插入: ";
    for (int val : skew_data2) {
        cout << val << " ";
        skew2.insert(val);
    }
    cout << "\n";
    
    cout << "\n合并前:\n";
    cout << "堆1最大值: " << skew1.top() << ", 大小: " << skew1.size() << "\n";
    cout << "堆2最大值: " << skew2.top() << ", 大小: " << skew2.size() << "\n";
    
    // 合并堆
    skew1.merge_with(skew2);
    
    cout << "\n合并后:\n";
    cout << "合并堆最大值: " << skew1.top() << ", 大小: " << skew1.size() << "\n";
    
    // 测试二项堆
    cout << "\n--- 二项堆测试 ---\n";
    BinomialHeap<int> binomial1, binomial2;
    
    // 向第一个堆插入数据
    vector<int> bin_data1 = {12, 7, 25, 15, 28, 33, 41};
    cout << "堆1插入: ";
    for (int val : bin_data1) {
        cout << val << " ";
        binomial1.insert(val);
    }
    cout << "\n";
    
    // 向第二个堆插入数据
    vector<int> bin_data2 = {18, 35, 20, 42, 9, 31, 23, 6, 48, 11, 17, 39};
    cout << "堆2插入: ";
    for (int val : bin_data2) {
        cout << val << " ";
        binomial2.insert(val);
    }
    cout << "\n";
    
    cout << "\n合并前:\n";
    cout << "堆1最大值: " << binomial1.top() << ", 大小: " << binomial1.size() 
         << ", 根数量: " << binomial1.root_count() << "\n";
    cout << "堆2最大值: " << binomial2.top() << ", 大小: " << binomial2.size() 
         << ", 根数量: " << binomial2.root_count() << "\n";
    
    // 合并堆
    binomial1.merge_with(binomial2);
    
    cout << "\n合并后:\n";
    cout << "合并堆最大值: " << binomial1.top() << ", 大小: " << binomial1.size() 
         << ", 根数量: " << binomial1.root_count() << "\n";
    
    cout << "\n提取前5个最大元素: ";
    for (int i = 0; i < 5 && !binomial1.empty(); ++i) {
        cout << binomial1.extract() << " ";
    }
    cout << "\n";
    
    cout << "剩余元素数量: " << binomial1.size() << "\n";
}

void performance_comparison() {
    cout << "\n=== 堆性能比较 ===\n";
    
    const size_t test_size = 10000;
    vector<int> test_data = generate_random_data(test_size);
    
    cout << "测试数据大小: " << test_size << "\n";
    cout << "\n性能测试结果 (毫秒):\n";
    cout << "操作\t\t二叉堆\t\t二项堆\t\t斐波那契堆\n";
    cout << "-----------------------------------------------------------\n";
    
    // 插入操作性能测试
    double binary_insert_time = measure_time([&]() {
        MinHeap<int> heap;
        for (int val : test_data) {
            heap.insert(val);
        }
    });
    
    double binomial_insert_time = measure_time([&]() {
        MinBinomialHeap<int> heap;
        for (int val : test_data) {
            heap.insert(val);
        }
    });
    
    double fibonacci_insert_time = measure_time([&]() {
        MinFibonacciHeap<int> heap;
        for (int val : test_data) {
            heap.insert(val);
        }
    });
    
    cout << "插入\t\t" << binary_insert_time << "\t\t" 
         << binomial_insert_time << "\t\t" << fibonacci_insert_time << "\n";
    
    // 提取最小值性能测试
    MinHeap<int> binary_heap;
    MinBinomialHeap<int> binomial_heap;
    MinFibonacciHeap<int> fibonacci_heap;
    
    // 预先插入数据
    for (int val : test_data) {
        binary_heap.insert(val);
        binomial_heap.insert(val);
        fibonacci_heap.insert(val);
    }
    
    double binary_extract_time = measure_time([&]() {
        while (!binary_heap.empty()) {
            binary_heap.extract_top();
        }
    });
    
    double binomial_extract_time = measure_time([&]() {
        while (!binomial_heap.empty()) {
            binomial_heap.extract_min();
        }
    });
    
    double fibonacci_extract_time = measure_time([&]() {
        while (!fibonacci_heap.empty()) {
            fibonacci_heap.extract_min();
        }
    });
    
    cout << "提取最小值\t" << binary_extract_time << "\t\t" 
         << binomial_extract_time << "\t\t" << fibonacci_extract_time << "\n";
    
    // 合并操作性能测试（仅二项堆和斐波那契堆）
    MinBinomialHeap<int> bin_heap1, bin_heap2;
    MinFibonacciHeap<int> fib_heap1, fib_heap2;
    
    size_t half_size = test_size / 2;
    for (size_t i = 0; i < half_size; ++i) {
        bin_heap1.insert(test_data[i]);
        bin_heap2.insert(test_data[i + half_size]);
        fib_heap1.insert(test_data[i]);
        fib_heap2.insert(test_data[i + half_size]);
    }
    
    double binomial_merge_time = measure_time([&]() {
        bin_heap1.merge(bin_heap2);
    });
    
    double fibonacci_merge_time = measure_time([&]() {
        fib_heap1.merge(fib_heap2);
    });
    
    cout << "合并\t\t-\t\t" << binomial_merge_time 
         << "\t\t" << fibonacci_merge_time << "\n";
    
    cout << "\n注意: 斐波那契堆在decrease_key操作上有最佳性能 O(1)\n";
    cout << "      二项堆的合并操作性能优于二叉堆\n";
    cout << "      二叉堆在简单的插入/删除操作上通常最快\n";
}

int main() {
    cout << "C++ 堆数据结构教程示例\n";
    cout << "========================\n";
    
    try {
        // 基本堆操作测试
        test_binary_heap();
        
        // 堆算法测试
        test_heap_algorithms();
        
        // 二项堆测试
        test_binomial_heap();
        
        // 斐波那契堆测试
        test_fibonacci_heap();
        
        // 支持decrease_key的优先队列测试
        test_priority_queue_with_decrease_key();
        
        // 高级堆测试
        test_advanced_heaps();
        
        // 可合并堆测试
        test_mergeable_heaps();
        
        // 性能比较
        performance_comparison();
        
    } catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
    
    cout << "\n所有测试完成！\n";
    return 0;
}