#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <vector>
#include <list>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <memory>
#include "hash_functions.h"

namespace hash_table {

// 基础哈希表接口
template<typename Key, typename Value>
class HashTableInterface {
public:
    virtual ~HashTableInterface() = default;
    virtual void insert(const Key& key, const Value& value) = 0;
    virtual bool find(const Key& key, Value& value) const = 0;
    virtual bool remove(const Key& key) = 0;
    virtual size_t size() const = 0;
    virtual bool empty() const = 0;
    virtual void clear() = 0;
    virtual double load_factor() const = 0;
};

// 链地址法哈希表
template<typename Key, typename Value, typename Hash = std::hash<Key>>
class ChainingHashTable : public HashTableInterface<Key, Value> {
public:
    using KeyValuePair = std::pair<Key, Value>;
    using Bucket = std::list<KeyValuePair>;
    
private:
    std::vector<Bucket> table;
    size_t table_size;
    size_t num_elements;
    Hash hash_function;
    
    static constexpr double MAX_LOAD_FACTOR = 0.75;
    static constexpr double MIN_LOAD_FACTOR = 0.25;
    static constexpr size_t INITIAL_SIZE = 16;
    
    size_t hash(const Key& key) const {
        return hash_function(key) % table_size;
    }
    
    void resize(size_t new_size) {
        std::vector<Bucket> old_table = std::move(table);
        table.clear();
        table.resize(new_size);
        size_t old_size = table_size;
        table_size = new_size;
        num_elements = 0;
        
        // 重新插入所有元素
        for (const auto& bucket : old_table) {
            for (const auto& pair : bucket) {
                insert(pair.first, pair.second);
            }
        }
    }
    
public:
    explicit ChainingHashTable(size_t initial_size = INITIAL_SIZE) 
        : table(initial_size), table_size(initial_size), num_elements(0) {}
    
    void insert(const Key& key, const Value& value) override {
        size_t index = hash(key);
        
        // 检查是否已存在
        for (auto& pair : table[index]) {
            if (pair.first == key) {
                pair.second = value;
                return;
            }
        }
        
        // 插入新元素
        table[index].emplace_back(key, value);
        num_elements++;
        
        // 检查是否需要扩容
        if (load_factor() > MAX_LOAD_FACTOR) {
            resize(table_size * 2);
        }
    }
    
    bool find(const Key& key, Value& value) const override {
        size_t index = hash(key);
        
        for (const auto& pair : table[index]) {
            if (pair.first == key) {
                value = pair.second;
                return true;
            }
        }
        
        return false;
    }
    
    bool remove(const Key& key) override {
        size_t index = hash(key);
        
        auto& bucket = table[index];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->first == key) {
                bucket.erase(it);
                num_elements--;
                
                // 检查是否需要缩容
                if (table_size > INITIAL_SIZE && load_factor() < MIN_LOAD_FACTOR) {
                    resize(table_size / 2);
                }
                
                return true;
            }
        }
        
        return false;
    }
    
    size_t size() const override {
        return num_elements;
    }
    
    bool empty() const override {
        return num_elements == 0;
    }
    
    void clear() override {
        for (auto& bucket : table) {
            bucket.clear();
        }
        num_elements = 0;
    }
    
    double load_factor() const override {
        return static_cast<double>(num_elements) / table_size;
    }
    
    // 额外功能
    size_t bucket_count() const {
        return table_size;
    }
    
    size_t bucket_size(size_t index) const {
        if (index >= table_size) {
            throw std::out_of_range("Bucket index out of range");
        }
        return table[index].size();
    }
    
    double collision_rate() const {
        size_t non_empty_buckets = 0;
        for (const auto& bucket : table) {
            if (!bucket.empty()) {
                non_empty_buckets++;
            }
        }
        
        if (num_elements == 0) return 0.0;
        return 1.0 - static_cast<double>(non_empty_buckets) / num_elements;
    }
    
    void print_statistics() const {
        std::cout << "Hash Table Statistics:\n";
        std::cout << "Size: " << num_elements << "\n";
        std::cout << "Bucket count: " << table_size << "\n";
        std::cout << "Load factor: " << load_factor() << "\n";
        std::cout << "Collision rate: " << collision_rate() << "\n";
        
        // 桶大小分布
        std::vector<size_t> bucket_size_dist(10, 0);
        for (const auto& bucket : table) {
            size_t size = bucket.size();
            if (size < bucket_size_dist.size()) {
                bucket_size_dist[size]++;
            } else {
                bucket_size_dist.back()++;
            }
        }
        
        std::cout << "Bucket size distribution:\n";
        for (size_t i = 0; i < bucket_size_dist.size(); ++i) {
            if (i == bucket_size_dist.size() - 1) {
                std::cout << "  >=" << i << ": " << bucket_size_dist[i] << "\n";
            } else {
                std::cout << "  " << i << ": " << bucket_size_dist[i] << "\n";
            }
        }
    }
    
    // 遍历所有元素
    template<typename Func>
    void for_each(Func func) const {
        for (const auto& bucket : table) {
            for (const auto& pair : bucket) {
                func(pair.first, pair.second);
            }
        }
    }
    
    // 内存使用统计
    size_t memory_usage() const {
        size_t total = sizeof(*this);
        total += table.capacity() * sizeof(Bucket);
        
        for (const auto& bucket : table) {
            total += bucket.size() * sizeof(KeyValuePair);
        }
        
        return total;
    }
};

