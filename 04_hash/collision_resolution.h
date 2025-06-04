#ifndef COLLISION_RESOLUTION_H
#define COLLISION_RESOLUTION_H

#include <vector>
#include <list>
#include <random>
#include <algorithm>
#include <bitset>
#include <cstring>
#include "hash_table.h"

namespace collision_resolution {

// Robin Hood 哈希表
template<typename Key, typename Value, typename Hash = std::hash<Key>>
class RobinHoodHashTable : public hash_table::HashTableInterface<Key, Value> {
private:
    struct Entry {
        Key key;
        Value value;
        int distance; // 距离理想位置的距离
        bool occupied;
        
        Entry() : distance(0), occupied(false) {}
        Entry(const Key& k, const Value& v, int d) 
            : key(k), value(v), distance(d), occupied(true) {}
    };
    
    std::vector<Entry> table;
    size_t table_size;
    size_t num_elements;
    Hash hash_function;
    
    static constexpr double MAX_LOAD_FACTOR = 0.9;
    static constexpr size_t INITIAL_SIZE = 16;
    
    size_t hash(const Key& key) const {
        return hash_function(key) % table_size;
    }
    
    void resize(size_t new_size) {
        std::vector<Entry> old_table = std::move(table);
        table.clear();
        table.resize(new_size);
        size_t old_size = table_size;
        table_size = new_size;
        num_elements = 0;
        
        for (const auto& entry : old_table) {
            if (entry.occupied) {
                insert(entry.key, entry.value);
            }
        }
    }
    
    void insert_entry(Entry entry) {
        while (true) {
            size_t pos = (hash(entry.key) + entry.distance) % table_size;
            
            if (!table[pos].occupied) {
                table[pos] = entry;
                return;
            }
            
            // Robin Hood: 如果当前元素比表中元素更"贫穷"，则交换
            if (table[pos].distance < entry.distance) {
                std::swap(table[pos], entry);
            }
            
            entry.distance++;
        }
    }
    
public:
    explicit RobinHoodHashTable(size_t initial_size = INITIAL_SIZE)
        : table(initial_size), table_size(initial_size), num_elements(0) {}
    
    void insert(const Key& key, const Value& value) override {
        if (load_factor() > MAX_LOAD_FACTOR) {
            resize(table_size * 2);
        }
        
        // 检查是否已存在
        Value dummy;
        if (find(key, dummy)) {
            // 更新现有值
            size_t ideal_pos = hash(key);
            for (int distance = 0; distance < static_cast<int>(table_size); ++distance) {
                size_t pos = (ideal_pos + distance) % table_size;
                if (table[pos].occupied && table[pos].key == key) {
                    table[pos].value = value;
                    return;
                }
            }
        }
        
        Entry new_entry(key, value, 0);
        insert_entry(new_entry);
        num_elements++;
    }
    
    bool find(const Key& key, Value& value) const override {
        size_t ideal_pos = hash(key);
        
        for (int distance = 0; distance < static_cast<int>(table_size); ++distance) {
            size_t pos = (ideal_pos + distance) % table_size;
            
            if (!table[pos].occupied) {
                return false;
            }
            
            if (table[pos].key == key) {
                value = table[pos].value;
                return true;
            }
            
            // 如果当前位置的元素距离比我们查找的距离小，说明元素不存在
            if (table[pos].distance < distance) {
                return false;
            }
        }
        
        return false;
    }
    
    bool remove(const Key& key) override {
        size_t ideal_pos = hash(key);
        
        for (int distance = 0; distance < static_cast<int>(table_size); ++distance) {
            size_t pos = (ideal_pos + distance) % table_size;
            
            if (!table[pos].occupied) {
                return false;
            }
            
            if (table[pos].key == key) {
                // 找到要删除的元素
                table[pos].occupied = false;
                num_elements--;
                
                // 向后移动元素以填补空隙
                size_t next_pos = (pos + 1) % table_size;
                while (table[next_pos].occupied && table[next_pos].distance > 0) {
                    table[pos] = table[next_pos];
                    table[pos].distance--;
                    table[next_pos].occupied = false;
                    
                    pos = next_pos;
                    next_pos = (next_pos + 1) % table_size;
                }
                
                return true;
            }
            
            if (table[pos].distance < distance) {
                return false;
            }
        }
        
        return false;
    }
    
