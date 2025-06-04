#include "btree.h"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cassert>

using namespace std;
using namespace chrono;

// 性能测试函数
void performance_test() {
    cout << "\n=== B-Tree Performance Test ===\n";
    
    const int test_size = 10000;
    BTree<int, 5> btree;
    
    // 插入性能测试
    auto start = high_resolution_clock::now();
    for (int i = 1; i <= test_size; ++i) {
        btree.insert(i);
    }
    auto end = high_resolution_clock::now();
    auto insert_time = duration_cast<microseconds>(end - start);
    
    cout << "Inserted " << test_size << " elements in " << insert_time.count() << " microseconds\n";
    cout << "Tree height: " << btree.height() << "\n";
    cout << "Node count: " << btree.node_count() << "\n";
    
    // 搜索性能测试
    start = high_resolution_clock::now();
    for (int i = 1; i <= test_size; ++i) {
        assert(btree.search(i));
    }
    end = high_resolution_clock::now();
    auto search_time = duration_cast<microseconds>(end - start);
    
    cout << "Searched " << test_size << " elements in " << search_time.count() << " microseconds\n";
    
    // 删除性能测试
    start = high_resolution_clock::now();
    for (int i = 1; i <= test_size / 2; ++i) {
        btree.remove(i * 2);
    }
    end = high_resolution_clock::now();
    auto remove_time = duration_cast<microseconds>(end - start);
    
    cout << "Removed " << test_size / 2 << " elements in " << remove_time.count() << " microseconds\n";
    cout << "Final tree height: " << btree.height() << "\n";
    cout << "Final node count: " << btree.node_count() << "\n";
}

// 基本操作测试
void test_basic_operations() {
    cout << "\n=== Basic Operations Test ===\n";
    
    BTree<int, 3> btree;
    
    // 测试插入
    cout << "Inserting elements: ";
    vector<int> elements = {10, 20, 5, 6, 12, 30, 7, 17};
    for (int elem : elements) {
        cout << elem << " ";
        btree.insert(elem);
    }
    cout << "\n";
    
    // 测试搜索
    cout << "\nSearch results:\n";
    for (int elem : {5, 10, 15, 20, 25}) {
        cout << "Search " << elem << ": " << (btree.search(elem) ? "Found" : "Not found") << "\n";
    }
    
    // 测试遍历
    cout << "\nInorder traversal: ";
    btree.inorder_traverse([](const int& key) {
        cout << key << " ";
    });
    cout << "\n";
    
    cout << "Level order traversal: ";
    btree.level_order_traverse([](const int& key) {
        cout << key << " ";
    });
    cout << "\n";
    
    // 测试范围查询
    cout << "\nRange search [7, 17]: ";
    auto range_result = btree.range_search(7, 17);
    for (int key : range_result) {
        cout << key << " ";
    }
    cout << "\n";
    
    // 测试最值
    cout << "\nMin element: " << btree.get_min() << "\n";
    cout << "Max element: " << btree.get_max() << "\n";
    
    // 测试删除
    cout << "\nRemoving elements: ";
    vector<int> to_remove = {6, 12, 20};
    for (int elem : to_remove) {
        cout << elem << " ";
        btree.remove(elem);
    }
    cout << "\n";
    
    cout << "After removal - Inorder traversal: ";
    btree.inorder_traverse([](const int& key) {
        cout << key << " ";
    });
    cout << "\n";
    
    // 测试树的统计信息
    auto stats = btree.get_statistics();
    cout << "\nTree statistics:\n";
    cout << "Size: " << stats.size << "\n";
    cout << "Height: " << stats.height << "\n";
    cout << "Node count: " << stats.node_count << "\n";
    cout << "Average keys per node: " << stats.avg_keys_per_node << "\n";
    cout << "Fill factor: " << stats.fill_factor << "\n";
}

// 不同度数的B树测试
void test_different_degrees() {
    cout << "\n=== Different Degrees Test ===\n";
    
    vector<int> test_data;
    for (int i = 1; i <= 100; ++i) {
        test_data.push_back(i);
    }
    
    // 随机打乱数据
    random_device rd;
    mt19937 g(rd());
    shuffle(test_data.begin(), test_data.end(), g);
    
    // 测试不同度数的B树
    vector<int> degrees = {3, 5, 10};
    
    for (int degree : degrees) {
        cout << "\nTesting B-Tree with minimum degree " << degree << ":\n";
        
        if (degree == 3) {
            BTree<int, 3> btree;
            for (int data : test_data) {
                btree.insert(data);
            }
            auto stats = btree.get_statistics();
            cout << "Height: " << stats.height << ", Nodes: " << stats.node_count << "\n";
        } else if (degree == 5) {
            BTree<int, 5> btree;
            for (int data : test_data) {
                btree.insert(data);
            }
            auto stats = btree.get_statistics();
            cout << "Height: " << stats.height << ", Nodes: " << stats.node_count << "\n";
        } else if (degree == 10) {
            BTree<int, 10> btree;
            for (int data : test_data) {
                btree.insert(data);
            }
            auto stats = btree.get_statistics();
            cout << "Height: " << stats.height << ", Nodes: " << stats.node_count << "\n";
        }
    }
}

