# 哈希表 (Hash Table)

## 概述

哈希表（Hash Table），也称为散列表，是一种基于哈希函数实现的数据结构。它通过将键（key）映射到表中的位置来访问记录，以加快查找速度。哈希表是实现关联数组（associative array）抽象数据类型的一种有效方式。

## 核心特性

### 基本概念

- **哈希函数（Hash Function）**：将键映射到哈希表索引的函数
- **哈希值（Hash Value）**：哈希函数的输出结果
- **桶（Bucket）**：哈希表中存储数据的位置
- **负载因子（Load Factor）**：已存储元素数量与哈希表大小的比值
- **哈希冲突（Hash Collision）**：不同键映射到相同哈希值的情况

### 时间复杂度

- **平均情况**：O(1) 查找、插入、删除
- **最坏情况**：O(n) 查找、插入、删除（所有元素都冲突）
- **空间复杂度**：O(n)

### 性能特点

- **快速访问**：理想情况下常数时间复杂度
- **动态大小**：支持动态扩容和缩容
- **内存效率**：合理的负载因子下内存利用率高
- **无序存储**：不保持元素的插入顺序

## 哈希函数

### 设计原则

1. **确定性**：相同输入总是产生相同输出
2. **均匀分布**：输出值在哈希表中均匀分布
3. **快速计算**：计算速度要快
4. **雪崩效应**：输入的微小变化导致输出的巨大变化

### 常见哈希函数

#### 除法哈希
```cpp
size_t division_hash(int key, size_t table_size) {
    return key % table_size;
}
```

#### 乘法哈希
```cpp
size_t multiplication_hash(int key, size_t table_size) {
    const double A = 0.6180339887; // (√5 - 1) / 2
    double temp = key * A;
    temp = temp - floor(temp);
    return (size_t)(table_size * temp);
}
```

#### 字符串哈希
```cpp
// DJB2 哈希
size_t djb2_hash(const string& str) {
    size_t hash = 5381;
    for (char c : str) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// FNV-1a 哈希
size_t fnv1a_hash(const string& str) {
    size_t hash = 2166136261u;
    for (char c : str) {
        hash ^= c;
        hash *= 16777619;
    }
    return hash;
}
```

#### 通用哈希
```cpp
// 通用哈希族
class UniversalHash {
private:
    size_t a, b, p, m;
public:
    UniversalHash(size_t table_size) : m(table_size) {
        p = next_prime(table_size * 2);
        a = rand() % (p - 1) + 1;
        b = rand() % p;
    }
    
    size_t hash(int key) {
        return ((a * key + b) % p) % m;
    }
};
```

## 冲突解决方法

### 开放寻址法（Open Addressing）

#### 线性探测
```cpp
class LinearProbingHashTable {
private:
    vector<pair<int, bool>> table; // (value, is_occupied)
    size_t size, capacity;
    
    size_t probe(int key, int i) {
        return (hash(key) + i) % capacity;
    }
};
```

#### 二次探测
```cpp
size_t quadratic_probe(int key, int i) {
    return (hash(key) + i * i) % capacity;
}
```

#### 双重哈希
```cpp
size_t double_hash_probe(int key, int i) {
    return (hash1(key) + i * hash2(key)) % capacity;
}
```

### 链地址法（Separate Chaining）

```cpp
class ChainingHashTable {
private:
    vector<list<pair<int, int>>> table; // key-value pairs
    size_t size, capacity;
    
public:
    void insert(int key, int value) {
        size_t index = hash(key);
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }
        table[index].emplace_back(key, value);
        size++;
        
        if (load_factor() > 0.75) {
            resize();
        }
    }
};
```

### 布谷鸟哈希（Cuckoo Hashing）

```cpp
class CuckooHashTable {
private:
    vector<int> table1, table2;
    size_t capacity;
    
    bool insert_helper(int key, int table_num, int count) {
        if (count > capacity) return false;
        
        if (table_num == 1) {
            size_t pos = hash1(key);
            if (table1[pos] == EMPTY) {
                table1[pos] = key;
                return true;
            }
            int displaced = table1[pos];
            table1[pos] = key;
            return insert_helper(displaced, 2, count + 1);
        } else {
            size_t pos = hash2(key);
            if (table2[pos] == EMPTY) {
                table2[pos] = key;
                return true;
            }
            int displaced = table2[pos];
            table2[pos] = key;
            return insert_helper(displaced, 1, count + 1);
        }
    }
};
```