    size_t size() const override { return num_elements; }
    bool empty() const override { return num_elements == 0; }
    
    void clear() override {
        for (auto& entry : table) {
            entry.occupied = false;
        }
        num_elements = 0;
    }
    
    double load_factor() const override {
        return static_cast<double>(num_elements) / table_size;
    }
    
    // 统计平均距离
    double average_distance() const {
        if (num_elements == 0) return 0.0;
        
        int total_distance = 0;
        for (const auto& entry : table) {
            if (entry.occupied) {
                total_distance += entry.distance;
            }
        }
        
        return static_cast<double>(total_distance) / num_elements;
    }
};

// Hopscotch 哈希表
template<typename Key, typename Value, typename Hash = std::hash<Key>>
class HopscotchHashTable : public hash_table::HashTableInterface<Key, Value> {
private:
    static constexpr int HOP_RANGE = 32;
    
    struct Entry {
        Key key;
        Value value;
        std::bitset<HOP_RANGE> hop_info; // 邻域信息
        bool occupied;
        
        Entry() : occupied(false) {}
        Entry(const Key& k, const Value& v) : key(k), value(v), occupied(true) {}
    };
    
    std::vector<Entry> table;
    size_t table_size;
    size_t num_elements;
    Hash hash_function;
    
    static constexpr double MAX_LOAD_FACTOR = 0.75;
    static constexpr size_t INITIAL_SIZE = 64; // 必须大于HOP_RANGE
    
    size_t hash(const Key& key) const {
        return hash_function(key) % table_size;
    }
    
    void resize(size_t new_size) {
        std::vector<Entry> old_table = std::move(table);
        table.clear();
        table.resize(new_size);
        size_t old_size = table_size;
        table_size = new_size;
        num_elements = 0;
        
        for (const auto& entry : old_table) {
            if (entry.occupied) {
                insert(entry.key, entry.value);
            }
        }
    }
    
    bool find_closer_bucket(size_t start_bucket, size_t& free_bucket) {
        // 尝试在邻域内找到更近的空桶
        for (int distance = HOP_RANGE - 1; distance > 0; --distance) {
            size_t check_bucket = (start_bucket + distance) % table_size;
            
            for (int hop_distance = 1; hop_distance < HOP_RANGE; ++hop_distance) {
                size_t hop_bucket = (check_bucket - hop_distance + table_size) % table_size;
                
                if (table[hop_bucket].hop_info[hop_distance]) {
                    // 找到可以移动的元素
                    size_t occupied_bucket = (check_bucket + hop_distance) % table_size;
                    
                    // 移动元素
                    table[free_bucket] = table[occupied_bucket];
                    table[occupied_bucket].occupied = false;
                    
                    // 更新hop_info
                    table[hop_bucket].hop_info[hop_distance] = false;
                    size_t new_distance = (free_bucket - hop_bucket + table_size) % table_size;
                    table[hop_bucket].hop_info[new_distance] = true;
                    
                    free_bucket = occupied_bucket;
                    return true;
                }
            }
        }
        
        return false;
    }
    
public:
    explicit HopscotchHashTable(size_t initial_size = INITIAL_SIZE)
        : table(std::max(initial_size, static_cast<size_t>(HOP_RANGE * 2))), 
          table_size(std::max(initial_size, static_cast<size_t>(HOP_RANGE * 2))), 
          num_elements(0) {}
    
