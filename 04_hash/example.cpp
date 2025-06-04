#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

#include "hash_functions.h"
#include "hash_table.h"
#include "collision_resolution.h"
#include "advanced_hash.h"

using namespace std;
using namespace hash_functions;
using namespace hash_table;
using namespace collision_resolution;
using namespace advanced_hash;

// 测试数据生成器
class TestDataGenerator {
public:
    static vector<pair<int, int>> generate_integer_pairs(size_t count, int min_val = 0, int max_val = 100000) {
        vector<pair<int, int>> data;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dis(min_val, max_val);
        
        for (size_t i = 0; i < count; ++i) {
            data.emplace_back(dis(gen), dis(gen));
        }
        
        return data;
    }
    
    static vector<pair<string, int>> generate_string_pairs(size_t count) {
        vector<pair<string, int>> data;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> length_dis(5, 20);
        uniform_int_distribution<int> char_dis(97, 122); // a-z
        uniform_int_distribution<int> value_dis(1, 1000);
        
        for (size_t i = 0; i < count; ++i) {
            string key;
            int length = length_dis(gen);
            for (int j = 0; j < length; ++j) {
                key += static_cast<char>(char_dis(gen));
            }
            data.emplace_back(key, value_dis(gen));
        }
        
        return data;
    }
    
    static vector<string> generate_strings(size_t count) {
        vector<string> data;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> length_dis(5, 15);
        uniform_int_distribution<int> char_dis(97, 122);
        
        for (size_t i = 0; i < count; ++i) {
            string str;
            int length = length_dis(gen);
            for (int j = 0; j < length; ++j) {
                str += static_cast<char>(char_dis(gen));
            }
            data.push_back(str);
        }
        
        return data;
    }
};

// 哈希函数测试
void test_hash_functions() {
    cout << "\n=== 哈希函数测试 ===\n";
    
    vector<string> test_strings = {
        "hello", "world", "hash", "function", "test",
        "algorithm", "data", "structure", "programming", "computer"
    };
    
    // 测试不同的字符串哈希函数
    auto djb2 = make_unique<string_hash::DJB2Hash>();
    auto fnv1a = make_unique<string_hash::FNV1aHash>();
    auto murmur3 = make_unique<string_hash::Murmur3Hash>();
    
    cout << "字符串哈希函数比较:\n";
    cout << setw(12) << "String" << setw(15) << "DJB2" << setw(15) << "FNV1a" << setw(15) << "Murmur3" << "\n";
    cout << string(60, '-') << "\n";
    
    for (const auto& str : test_strings) {
        cout << setw(12) << str 
             << setw(15) << djb2->hash(str)
             << setw(15) << fnv1a->hash(str)
             << setw(15) << murmur3->hash(str) << "\n";
    }
    
    // 测试哈希函数质量
    cout << "\n哈希函数质量测试:\n";
    vector<string> large_dataset = TestDataGenerator::generate_strings(10000);
    
    double djb2_chi = HashQualityTester::test_distribution(*djb2, large_dataset, 1000);
    double fnv1a_chi = HashQualityTester::test_distribution(*fnv1a, large_dataset, 1000);
    double murmur3_chi = HashQualityTester::test_distribution(*murmur3, large_dataset, 1000);
    
    cout << "DJB2 卡方值: " << djb2_chi << "\n";
    cout << "FNV1a 卡方值: " << fnv1a_chi << "\n";
    cout << "Murmur3 卡方值: " << murmur3_chi << "\n";
    
    // 测试雪崩效应
    vector<string> avalanche_test = {"test", "tesa", "best", "rest"};
    double djb2_avalanche = HashQualityTester::test_avalanche_effect(*djb2, avalanche_test);
    double fnv1a_avalanche = HashQualityTester::test_avalanche_effect(*fnv1a, avalanche_test);
    
    cout << "\n雪崩效应测试:\n";
    cout << "DJB2 雪崩比例: " << djb2_avalanche << "\n";
    cout << "FNV1a 雪崩比例: " << fnv1a_avalanche << "\n";
}