## 动态调整

### 负载因子管理

```cpp
double load_factor() const {
    return static_cast<double>(size) / capacity;
}

bool should_resize() const {
    return load_factor() > 0.75 || load_factor() < 0.25;
}
```

### 动态扩容

```cpp
void resize(size_t new_capacity) {
    vector<pair<int, int>> old_data;
    
    // 保存旧数据
    for (const auto& bucket : table) {
        for (const auto& pair : bucket) {
            old_data.push_back(pair);
        }
    }
    
    // 重新初始化
    table.clear();
    table.resize(new_capacity);
    capacity = new_capacity;
    size = 0;
    
    // 重新插入数据
    for (const auto& pair : old_data) {
        insert(pair.first, pair.second);
    }
}
```

## 高级哈希技术

### 一致性哈希

```cpp
class ConsistentHash {
private:
    map<size_t, string> ring;
    size_t virtual_nodes;
    
public:
    void add_node(const string& node) {
        for (size_t i = 0; i < virtual_nodes; ++i) {
            string virtual_node = node + "#" + to_string(i);
            size_t hash_value = hash_function(virtual_node);
            ring[hash_value] = node;
        }
    }
    
    string get_node(const string& key) {
        size_t hash_value = hash_function(key);
        auto it = ring.lower_bound(hash_value);
        if (it == ring.end()) {
            it = ring.begin();
        }
        return it->second;
    }
};
```

### 布隆过滤器

```cpp
class BloomFilter {
private:
    vector<bool> bit_array;
    size_t size;
    size_t hash_count;
    
public:
    void insert(const string& item) {
        for (size_t i = 0; i < hash_count; ++i) {
            size_t hash_value = hash_function(item, i) % size;
            bit_array[hash_value] = true;
        }
    }
    
    bool might_contain(const string& item) {
        for (size_t i = 0; i < hash_count; ++i) {
            size_t hash_value = hash_function(item, i) % size;
            if (!bit_array[hash_value]) {
                return false;
            }
        }
        return true;
    }
};
```

### 计数布隆过滤器

```cpp
class CountingBloomFilter {
private:
    vector<int> counters;
    size_t size;
    size_t hash_count;
    
public:
    void insert(const string& item) {
        for (size_t i = 0; i < hash_count; ++i) {
            size_t hash_value = hash_function(item, i) % size;
            counters[hash_value]++;
        }
    }
    
    void remove(const string& item) {
        for (size_t i = 0; i < hash_count; ++i) {
            size_t hash_value = hash_function(item, i) % size;
            if (counters[hash_value] > 0) {
                counters[hash_value]--;
            }
        }
    }
};
```

## 应用场景

### 数据库索引

- **哈希索引**：快速等值查询
- **分区表**：数据分布和负载均衡
- **缓存系统**：快速数据访问

### 分布式系统

- **负载均衡**：请求分发
- **数据分片**：数据分布
- **缓存一致性**：分布式缓存

### 安全应用

- **密码存储**：密码哈希
- **数字签名**：完整性验证
- **区块链**：工作量证明

### 算法优化

- **去重**：快速查重
- **计数**：频率统计
- **查找**：快速检索

## 经典算法问题

### 初级问题

1. **两数之和**：使用哈希表优化查找
2. **有效的字母异位词**：字符频率统计
3. **存在重复元素**：快速查重
4. **单词模式**：模式匹配
5. **赎金信**：字符计数

### 中级问题

1. **字母异位词分组**：分组哈希
2. **最长连续序列**：序列查找
3. **四数相加II**：哈希优化
4. **前K个高频元素**：频率统计+排序
5. **设计哈希映射**：实现哈希表

### 高级问题

1. **LRU缓存**：哈希表+双向链表
2. **设计推特**：多级哈希
3. **随机化集合**：哈希表+动态数组
4. **O(1)时间插入、删除和获取随机元素**：复合数据结构
5. **一致性哈希**：分布式系统设计

## 哈希表变种

### Robin Hood哈希