    void insert(const Key& key, const Value& value) override {
        if (load_factor() > MAX_LOAD_FACTOR) {
            resize(table_size * 2);
        }
        
        size_t start_bucket = hash(key);
        
        // 检查是否已存在
        for (int i = 0; i < HOP_RANGE; ++i) {
            if (table[start_bucket].hop_info[i]) {
                size_t bucket = (start_bucket + i) % table_size;
                if (table[bucket].occupied && table[bucket].key == key) {
                    table[bucket].value = value;
                    return;
                }
            }
        }
        
        // 寻找空桶
        size_t free_bucket = start_bucket;
        while (table[free_bucket].occupied) {
            free_bucket = (free_bucket + 1) % table_size;
            
            // 如果搜索了整个表都没找到空位，需要扩容
            if (free_bucket == start_bucket) {
                resize(table_size * 2);
                insert(key, value); // 重新插入
                return;
            }
        }
        
        // 如果空桶在邻域外，尝试移动到邻域内
        while ((free_bucket - start_bucket + table_size) % table_size >= HOP_RANGE) {
            if (!find_closer_bucket(start_bucket, free_bucket)) {
                resize(table_size * 2);
                insert(key, value);
                return;
            }
        }
        
        // 插入元素
        table[free_bucket] = Entry(key, value);
        size_t distance = (free_bucket - start_bucket + table_size) % table_size;
        table[start_bucket].hop_info[distance] = true;
        num_elements++;
    }
    
    bool find(const Key& key, Value& value) const override {
        size_t start_bucket = hash(key);
        
        for (int i = 0; i < HOP_RANGE; ++i) {
            if (table[start_bucket].hop_info[i]) {
                size_t bucket = (start_bucket + i) % table_size;
                if (table[bucket].occupied && table[bucket].key == key) {
                    value = table[bucket].value;
                    return true;
                }
            }
        }
        
        return false;
    }
    
    bool remove(const Key& key) override {
        size_t start_bucket = hash(key);
        
        for (int i = 0; i < HOP_RANGE; ++i) {
            if (table[start_bucket].hop_info[i]) {
                size_t bucket = (start_bucket + i) % table_size;
                if (table[bucket].occupied && table[bucket].key == key) {
                    table[bucket].occupied = false;
                    table[start_bucket].hop_info[i] = false;
                    num_elements--;
                    return true;
                }
            }
        }
        
        return false;
    }
    
    size_t size() const override { return num_elements; }
    bool empty() const override { return num_elements == 0; }
    
    void clear() override {
        for (auto& entry : table) {
            entry.occupied = false;
            entry.hop_info.reset();
        }
        num_elements = 0;
    }
    
    double load_factor() const override {
        return static_cast<double>(num_elements) / table_size;
    }
};

// 布谷鸟哈希表
template<typename Key, typename Value, typename Hash1 = std::hash<Key>, typename Hash2 = std::hash<Key>>
class CuckooHashTable : public hash_table::HashTableInterface<Key, Value> {
private:
    std::vector<std::pair<Key, Value>> table1, table2;
    std::vector<bool> occupied1, occupied2;
    size_t table_size;
    size_t num_elements;
    Hash1 hash1_function;
    Hash2 hash2_function;
    
    static constexpr size_t MAX_ITERATIONS = 8;
    static constexpr double MAX_LOAD_FACTOR = 0.5;
    static constexpr size_t INITIAL_SIZE = 16;
    
    size_t hash1(const Key& key) const {
        return hash1_function(key) % table_size;
    }
    
    size_t hash2(const Key& key) const {
        return hash2_function(key) % table_size;
    }
    
    void resize(size_t new_size) {
        std::vector<std::pair<Key, Value>> old_elements;
        
        // 收集所有元素
        for (size_t i = 0; i < table_size; ++i) {
            if (occupied1[i]) {
                old_elements.push_back(table1[i]);
            }
            if (occupied2[i]) {
                old_elements.push_back(table2[i]);
            }
        }
        
        // 重新初始化
        table_size = new_size;
        table1.resize(table_size);
        table2.resize(table_size);
        occupied1.assign(table_size, false);
        occupied2.assign(table_size, false);
        num_elements = 0;
        
        // 重新插入所有元素
        for (const auto& element : old_elements) {
            insert(element.first, element.second);
        }
    }
    