// 基本哈希表测试
void test_basic_hash_table() {
    cout << "\n=== 基本哈希表测试 ===\n";
    
    ChainingHashTable<int, string> hash_table;
    
    // 基本操作测试
    cout << "插入测试:\n";
    hash_table.insert(1, "apple");
    hash_table.insert(2, "banana");
    hash_table.insert(3, "orange");
    hash_table.insert(17, "grape"); // 可能产生冲突
    
    cout << "哈希表大小: " << hash_table.size() << "\n";
    cout << "负载因子: " << hash_table.load_factor() << "\n";
    
    // 查找测试
    cout << "\n查找测试:\n";
    string value;
    for (int key : {1, 2, 3, 17, 99}) {
        if (hash_table.find(key, value)) {
            cout << "键 " << key << " -> 值 " << value << "\n";
        } else {
            cout << "键 " << key << " 未找到\n";
        }
    }
    
    // 删除测试
    cout << "\n删除测试:\n";
    cout << "删除键 2: " << (hash_table.remove(2) ? "成功" : "失败") << "\n";
    cout << "删除键 99: " << (hash_table.remove(99) ? "成功" : "失败") << "\n";
    cout << "删除后大小: " << hash_table.size() << "\n";
    
    // 遍历测试
    cout << "\n遍历所有元素:\n";
    hash_table.for_each([](int key, const string& val) {
        cout << key << ": " << val << "\n";
    });
    
    // 统计信息
    cout << "\n";
    hash_table.print_statistics();
}

// 冲突解决方法比较
void test_collision_resolution() {
    cout << "\n=== 冲突解决方法比较 ===\n";
    
    const size_t test_size = 1000;
    auto test_data = TestDataGenerator::generate_integer_pairs(test_size);
    
    // 测试不同的哈希表实现
    ChainingHashTable<int, int> chaining_table;
    LinearProbingHashTable<int, int> linear_table;
    QuadraticProbingHashTable<int, int> quadratic_table;
    DoubleHashingTable<int, int> double_hash_table;
    RobinHoodHashTable<int, int> robin_hood_table;
    
    auto test_performance = [&](auto& table, const string& name) {
        auto start = chrono::high_resolution_clock::now();
        
        // 插入
        for (const auto& pair : test_data) {
            table.insert(pair.first, pair.second);
        }
        
        auto insert_end = chrono::high_resolution_clock::now();
        
        // 查找
        int found_count = 0;
        for (const auto& pair : test_data) {
            int value;
            if (table.find(pair.first, value)) {
                found_count++;
            }
        }
        
        auto find_end = chrono::high_resolution_clock::now();
        
        auto insert_time = chrono::duration_cast<chrono::microseconds>(insert_end - start);
        auto find_time = chrono::duration_cast<chrono::microseconds>(find_end - insert_end);
        
        cout << name << ":\n";
        cout << "  插入时间: " << insert_time.count() << " 微秒\n";
        cout << "  查找时间: " << find_time.count() << " 微秒\n";
        cout << "  负载因子: " << table.load_factor() << "\n";
        cout << "  找到元素: " << found_count << "/" << test_size << "\n";
        
        table.clear();
    };
    
    test_performance(chaining_table, "链地址法");
    test_performance(linear_table, "线性探测");
    test_performance(quadratic_table, "二次探测");
    test_performance(double_hash_table, "双重哈希");
    test_performance(robin_hood_table, "Robin Hood哈希");
}