```cpp
class RobinHoodHashTable {
private:
    struct Entry {
        int key, value;
        int distance; // 距离理想位置的距离
        bool occupied;
    };
    
    vector<Entry> table;
    
    void insert_entry(Entry entry) {
        while (true) {
            size_t pos = (hash(entry.key) + entry.distance) % capacity;
            
            if (!table[pos].occupied) {
                table[pos] = entry;
                return;
            }
            
            if (table[pos].distance < entry.distance) {
                swap(table[pos], entry);
            }
            
            entry.distance++;
        }
    }
};
```

### Hopscotch哈希

```cpp
class HopscotchHashTable {
private:
    static const int HOP_RANGE = 32;
    
    struct Entry {
        int key, value;
        uint32_t hop_info; // 位图，表示邻域内的占用情况
        bool occupied;
    };
    
    vector<Entry> table;
};
```

### 2-Choice哈希

```cpp
class TwoChoiceHashTable {
private:
    vector<list<pair<int, int>>> table1, table2;
    
public:
    void insert(int key, int value) {
        size_t pos1 = hash1(key) % table1.size();
        size_t pos2 = hash2(key) % table2.size();
        
        // 选择负载较小的桶
        if (table1[pos1].size() <= table2[pos2].size()) {
            table1[pos1].emplace_back(key, value);
        } else {
            table2[pos2].emplace_back(key, value);
        }
    }
};
```

## 时间复杂度分析

| 操作 | 平均时间 | 最坏时间 | 最好时间 |
|------|----------|----------|----------|
| 查找 | O(1) | O(n) | O(1) |
| 插入 | O(1) | O(n) | O(1) |
| 删除 | O(1) | O(n) | O(1) |
| 空间 | O(n) | O(n) | O(n) |

### 负载因子影响

| 负载因子 | 平均探测次数 | 性能 |
|----------|--------------|------|
| 0.5 | 1.5 | 优秀 |
| 0.75 | 2.5 | 良好 |
| 0.9 | 5.5 | 一般 |
| 0.95 | 10.5 | 较差 |

## 实现文件结构

```
04_hash/
├── README.md                    # 本文件
├── hash_functions.h             # 哈希函数实现
├── hash_table.h                 # 基本哈希表实现
├── collision_resolution.h       # 冲突解决方法
├── advanced_hash.h              # 高级哈希技术
└── example.cpp                  # 使用示例和测试
```

## 使用示例

### 基本用法

```cpp
#include "hash_table.h"

int main() {
    // 创建哈希表
    HashTable<int, string> hash_table;
    
    // 插入数据
    hash_table.insert(1, "apple");
    hash_table.insert(2, "banana");
    hash_table.insert(3, "orange");
    
    // 查找数据
    string value;
    if (hash_table.find(2, value)) {
        cout << "Found: " << value << endl;
    }
    
    // 删除数据
    hash_table.remove(1);
    
    // 遍历哈希表
    hash_table.for_each([](int key, const string& value) {
        cout << key << ": " << value << endl;
    });
    
    return 0;
}
```

### 高级用法

```cpp
#include "advanced_hash.h"

int main() {
    // 布隆过滤器
    BloomFilter bloom_filter(1000, 3);
    bloom_filter.insert("apple");
    bloom_filter.insert("banana");
    
    cout << bloom_filter.might_contain("apple") << endl;   // true
    cout << bloom_filter.might_contain("grape") << endl;   // false (可能)
    
    // 一致性哈希
    ConsistentHash consistent_hash(150);
    consistent_hash.add_node("server1");
    consistent_hash.add_node("server2");
    consistent_hash.add_node("server3");
    
    string server = consistent_hash.get_node("user123");
    cout << "User123 -> " << server << endl;
    
    return 0;
}
```

## 编译和运行

```bash
# 编译示例程序
g++ -std=c++17 -O2 -o hash_example example.cpp

# 运行示例
./hash_example

# 编译时启用调试信息
g++ -std=c++17 -g -DDEBUG -o hash_debug example.cpp

# 性能测试编译
g++ -std=c++17 -O3 -DNDEBUG -o hash_performance example.cpp
```

## 练习题目

### 初级练习