    bool insert_helper(const Key& key, const Value& value, int table_num, int count) {
        if (count > MAX_ITERATIONS * table_size) {
            return false; // 插入失败，需要扩容
        }
        
        if (table_num == 1) {
            size_t pos = hash1(key);
            if (!occupied1[pos]) {
                table1[pos] = std::make_pair(key, value);
                occupied1[pos] = true;
                return true;
            }
            
            // 踢出现有元素
            auto displaced = table1[pos];
            table1[pos] = std::make_pair(key, value);
            return insert_helper(displaced.first, displaced.second, 2, count + 1);
        } else {
            size_t pos = hash2(key);
            if (!occupied2[pos]) {
                table2[pos] = std::make_pair(key, value);
                occupied2[pos] = true;
                return true;
            }
            
            // 踢出现有元素
            auto displaced = table2[pos];
            table2[pos] = std::make_pair(key, value);
            return insert_helper(displaced.first, displaced.second, 1, count + 1);
        }
    }
    
public:
    explicit CuckooHashTable(size_t initial_size = INITIAL_SIZE)
        : table1(initial_size), table2(initial_size),
          occupied1(initial_size, false), occupied2(initial_size, false),
          table_size(initial_size), num_elements(0) {}
    
    void insert(const Key& key, const Value& value) override {
        // 检查是否已存在
        Value dummy;
        if (find(key, dummy)) {
            // 更新现有值
            size_t pos1 = hash1(key);
            size_t pos2 = hash2(key);
            
            if (occupied1[pos1] && table1[pos1].first == key) {
                table1[pos1].second = value;
                return;
            }
            if (occupied2[pos2] && table2[pos2].first == key) {
                table2[pos2].second = value;
                return;
            }
        }
        
        // 尝试插入
        while (!insert_helper(key, value, 1, 0)) {
            resize(table_size * 2);
        }
        
        num_elements++;
    }
    
    bool find(const Key& key, Value& value) const override {
        size_t pos1 = hash1(key);
        if (occupied1[pos1] && table1[pos1].first == key) {
            value = table1[pos1].second;
            return true;
        }
        
        size_t pos2 = hash2(key);
        if (occupied2[pos2] && table2[pos2].first == key) {
            value = table2[pos2].second;
            return true;
        }
        
        return false;
    }
    
    bool remove(const Key& key) override {
        size_t pos1 = hash1(key);
        if (occupied1[pos1] && table1[pos1].first == key) {
            occupied1[pos1] = false;
            num_elements--;
            return true;
        }
        
        size_t pos2 = hash2(key);
        if (occupied2[pos2] && table2[pos2].first == key) {
            occupied2[pos2] = false;
            num_elements--;
            return true;
        }
        
        return false;
    }
    
    size_t size() const override { return num_elements; }
    bool empty() const override { return num_elements == 0; }
    
    void clear() override {
        occupied1.assign(table_size, false);
        occupied2.assign(table_size, false);
        num_elements = 0;
    }
    
    double load_factor() const override {
        return static_cast<double>(num_elements) / (2 * table_size);
    }
    
    // 统计两个表的负载分布
    void print_load_distribution() const {
        size_t count1 = 0, count2 = 0;
        for (size_t i = 0; i < table_size; ++i) {
            if (occupied1[i]) count1++;
            if (occupied2[i]) count2++;
        }
        
        std::cout << "Table 1 load: " << count1 << "/" << table_size 
                  << " (" << (100.0 * count1 / table_size) << "%)\n";
        std::cout << "Table 2 load: " << count2 << "/" << table_size 
                  << " (" << (100.0 * count2 / table_size) << "%)\n";
    }
};

// 2-Choice 哈希表
template<typename Key, typename Value, typename Hash1 = std::hash<Key>, typename Hash2 = std::hash<Key>>
class TwoChoiceHashTable : public hash_table::HashTableInterface<Key, Value> {
public:
    using KeyValuePair = std::pair<Key, Value>;
    using Bucket = std::list<KeyValuePair>;
    
private:
    std::vector<Bucket> table1, table2;
    size_t table_size;
    size_t num_elements;
    Hash1 hash1_function;
    Hash2 hash2_function;
    
    static constexpr double MAX_LOAD_FACTOR = 1.0;
    static constexpr size_t INITIAL_SIZE = 16;
    
    size_t hash1(const Key& key) const {
        return hash1_function(key) % table_size;
    }
    
    size_t hash2(const Key& key) const {
        return hash2_function(key) % table_size;
    }
    