// 开放寻址法哈希表基类
template<typename Key, typename Value, typename Hash = std::hash<Key>>
class OpenAddressingHashTable : public HashTableInterface<Key, Value> {
protected:
    enum class EntryState {
        EMPTY,
        OCCUPIED,
        DELETED
    };
    
    struct Entry {
        Key key;
        Value value;
        EntryState state;
        
        Entry() : state(EntryState::EMPTY) {}
        Entry(const Key& k, const Value& v) : key(k), value(v), state(EntryState::OCCUPIED) {}
    };
    
    std::vector<Entry> table;
    size_t table_size;
    size_t num_elements;
    size_t num_deleted;
    Hash hash_function;
    
    static constexpr double MAX_LOAD_FACTOR = 0.5;
    static constexpr double MIN_LOAD_FACTOR = 0.125;
    static constexpr size_t INITIAL_SIZE = 16;
    
    size_t hash(const Key& key) const {
        return hash_function(key) % table_size;
    }
    
    // 纯虚函数，由子类实现具体的探测策略
    virtual size_t probe(const Key& key, size_t attempt) const = 0;
    
    void resize(size_t new_size) {
        std::vector<Entry> old_table = std::move(table);
        table.clear();
        table.resize(new_size);
        size_t old_size = table_size;
        table_size = new_size;
        num_elements = 0;
        num_deleted = 0;
        
        // 重新插入所有元素
        for (const auto& entry : old_table) {
            if (entry.state == EntryState::OCCUPIED) {
                insert(entry.key, entry.value);
            }
        }
    }
    
public:
    explicit OpenAddressingHashTable(size_t initial_size = INITIAL_SIZE)
        : table(initial_size), table_size(initial_size), num_elements(0), num_deleted(0) {}
    
    void insert(const Key& key, const Value& value) override {
        // 检查是否需要扩容
        if (static_cast<double>(num_elements + num_deleted) / table_size > MAX_LOAD_FACTOR) {
            resize(table_size * 2);
        }
        
        for (size_t attempt = 0; attempt < table_size; ++attempt) {
            size_t index = probe(key, attempt);
            
            if (table[index].state == EntryState::OCCUPIED && table[index].key == key) {
                // 更新现有键的值
                table[index].value = value;
                return;
            }
            
            if (table[index].state != EntryState::OCCUPIED) {
                // 找到空位置或已删除位置
                if (table[index].state == EntryState::DELETED) {
                    num_deleted--;
                }
                table[index] = Entry(key, value);
                num_elements++;
                return;
            }
        }
        
        throw std::runtime_error("Hash table is full");
    }
    
    bool find(const Key& key, Value& value) const override {
        for (size_t attempt = 0; attempt < table_size; ++attempt) {
            size_t index = probe(key, attempt);
            
            if (table[index].state == EntryState::EMPTY) {
                return false;
            }
            
            if (table[index].state == EntryState::OCCUPIED && table[index].key == key) {
                value = table[index].value;
                return true;
            }
        }
        
        return false;
    }
    
    bool remove(const Key& key) override {
        for (size_t attempt = 0; attempt < table_size; ++attempt) {
            size_t index = probe(key, attempt);
            
            if (table[index].state == EntryState::EMPTY) {
                return false;
            }
            
            if (table[index].state == EntryState::OCCUPIED && table[index].key == key) {
                table[index].state = EntryState::DELETED;
                num_elements--;
                num_deleted++;
                
                // 检查是否需要缩容
                if (table_size > INITIAL_SIZE && load_factor() < MIN_LOAD_FACTOR) {
                    resize(table_size / 2);
                }
                
                return true;
            }
        }
        
        return false;
    }
    
    size_t size() const override {
        return num_elements;
    }
    
    bool empty() const override {
        return num_elements == 0;
    }
    
    void clear() override {
        for (auto& entry : table) {
            entry.state = EntryState::EMPTY;
        }
        num_elements = 0;
        num_deleted = 0;
    }
    
    double load_factor() const override {
        return static_cast<double>(num_elements) / table_size;
    }
    