// 高级哈希技术测试
void test_advanced_hash_techniques() {
    cout << "\n=== 高级哈希技术测试 ===\n";
    
    // 一致性哈希测试
    cout << "一致性哈希测试:\n";
    ConsistentHash consistent_hash(100);
    
    // 添加节点
    vector<string> nodes = {"server1", "server2", "server3", "server4"};
    for (const auto& node : nodes) {
        consistent_hash.add_node(node);
    }
    
    // 测试键分布
    map<string, int> distribution;
    for (int i = 0; i < 1000; ++i) {
        string key = "key" + to_string(i);
        string node = consistent_hash.get_node(key);
        distribution[node]++;
    }
    
    cout << "键分布:\n";
    for (const auto& pair : distribution) {
        cout << "  " << pair.first << ": " << pair.second << " 键\n";
    }
    
    // 测试节点移除后的影响
    cout << "\n移除 server2 后的分布:\n";
    consistent_hash.remove_node("server2");
    distribution.clear();
    
    for (int i = 0; i < 1000; ++i) {
        string key = "key" + to_string(i);
        string node = consistent_hash.get_node(key);
        distribution[node]++;
    }
    
    for (const auto& pair : distribution) {
        cout << "  " << pair.first << ": " << pair.second << " 键\n";
    }
}

// 布隆过滤器测试
void test_bloom_filter() {
    cout << "\n=== 布隆过滤器测试 ===\n";
    
    BloomFilter bloom_filter(10000, 0.01);
    
    // 插入测试数据
    vector<string> inserted_items;
    for (int i = 0; i < 5000; ++i) {
        string item = "item" + to_string(i);
        bloom_filter.insert(item);
        inserted_items.push_back(item);
    }
    
    // 测试已插入的元素
    int true_positives = 0;
    for (const auto& item : inserted_items) {
        if (bloom_filter.might_contain(item)) {
            true_positives++;
        }
    }
    
    // 测试未插入的元素
    int false_positives = 0;
    for (int i = 5000; i < 10000; ++i) {
        string item = "item" + to_string(i);
        if (bloom_filter.might_contain(item)) {
            false_positives++;
        }
    }
    
    cout << "布隆过滤器测试结果:\n";
    cout << "真正例: " << true_positives << "/" << inserted_items.size() << "\n";
    cout << "假正例: " << false_positives << "/5000\n";
    cout << "实际假正例率: " << (double)false_positives / 5000 << "\n";
    
    bloom_filter.print_statistics();
    
    // 计数布隆过滤器测试
    cout << "\n计数布隆过滤器测试:\n";
    CountingBloomFilter counting_filter(1000, 0.01);
    
    // 插入和删除测试
    counting_filter.insert("apple");
    counting_filter.insert("banana");
    counting_filter.insert("orange");
    
    cout << "插入后:\n";
    cout << "apple 存在: " << counting_filter.might_contain("apple") << "\n";
    cout << "banana 存在: " << counting_filter.might_contain("banana") << "\n";
    cout << "grape 存在: " << counting_filter.might_contain("grape") << "\n";
    
    counting_filter.remove("banana");
    cout << "\n删除 banana 后:\n";
    cout << "banana 存在: " << counting_filter.might_contain("banana") << "\n";
}

// HyperLogLog测试
void test_hyperloglog() {
    cout << "\n=== HyperLogLog测试 ===\n";
    
    HyperLogLog hll(12); // 2^12 = 4096 桶
    
    // 添加不同数量的唯一元素
    vector<int> test_sizes = {100, 1000, 10000, 100000};
    
    for (int size : test_sizes) {
        hll.clear();
        
        // 添加唯一元素
        for (int i = 0; i < size; ++i) {
            hll.add("unique_item_" + to_string(i));
        }
        
        size_t estimated = hll.estimate_cardinality();
        double error = abs(static_cast<int>(estimated) - size) / static_cast<double>(size);
        
        cout << "实际基数: " << size << ", 估计基数: " << estimated 
             << ", 误差: " << (error * 100) << "%\n";
    }
    
    hll.print_statistics();
}