1. **实现基本哈希表**：使用链地址法解决冲突
2. **字符串哈希**：实现多种字符串哈希函数
3. **负载因子控制**：实现动态扩容机制
4. **哈希函数比较**：比较不同哈希函数的性能
5. **冲突统计**：统计不同方法的冲突率

### 中级练习

1. **开放寻址法**：实现线性探测、二次探测、双重哈希
2. **Robin Hood哈希**：实现Robin Hood哈希表
3. **布隆过滤器**：实现可扩展的布隆过滤器
4. **LRU缓存**：使用哈希表+双向链表实现
5. **一致性哈希**：实现支持虚拟节点的一致性哈希

### 高级练习

1. **并发哈希表**：实现线程安全的哈希表
2. **分布式哈希**：实现分布式哈希表
3. **内存池哈希表**：优化内存分配的哈希表
4. **持久化哈希表**：支持磁盘存储的哈希表
5. **哈希表性能优化**：SIMD优化、缓存友好设计

## 调试技巧

### 哈希函数质量检测

```cpp
void test_hash_quality(HashFunction hash_func, int num_keys) {
    vector<int> bucket_counts(TABLE_SIZE, 0);
    
    for (int i = 0; i < num_keys; ++i) {
        size_t hash_value = hash_func(i) % TABLE_SIZE;
        bucket_counts[hash_value]++;
    }
    
    // 计算分布均匀性
    double expected = static_cast<double>(num_keys) / TABLE_SIZE;
    double chi_square = 0.0;
    
    for (int count : bucket_counts) {
        double diff = count - expected;
        chi_square += (diff * diff) / expected;
    }
    
    cout << "Chi-square value: " << chi_square << endl;
}
```

### 性能分析

```cpp
void performance_analysis() {
    auto start = chrono::high_resolution_clock::now();
    
    // 执行操作
    for (int i = 0; i < 1000000; ++i) {
        hash_table.insert(i, i * 2);
    }
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    
    cout << "Insert time: " << duration.count() << " microseconds" << endl;
    cout << "Load factor: " << hash_table.load_factor() << endl;
    cout << "Collision rate: " << hash_table.collision_rate() << endl;
}
```

### 内存使用分析

```cpp
void memory_analysis() {
    size_t table_memory = hash_table.memory_usage();
    size_t overhead = table_memory - hash_table.size() * sizeof(Entry);
    
    cout << "Total memory: " << table_memory << " bytes" << endl;
    cout << "Data memory: " << hash_table.size() * sizeof(Entry) << " bytes" << endl;
    cout << "Overhead: " << overhead << " bytes (" 
         << (100.0 * overhead / table_memory) << "%)" << endl;
}
```

## 学习建议

### 学习路径

1. **基础概念**：理解哈希函数和冲突解决
2. **基本实现**：实现简单的哈希表
3. **性能优化**：学习负载因子控制和动态调整
4. **高级技术**：掌握一致性哈希、布隆过滤器等
5. **实际应用**：在项目中应用哈希表

### 实践建议

1. **从简单开始**：先实现基本的链地址法哈希表
2. **性能测试**：比较不同实现的性能差异
3. **实际应用**：在算法题中使用哈希表优化
4. **源码学习**：研究STL unordered_map的实现
5. **系统设计**：在系统设计中合理使用哈希技术

### 进阶学习

1. **并发编程**：学习无锁哈希表设计
2. **分布式系统**：理解分布式哈希的应用
3. **数据库系统**：学习数据库中的哈希索引
4. **密码学**：了解密码学哈希函数
5. **系统优化**：学习缓存友好的哈希表设计

## 参考资料

### 经典教材

1. **《算法导论》** - Cormen等著
2. **《数据结构与算法分析》** - Mark Allen Weiss著
3. **《算法》** - Robert Sedgewick著

### 论文资料

1. **"Robin Hood Hashing"** - Pedro Celis
2. **"Cuckoo Hashing"** - Rasmus Pagh
3. **"Consistent Hashing"** - David Karger

### 在线资源

1. **LeetCode哈希表专题**
2. **GeeksforGeeks哈希教程**
3. **Coursera算法课程**
4. **MIT 6.006课程**

### 开源实现

1. **Google Dense Hash Map**
2. **Facebook Folly Hash Map**
3. **Boost Unordered**
4. **STL unordered_map源码**