    // 额外功能
    size_t bucket_count() const {
        return table_size;
    }
    
    double deletion_ratio() const {
        return static_cast<double>(num_deleted) / table_size;
    }
    
    void print_statistics() const {
        std::cout << "Open Addressing Hash Table Statistics:\n";
        std::cout << "Size: " << num_elements << "\n";
        std::cout << "Capacity: " << table_size << "\n";
        std::cout << "Load factor: " << load_factor() << "\n";
        std::cout << "Deleted entries: " << num_deleted << "\n";
        std::cout << "Deletion ratio: " << deletion_ratio() << "\n";
        
        // 聚集统计
        size_t max_cluster = 0;
        size_t current_cluster = 0;
        size_t total_clusters = 0;
        
        for (const auto& entry : table) {
            if (entry.state == EntryState::OCCUPIED) {
                current_cluster++;
            } else {
                if (current_cluster > 0) {
                    max_cluster = std::max(max_cluster, current_cluster);
                    total_clusters++;
                    current_cluster = 0;
                }
            }
        }
        
        if (current_cluster > 0) {
            max_cluster = std::max(max_cluster, current_cluster);
            total_clusters++;
        }
        
        std::cout << "Max cluster size: " << max_cluster << "\n";
        std::cout << "Number of clusters: " << total_clusters << "\n";
    }
};

// 线性探测哈希表
template<typename Key, typename Value, typename Hash = std::hash<Key>>
class LinearProbingHashTable : public OpenAddressingHashTable<Key, Value, Hash> {
protected:
    size_t probe(const Key& key, size_t attempt) const override {
        return (this->hash(key) + attempt) % this->table_size;
    }
    
public:
    explicit LinearProbingHashTable(size_t initial_size = 16)
        : OpenAddressingHashTable<Key, Value, Hash>(initial_size) {}
};

// 二次探测哈希表
template<typename Key, typename Value, typename Hash = std::hash<Key>>
class QuadraticProbingHashTable : public OpenAddressingHashTable<Key, Value, Hash> {
protected:
    size_t probe(const Key& key, size_t attempt) const override {
        return (this->hash(key) + attempt * attempt) % this->table_size;
    }
    
public:
    explicit QuadraticProbingHashTable(size_t initial_size = 16)
        : OpenAddressingHashTable<Key, Value, Hash>(initial_size) {}
};

// 双重哈希表
template<typename Key, typename Value, typename Hash1 = std::hash<Key>, typename Hash2 = std::hash<Key>>
class DoubleHashingTable : public OpenAddressingHashTable<Key, Value, Hash1> {
private:
    Hash2 hash2_function;
    
    size_t hash2(const Key& key) const {
        size_t h = hash2_function(key) % (this->table_size - 1);
        return h == 0 ? 1 : h; // 确保步长不为0
    }
    
protected:
    size_t probe(const Key& key, size_t attempt) const override {
        return (this->hash(key) + attempt * hash2(key)) % this->table_size;
    }
    
public:
    explicit DoubleHashingTable(size_t initial_size = 16)
        : OpenAddressingHashTable<Key, Value, Hash1>(initial_size) {}
};

// 哈希表性能测试器
template<typename HashTable>
class HashTableTester {
public:
    static void performance_test(HashTable& table, const std::vector<std::pair<int, int>>& data) {
        auto start = std::chrono::high_resolution_clock::now();
        
        // 插入测试
        for (const auto& pair : data) {
            table.insert(pair.first, pair.second);
        }
        
        auto insert_end = std::chrono::high_resolution_clock::now();
        
        // 查找测试
        int found_count = 0;
        for (const auto& pair : data) {
            int value;
            if (table.find(pair.first, value)) {
                found_count++;
            }
        }
        
        auto find_end = std::chrono::high_resolution_clock::now();
        
        // 删除测试
        for (size_t i = 0; i < data.size() / 2; ++i) {
            table.remove(data[i].first);
        }
        
        auto remove_end = std::chrono::high_resolution_clock::now();
        
        auto insert_time = std::chrono::duration_cast<std::chrono::microseconds>(insert_end - start);
        auto find_time = std::chrono::duration_cast<std::chrono::microseconds>(find_end - insert_end);
        auto remove_time = std::chrono::duration_cast<std::chrono::microseconds>(remove_end - find_end);
        
        std::cout << "Performance Test Results:\n";
        std::cout << "Insert time: " << insert_time.count() << " microseconds\n";
        std::cout << "Find time: " << find_time.count() << " microseconds\n";
        std::cout << "Remove time: " << remove_time.count() << " microseconds\n";
        std::cout << "Found " << found_count << "/" << data.size() << " elements\n";
    }
};

} // namespace hash_table

#endif // HASH_TABLE_H