// MinHash测试
void test_minhash() {
    cout << "\n=== MinHash测试 ===\n";
    
    MinHash mh1(128);
    MinHash mh2(128);
    MinHash mh3(128);
    
    // 创建三个集合
    vector<string> set1 = {"apple", "banana", "orange", "grape", "kiwi"};
    vector<string> set2 = {"apple", "banana", "mango", "pear", "peach"};
    vector<string> set3 = {"apple", "banana", "orange", "grape", "kiwi", "mango"};
    
    for (const auto& item : set1) mh1.add(item);
    for (const auto& item : set2) mh2.add(item);
    for (const auto& item : set3) mh3.add(item);
    
    cout << "Jaccard相似度:\n";
    cout << "集合1 vs 集合2: " << mh1.jaccard_similarity(mh2) << "\n";
    cout << "集合1 vs 集合3: " << mh1.jaccard_similarity(mh3) << "\n";
    cout << "集合2 vs 集合3: " << mh2.jaccard_similarity(mh3) << "\n";
    
    // 计算实际Jaccard相似度进行比较
    auto jaccard_actual = [](const vector<string>& a, const vector<string>& b) {
        set<string> union_set(a.begin(), a.end());
        union_set.insert(b.begin(), b.end());
        
        set<string> intersection;
        set_intersection(a.begin(), a.end(), b.begin(), b.end(),
                        inserter(intersection, intersection.begin()));
        
        return static_cast<double>(intersection.size()) / union_set.size();
    };
    
    cout << "\n实际Jaccard相似度:\n";
    cout << "集合1 vs 集合2: " << jaccard_actual(set1, set2) << "\n";
    cout << "集合1 vs 集合3: " << jaccard_actual(set1, set3) << "\n";
    cout << "集合2 vs 集合3: " << jaccard_actual(set2, set3) << "\n";
}

// 性能压力测试
void performance_stress_test() {
    cout << "\n=== 性能压力测试 ===\n";
    
    const size_t large_size = 100000;
    auto large_dataset = TestDataGenerator::generate_integer_pairs(large_size);
    
    cout << "测试数据大小: " << large_size << " 个键值对\n";
    
    // 测试链地址法哈希表
    {
        ChainingHashTable<int, int> table;
        HashTableTester<ChainingHashTable<int, int>>::performance_test(table, large_dataset);
        cout << "\n链地址法最终统计:\n";
        table.print_statistics();
    }
    
    cout << "\n" << string(50, '-') << "\n";
    
    // 测试Robin Hood哈希表
    {
        RobinHoodHashTable<int, int> table;
        auto start = chrono::high_resolution_clock::now();
        
        for (const auto& pair : large_dataset) {
            table.insert(pair.first, pair.second);
        }
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        
        cout << "Robin Hood哈希表插入时间: " << duration.count() << " 微秒\n";
        cout << "负载因子: " << table.load_factor() << "\n";
        cout << "平均距离: " << table.average_distance() << "\n";
    }
}