// 字符串B树测试
void test_string_btree() {
    cout << "\n=== String B-Tree Test ===\n";
    
    BTree<string, 3> string_btree;
    
    vector<string> words = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon"
    };
    
    cout << "Inserting words: ";
    for (const string& word : words) {
        cout << word << " ";
        string_btree.insert(word);
    }
    cout << "\n";
    
    cout << "\nSorted words (inorder traversal): ";
    string_btree.inorder_traverse([](const string& word) {
        cout << word << " ";
    });
    cout << "\n";
    
    cout << "\nRange search [\"cherry\", \"kiwi\"]: ";
    auto range_result = string_btree.range_search("cherry", "kiwi");
    for (const string& word : range_result) {
        cout << word << " ";
    }
    cout << "\n";
}

// B树排序测试
void test_btree_sort() {
    cout << "\n=== B-Tree Sort Test ===\n";
    
    vector<int> data = {64, 34, 25, 12, 22, 11, 90, 88, 76, 50, 42};
    
    cout << "Original array: ";
    for (int x : data) {
        cout << x << " ";
    }
    cout << "\n";
    
    btree_sort<int, 3>(data);
    
    cout << "Sorted array: ";
    for (int x : data) {
        cout << x << " ";
    }
    cout << "\n";
}

// 第K小元素测试
void test_kth_smallest() {
    cout << "\n=== Kth Smallest Element Test ===\n";
    
    vector<int> data = {20, 10, 30, 5, 15, 25, 35, 1, 8, 12, 18, 22, 28, 32, 40};
    auto btree = build_btree_from_array<int, 3>(data);
    
    cout << "Array: ";
    for (int x : data) {
        cout << x << " ";
    }
    cout << "\n";
    
    for (int k = 1; k <= min(10, (int)data.size()); ++k) {
        int kth = find_kth_smallest(btree, k);
        cout << k << "th smallest element: " << kth << "\n";
    }
}

// 压力测试
void stress_test() {
    cout << "\n=== Stress Test ===\n";
    
    BTree<int, 5> btree;
    const int stress_size = 50000;
    
    cout << "Inserting " << stress_size << " random elements...\n";
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, stress_size * 2);
    
    vector<int> inserted;
    for (int i = 0; i < stress_size; ++i) {
        int value = dis(gen);
        if (!btree.search(value)) {
            btree.insert(value);
            inserted.push_back(value);
        }
    }
    
    cout << "Actually inserted: " << inserted.size() << " unique elements\n";
    cout << "Tree height: " << btree.height() << "\n";
    cout << "Node count: " << btree.node_count() << "\n";
    
    // 验证所有插入的元素都能找到
    cout << "Verifying all inserted elements...\n";
    bool all_found = true;
    for (int value : inserted) {
        if (!btree.search(value)) {
            cout << "ERROR: Element " << value << " not found!\n";
            all_found = false;
        }
    }
    
    if (all_found) {
        cout << "All elements verified successfully!\n";
    }
    
    // 删除一半元素
    cout << "Removing half of the elements...\n";
    for (size_t i = 0; i < inserted.size() / 2; ++i) {
        btree.remove(inserted[i]);
    }
    
    cout << "After removal - Tree height: " << btree.height() << "\n";
    cout << "After removal - Node count: " << btree.node_count() << "\n";
    
    // 验证删除的元素确实不存在
    cout << "Verifying removed elements...\n";
    bool removal_correct = true;
    for (size_t i = 0; i < inserted.size() / 2; ++i) {
        if (btree.search(inserted[i])) {
            cout << "ERROR: Element " << inserted[i] << " still exists after removal!\n";
            removal_correct = false;
        }
    }
    
    if (removal_correct) {
        cout << "Removal verification successful!\n";
    }
}

int main() {
    cout << "B-Tree Implementation Test Suite\n";
    cout << "================================\n";
    
    try {
        test_basic_operations();
        test_different_degrees();
        test_string_btree();
        test_btree_sort();
        test_kth_smallest();
        performance_test();
        stress_test();
        
        cout << "\n=== All Tests Completed Successfully! ===\n";
    }
    catch (const exception& e) {
        cout << "\nError during testing: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}