    void resize(size_t new_size) {
        std::vector<KeyValuePair> old_elements;
        
        // 收集所有元素
        for (const auto& bucket : table1) {
            for (const auto& pair : bucket) {
                old_elements.push_back(pair);
            }
        }
        for (const auto& bucket : table2) {
            for (const auto& pair : bucket) {
                old_elements.push_back(pair);
            }
        }
        
        // 重新初始化
        table1.clear();
        table2.clear();
        table1.resize(new_size);
        table2.resize(new_size);
        table_size = new_size;
        num_elements = 0;
        
        // 重新插入所有元素
        for (const auto& pair : old_elements) {
            insert(pair.first, pair.second);
        }
    }
    
public:
    explicit TwoChoiceHashTable(size_t initial_size = INITIAL_SIZE)
        : table1(initial_size), table2(initial_size), 
          table_size(initial_size), num_elements(0) {}
    
    void insert(const Key& key, const Value& value) override {
        // 检查是否需要扩容
        if (load_factor() > MAX_LOAD_FACTOR) {
            resize(table_size * 2);
        }
        
        size_t pos1 = hash1(key);
        size_t pos2 = hash2(key);
        
        // 检查是否已存在
        for (auto& pair : table1[pos1]) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }
        for (auto& pair : table2[pos2]) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }
        
        // 选择负载较小的桶
        if (table1[pos1].size() <= table2[pos2].size()) {
            table1[pos1].emplace_back(key, value);
        } else {
            table2[pos2].emplace_back(key, value);
        }
        
        num_elements++;
    }
    
    bool find(const Key& key, Value& value) const override {
        size_t pos1 = hash1(key);
        for (const auto& pair : table1[pos1]) {
            if (pair.first == key) {
                value = pair.second;
                return true;
            }
        }
        
        size_t pos2 = hash2(key);
        for (const auto& pair : table2[pos2]) {
            if (pair.first == key) {
                value = pair.second;
                return true;
            }
        }
        
        return false;
    }
    
    bool remove(const Key& key) override {
        size_t pos1 = hash1(key);
        auto& bucket1 = table1[pos1];
        for (auto it = bucket1.begin(); it != bucket1.end(); ++it) {
            if (it->first == key) {
                bucket1.erase(it);
                num_elements--;
                return true;
            }
        }
        
        size_t pos2 = hash2(key);
        auto& bucket2 = table2[pos2];
        for (auto it = bucket2.begin(); it != bucket2.end(); ++it) {
            if (it->first == key) {
                bucket2.erase(it);
                num_elements--;
                return true;
            }
        }
        
        return false;
    }
    
    size_t size() const override { return num_elements; }
    bool empty() const override { return num_elements == 0; }
    
    void clear() override {
        for (auto& bucket : table1) bucket.clear();
        for (auto& bucket : table2) bucket.clear();
        num_elements = 0;
    }
    
    double load_factor() const override {
        return static_cast<double>(num_elements) / (2 * table_size);
    }
    
    // 统计最大桶大小
    size_t max_bucket_size() const {
        size_t max_size = 0;
        for (const auto& bucket : table1) {
            max_size = std::max(max_size, bucket.size());
        }
        for (const auto& bucket : table2) {
            max_size = std::max(max_size, bucket.size());
        }
        return max_size;
    }
    
    // 统计负载分布
    void print_load_distribution() const {
        std::vector<size_t> bucket_sizes;
        for (const auto& bucket : table1) {
            bucket_sizes.push_back(bucket.size());
        }
        for (const auto& bucket : table2) {
            bucket_sizes.push_back(bucket.size());
        }
        
        std::sort(bucket_sizes.begin(), bucket_sizes.end());
        
        std::cout << "Bucket size distribution:\n";
        std::cout << "Min: " << bucket_sizes.front() << "\n";
        std::cout << "Max: " << bucket_sizes.back() << "\n";
        std::cout << "Median: " << bucket_sizes[bucket_sizes.size() / 2] << "\n";
        
        double avg = 0.0;
        for (size_t size : bucket_sizes) {
            avg += size;
        }
        avg /= bucket_sizes.size();
        std::cout << "Average: " << avg << "\n";
    }
};

} // namespace collision_resolution

#endif // COLLISION_RESOLUTION_H