// 实际应用场景演示
void demonstrate_real_world_usage() {
    cout << "\n=== 实际应用场景演示 ===\n";
    
    // 场景1: 缓存系统
    cout << "场景1: LRU缓存系统\n";
    {
        // 简单的LRU缓存实现
        class LRUCache {
        private:
            struct Node {
                int key, value;
                Node* prev;
                Node* next;
                Node(int k = 0, int v = 0) : key(k), value(v), prev(nullptr), next(nullptr) {}
            };
            
            unordered_map<int, Node*> cache;
            Node* head;
            Node* tail;
            int capacity;
            
            void add_to_head(Node* node) {
                node->prev = head;
                node->next = head->next;
                head->next->prev = node;
                head->next = node;
            }
            
            void remove_node(Node* node) {
                node->prev->next = node->next;
                node->next->prev = node->prev;
            }
            
            void move_to_head(Node* node) {
                remove_node(node);
                add_to_head(node);
            }
            
            Node* pop_tail() {
                Node* last = tail->prev;
                remove_node(last);
                return last;
            }
            
        public:
            LRUCache(int cap) : capacity(cap) {
                head = new Node();
                tail = new Node();
                head->next = tail;
                tail->prev = head;
            }
            
            int get(int key) {
                auto it = cache.find(key);
                if (it == cache.end()) {
                    return -1;
                }
                
                Node* node = it->second;
                move_to_head(node);
                return node->value;
            }
            
            void put(int key, int value) {
                auto it = cache.find(key);
                if (it != cache.end()) {
                    Node* node = it->second;
                    node->value = value;
                    move_to_head(node);
                } else {
                    Node* new_node = new Node(key, value);
                    
                    if (cache.size() >= capacity) {
                        Node* tail_node = pop_tail();
                        cache.erase(tail_node->key);
                        delete tail_node;
                    }
                    
                    cache[key] = new_node;
                    add_to_head(new_node);
                }
            }
        };
        
        LRUCache lru(3);
        lru.put(1, 1);
        lru.put(2, 2);
        lru.put(3, 3);
        cout << "get(2): " << lru.get(2) << "\n";
        lru.put(4, 4); // 应该移除键1
        cout << "get(1): " << lru.get(1) << " (应该是-1)\n";
        cout << "get(3): " << lru.get(3) << "\n";
        cout << "get(4): " << lru.get(4) << "\n";
    }
    
    // 场景2: 分布式系统中的数据分片
    cout << "\n场景2: 分布式数据分片\n";
    {
        ConsistentHash sharding(50);
        vector<string> servers = {"db1", "db2", "db3", "db4"};
        
        for (const auto& server : servers) {
            sharding.add_node(server);
        }
        
        // 模拟数据分布
        map<string, vector<string>> data_distribution;
        for (int i = 0; i < 100; ++i) {
            string data_key = "data_" + to_string(i);
            string server = sharding.get_node(data_key);
            data_distribution[server].push_back(data_key);
        }
        
        cout << "数据分布:\n";
        for (const auto& pair : data_distribution) {
            cout << "  " << pair.first << ": " << pair.second.size() << " 条数据\n";
        }
    }
    
    // 场景3: 去重系统
    cout << "\n场景3: 大数据去重\n";
    {
        BloomFilter dedup_filter(100000, 0.001);
        
        // 模拟数据流
        int total_items = 50000;
        int duplicates = 0;
        int false_positives = 0;
        
        set<string> actual_seen;
        
        for (int i = 0; i < total_items; ++i) {
            // 生成可能重复的数据
            string item = "item_" + to_string(i % 30000); // 制造重复
            
            bool bloom_says_seen = dedup_filter.might_contain(item);
            bool actually_seen = actual_seen.count(item) > 0;
            
            if (actually_seen) {
                duplicates++;
            } else if (bloom_says_seen) {
                false_positives++;
            }
            
            if (!bloom_says_seen) {
                dedup_filter.insert(item);
                actual_seen.insert(item);
            }
        }
        
        cout << "处理了 " << total_items << " 个项目\n";
        cout << "实际重复: " << duplicates << "\n";
        cout << "假正例: " << false_positives << "\n";
        cout << "唯一项目: " << actual_seen.size() << "\n";
        cout << "节省的处理: " << (duplicates + false_positives) << " 次\n";
    }
}

int main() {
    cout << "哈希表数据结构教程示例\n";
    cout << string(50, '=') << "\n";
    
    try {
        test_hash_functions();
        test_basic_hash_table();
        test_collision_resolution();
        test_advanced_hash_techniques();
        test_bloom_filter();
        test_hyperloglog();
        test_minhash();
        performance_stress_test();
        demonstrate_real_world_usage();
        
        cout << "\n" << string(50, '=') << "\n";
        cout << "所有测试完成！\n";
        
    } catch (const exception& e) {
        cerr << "错误: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}