#ifndef ADVANCED_HASH_H
#define ADVANCED_HASH_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <functional>
#include <random>
#include <algorithm>
#include <cmath>
#include <bitset>
#include <memory>
#include <list>
#include <chrono>

namespace advanced_hash {

// 一致性哈希
class ConsistentHash {
private:
    std::map<size_t, std::string> ring;
    size_t virtual_nodes;
    std::hash<std::string> hash_function;
    
    size_t hash(const std::string& key) const {
        return hash_function(key);
    }
    
public:
    explicit ConsistentHash(size_t virtual_node_count = 150) 
        : virtual_nodes(virtual_node_count) {}
    
    void add_node(const std::string& node) {
        for (size_t i = 0; i < virtual_nodes; ++i) {
            std::string virtual_node = node + "#" + std::to_string(i);
            size_t hash_value = hash(virtual_node);
            ring[hash_value] = node;
        }
    }
    
    void remove_node(const std::string& node) {
        for (size_t i = 0; i < virtual_nodes; ++i) {
            std::string virtual_node = node + "#" + std::to_string(i);
            size_t hash_value = hash(virtual_node);
            ring.erase(hash_value);
        }
    }
    
    std::string get_node(const std::string& key) const {
        if (ring.empty()) {
            return "";
        }
        
        size_t hash_value = hash(key);
        auto it = ring.lower_bound(hash_value);
        
        if (it == ring.end()) {
            it = ring.begin();
        }
        
        return it->second;
    }
    
    std::vector<std::string> get_nodes(const std::string& key, size_t count) const {
        std::vector<std::string> result;
        if (ring.empty() || count == 0) {
            return result;
        }
        
        size_t hash_value = hash(key);
        auto it = ring.lower_bound(hash_value);
        
        std::unordered_map<std::string, bool> seen;
        
        while (result.size() < count && seen.size() < get_unique_nodes().size()) {
            if (it == ring.end()) {
                it = ring.begin();
            }
            
            if (seen.find(it->second) == seen.end()) {
                result.push_back(it->second);
                seen[it->second] = true;
            }
            
            ++it;
        }
        
        return result;
    }
    
    std::vector<std::string> get_unique_nodes() const {
        std::unordered_map<std::string, bool> unique_nodes;
        for (const auto& pair : ring) {
            unique_nodes[pair.second] = true;
        }
        
        std::vector<std::string> result;
        for (const auto& pair : unique_nodes) {
            result.push_back(pair.first);
        }
        
        return result;
    }
    
    size_t size() const {
        return get_unique_nodes().size();
    }
    
    void print_ring() const {
        std::cout << "Consistent Hash Ring:\n";
        for (const auto& pair : ring) {
            std::cout << "Hash: " << pair.first << " -> Node: " << pair.second << "\n";
        }
    }
};

// 布隆过滤器
class BloomFilter {
private:
    std::vector<bool> bit_array;
    size_t size;
    size_t hash_count;
    size_t inserted_elements;
    
    std::vector<std::function<size_t(const std::string&)>> hash_functions;
    
    void initialize_hash_functions() {
        std::random_device rd;
        std::mt19937 gen(rd());
        
        for (size_t i = 0; i < hash_count; ++i) {
            size_t seed = gen();
            hash_functions.push_back([this, seed](const std::string& item) {
                std::hash<std::string> hasher;
                return (hasher(item) ^ seed) % this->size;
            });
        }
    }
    
public:
    BloomFilter(size_t expected_elements, double false_positive_rate = 0.01) {
        // 计算最优参数
        size = static_cast<size_t>(-expected_elements * std::log(false_positive_rate) / (std::log(2) * std::log(2)));
        hash_count = static_cast<size_t>(size * std::log(2) / expected_elements);
        
        bit_array.resize(size, false);
        inserted_elements = 0;
        
        initialize_hash_functions();
    }
    
    BloomFilter(size_t bit_array_size, size_t hash_function_count) 
        : size(bit_array_size), hash_count(hash_function_count), inserted_elements(0) {
        bit_array.resize(size, false);
        initialize_hash_functions();
    }
    
    void insert(const std::string& item) {
        for (const auto& hash_func : hash_functions) {
            size_t hash_value = hash_func(item);
            bit_array[hash_value] = true;
        }
        inserted_elements++;
    }
    
    bool might_contain(const std::string& item) const {
        for (const auto& hash_func : hash_functions) {
            size_t hash_value = hash_func(item);
            if (!bit_array[hash_value]) {
                return false;
            }
        }
        return true;
    }
    
    void clear() {
        std::fill(bit_array.begin(), bit_array.end(), false);
        inserted_elements = 0;
    }
    
    double current_false_positive_rate() const {
        if (inserted_elements == 0) return 0.0;
        
        size_t set_bits = std::count(bit_array.begin(), bit_array.end(), true);
        double ratio = static_cast<double>(set_bits) / size;
        return std::pow(ratio, hash_count);
    }
    
    size_t get_size() const { return size; }
    size_t get_hash_count() const { return hash_count; }
    size_t get_inserted_count() const { return inserted_elements; }
    
    void print_statistics() const {
        size_t set_bits = std::count(bit_array.begin(), bit_array.end(), true);
        std::cout << "Bloom Filter Statistics:\n";
        std::cout << "Size: " << size << " bits\n";
        std::cout << "Hash functions: " << hash_count << "\n";
        std::cout << "Inserted elements: " << inserted_elements << "\n";
        std::cout << "Set bits: " << set_bits << " (" << (100.0 * set_bits / size) << "%)\n";
        std::cout << "Estimated false positive rate: " << current_false_positive_rate() << "\n";
    }
};

// 计数布隆过滤器
class CountingBloomFilter {
private:
    std::vector<uint8_t> counters;
    size_t size;
    size_t hash_count;
    size_t inserted_elements;
    
    std::vector<std::function<size_t(const std::string&)>> hash_functions;
    
    void initialize_hash_functions() {
        std::random_device rd;
        std::mt19937 gen(rd());
        
        for (size_t i = 0; i < hash_count; ++i) {
            size_t seed = gen();
            hash_functions.push_back([this, seed](const std::string& item) {
                std::hash<std::string> hasher;
                return (hasher(item) ^ seed) % this->size;
            });
        }
    }
    
public:
    CountingBloomFilter(size_t expected_elements, double false_positive_rate = 0.01) {
        size = static_cast<size_t>(-expected_elements * std::log(false_positive_rate) / (std::log(2) * std::log(2)));
        hash_count = static_cast<size_t>(size * std::log(2) / expected_elements);
        
        counters.resize(size, 0);
        inserted_elements = 0;
        
        initialize_hash_functions();
    }
    
    void insert(const std::string& item) {
        for (const auto& hash_func : hash_functions) {
            size_t hash_value = hash_func(item);
            if (counters[hash_value] < 255) { // 防止溢出
                counters[hash_value]++;
            }
        }
        inserted_elements++;
    }
    
    bool remove(const std::string& item) {
        // 首先检查是否可能存在
        if (!might_contain(item)) {
            return false;
        }
        
        // 减少计数器
        for (const auto& hash_func : hash_functions) {
            size_t hash_value = hash_func(item);
            if (counters[hash_value] > 0) {
                counters[hash_value]--;
            }
        }
        
        if (inserted_elements > 0) {
            inserted_elements--;
        }
        
        return true;
    }
    
    bool might_contain(const std::string& item) const {
        for (const auto& hash_func : hash_functions) {
            size_t hash_value = hash_func(item);
            if (counters[hash_value] == 0) {
                return false;
            }
        }
        return true;
    }
    
    void clear() {
        std::fill(counters.begin(), counters.end(), 0);
        inserted_elements = 0;
    }
    
    size_t get_size() const { return size; }
    size_t get_hash_count() const { return hash_count; }
    size_t get_inserted_count() const { return inserted_elements; }
};

// 可扩展布隆过滤器
class ScalableBloomFilter {
private:
    std::vector<std::unique_ptr<BloomFilter>> filters;
    size_t initial_capacity;
    double false_positive_rate;
    double growth_factor;
    size_t total_elements;
    
public:
    ScalableBloomFilter(size_t initial_cap = 1000, double fp_rate = 0.01, double growth = 2.0)
        : initial_capacity(initial_cap), false_positive_rate(fp_rate), 
          growth_factor(growth), total_elements(0) {
        filters.push_back(std::make_unique<BloomFilter>(initial_capacity, false_positive_rate));
    }
    
    void insert(const std::string& item) {
        // 检查当前过滤器是否已满
        auto& current_filter = filters.back();
        if (current_filter->get_inserted_count() >= initial_capacity * std::pow(growth_factor, filters.size() - 1)) {
            // 创建新的过滤器
            size_t new_capacity = static_cast<size_t>(initial_capacity * std::pow(growth_factor, filters.size()));
            double new_fp_rate = false_positive_rate / filters.size();
            filters.push_back(std::make_unique<BloomFilter>(new_capacity, new_fp_rate));
        }
        
        filters.back()->insert(item);
        total_elements++;
    }
    
    bool might_contain(const std::string& item) const {
        for (const auto& filter : filters) {
            if (filter->might_contain(item)) {
                return true;
            }
        }
        return false;
    }
    
    void clear() {
        filters.clear();
        filters.push_back(std::make_unique<BloomFilter>(initial_capacity, false_positive_rate));
        total_elements = 0;
    }
    
    size_t get_filter_count() const { return filters.size(); }
    size_t get_total_elements() const { return total_elements; }
    
    void print_statistics() const {
        std::cout << "Scalable Bloom Filter Statistics:\n";
        std::cout << "Number of filters: " << filters.size() << "\n";
        std::cout << "Total elements: " << total_elements << "\n";
        
        for (size_t i = 0; i < filters.size(); ++i) {
            std::cout << "Filter " << i << ": " << filters[i]->get_inserted_count() 
                      << " elements, FP rate: " << filters[i]->current_false_positive_rate() << "\n";
        }
    }
};

// HyperLogLog（基数估计）
class HyperLogLog {
private:
    std::vector<uint8_t> buckets;
    size_t bucket_count;
    size_t bucket_bits;
    
    static constexpr double ALPHA_16 = 0.673;
    static constexpr double ALPHA_32 = 0.697;
    static constexpr double ALPHA_64 = 0.709;
    
    double get_alpha() const {
        switch (bucket_count) {
            case 16: return ALPHA_16;
            case 32: return ALPHA_32;
            case 64: return ALPHA_64;
            default: return 0.7213 / (1.0 + 1.079 / bucket_count);
        }
    }
    
    uint8_t leading_zeros(uint64_t hash_value, size_t start_bit) const {
        uint64_t mask = 1ULL << (63 - start_bit);
        uint8_t zeros = 1;
        
        while (zeros <= 64 - start_bit && !(hash_value & mask)) {
            zeros++;
            mask >>= 1;
        }
        
        return std::min(zeros, static_cast<uint8_t>(64 - start_bit));
    }
    
public:
    explicit HyperLogLog(size_t precision = 12) {
        bucket_bits = precision;
        bucket_count = 1ULL << precision;
        buckets.resize(bucket_count, 0);
    }
    
    void add(const std::string& item) {
        std::hash<std::string> hasher;
        uint64_t hash_value = hasher(item);
        
        // 使用前bucket_bits位作为桶索引
        size_t bucket_index = hash_value >> (64 - bucket_bits);
        
        // 计算剩余位的前导零个数
        uint8_t leading_zero_count = leading_zeros(hash_value, bucket_bits) + 1;
        
        // 更新桶的最大值
        buckets[bucket_index] = std::max(buckets[bucket_index], leading_zero_count);
    }
    
    size_t estimate_cardinality() const {
        double raw_estimate = get_alpha() * bucket_count * bucket_count;
        
        double sum = 0.0;
        for (uint8_t bucket_value : buckets) {
            sum += std::pow(2.0, -bucket_value);
        }
        
        raw_estimate /= sum;
        
        // 小范围修正
        if (raw_estimate <= 2.5 * bucket_count) {
            size_t zero_buckets = std::count(buckets.begin(), buckets.end(), 0);
            if (zero_buckets != 0) {
                return bucket_count * std::log(static_cast<double>(bucket_count) / zero_buckets);
            }
        }
        
        // 大范围修正
        if (raw_estimate <= (1.0 / 30.0) * (1ULL << 32)) {
            return raw_estimate;
        } else {
            return -1 * (1ULL << 32) * std::log(1.0 - raw_estimate / (1ULL << 32));
        }
    }
    
    void merge(const HyperLogLog& other) {
        if (bucket_count != other.bucket_count) {
            throw std::invalid_argument("Cannot merge HyperLogLog with different bucket counts");
        }
        
        for (size_t i = 0; i < bucket_count; ++i) {
            buckets[i] = std::max(buckets[i], other.buckets[i]);
        }
    }
    
    void clear() {
        std::fill(buckets.begin(), buckets.end(), 0);
    }
    
    size_t get_bucket_count() const { return bucket_count; }
    
    void print_statistics() const {
        std::cout << "HyperLogLog Statistics:\n";
        std::cout << "Bucket count: " << bucket_count << "\n";
        std::cout << "Estimated cardinality: " << estimate_cardinality() << "\n";
        
        size_t zero_buckets = std::count(buckets.begin(), buckets.end(), 0);
        std::cout << "Zero buckets: " << zero_buckets << " (" 
                  << (100.0 * zero_buckets / bucket_count) << "%)\n";
    }
};

// MinHash（相似性估计）
class MinHash {
private:
    std::vector<size_t> hash_values;
    size_t hash_count;
    std::vector<std::function<size_t(const std::string&)>> hash_functions;
    
    void initialize_hash_functions() {
        std::random_device rd;
        std::mt19937 gen(rd());
        
        for (size_t i = 0; i < hash_count; ++i) {
            size_t seed = gen();
            hash_functions.push_back([seed](const std::string& item) {
                std::hash<std::string> hasher;
                return hasher(item) ^ seed;
            });
        }
    }
    
public:
    explicit MinHash(size_t num_hashes = 128) : hash_count(num_hashes) {
        hash_values.resize(hash_count, SIZE_MAX);
        initialize_hash_functions();
    }
    
    void add(const std::string& item) {
        for (size_t i = 0; i < hash_count; ++i) {
            size_t hash_value = hash_functions[i](item);
            hash_values[i] = std::min(hash_values[i], hash_value);
        }
    }
    
    double jaccard_similarity(const MinHash& other) const {
        if (hash_count != other.hash_count) {
            throw std::invalid_argument("MinHash objects must have the same number of hash functions");
        }
        
        size_t matches = 0;
        for (size_t i = 0; i < hash_count; ++i) {
            if (hash_values[i] == other.hash_values[i]) {
                matches++;
            }
        }
        
        return static_cast<double>(matches) / hash_count;
    }
    
    void merge(const MinHash& other) {
        if (hash_count != other.hash_count) {
            throw std::invalid_argument("MinHash objects must have the same number of hash functions");
        }
        
        for (size_t i = 0; i < hash_count; ++i) {
            hash_values[i] = std::min(hash_values[i], other.hash_values[i]);
        }
    }
    
    void clear() {
        std::fill(hash_values.begin(), hash_values.end(), SIZE_MAX);
    }
    
    size_t get_hash_count() const { return hash_count; }
    
    const std::vector<size_t>& get_signature() const { return hash_values; }
};

// 局部敏感哈希（LSH）
class LSH {
private:
    struct HashFamily {
        std::vector<std::vector<double>> random_vectors;
        size_t dimension;
        size_t hash_count;
        
        HashFamily(size_t dim, size_t num_hashes) : dimension(dim), hash_count(num_hashes) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::normal_distribution<double> dist(0.0, 1.0);
            
            random_vectors.resize(num_hashes, std::vector<double>(dimension));
            for (auto& vec : random_vectors) {
                for (double& val : vec) {
                    val = dist(gen);
                }
            }
        }
        
        std::vector<int> hash(const std::vector<double>& point) const {
            std::vector<int> result(hash_count);
            for (size_t i = 0; i < hash_count; ++i) {
                double dot_product = 0.0;
                for (size_t j = 0; j < dimension; ++j) {
                    dot_product += random_vectors[i][j] * point[j];
                }
                result[i] = (dot_product >= 0) ? 1 : 0;
            }
            return result;
        }
    };
    
    std::vector<HashFamily> hash_families;
    std::vector<std::unordered_map<std::string, std::vector<size_t>>> hash_tables;
    std::vector<std::vector<double>> data_points;
    size_t dimension;
    
    std::string vector_to_string(const std::vector<int>& vec) const {
        std::string result;
        for (int val : vec) {
            result += std::to_string(val);
        }
        return result;
    }
    
public:
    LSH(size_t dim, size_t num_tables = 10, size_t hash_per_table = 10) 
        : dimension(dim) {
        hash_families.reserve(num_tables);
        hash_tables.resize(num_tables);
        
        for (size_t i = 0; i < num_tables; ++i) {
            hash_families.emplace_back(dimension, hash_per_table);
        }
    }
    
    size_t add_point(const std::vector<double>& point) {
        if (point.size() != dimension) {
            throw std::invalid_argument("Point dimension mismatch");
        }
        
        size_t point_id = data_points.size();
        data_points.push_back(point);
        
        // 在每个哈希表中插入点
        for (size_t i = 0; i < hash_families.size(); ++i) {
            auto hash_signature = hash_families[i].hash(point);
            std::string hash_key = vector_to_string(hash_signature);
            hash_tables[i][hash_key].push_back(point_id);
        }
        
        return point_id;
    }
    
    std::vector<size_t> query(const std::vector<double>& query_point) const {
        if (query_point.size() != dimension) {
            throw std::invalid_argument("Query point dimension mismatch");
        }
        
        std::unordered_map<size_t, size_t> candidate_counts;
        
        // 在每个哈希表中查找候选点
        for (size_t i = 0; i < hash_families.size(); ++i) {
            auto hash_signature = hash_families[i].hash(query_point);
            std::string hash_key = vector_to_string(hash_signature);
            
            auto it = hash_tables[i].find(hash_key);
            if (it != hash_tables[i].end()) {
                for (size_t point_id : it->second) {
                    candidate_counts[point_id]++;
                }
            }
        }
        
        // 按出现频率排序候选点
        std::vector<std::pair<size_t, size_t>> candidates;
        for (const auto& pair : candidate_counts) {
            candidates.emplace_back(pair.second, pair.first); // (count, point_id)
        }
        
        std::sort(candidates.rbegin(), candidates.rend());
        
        std::vector<size_t> result;
        for (const auto& candidate : candidates) {
            result.push_back(candidate.second);
        }
        
        return result;
    }
    
    const std::vector<double>& get_point(size_t point_id) const {
        if (point_id >= data_points.size()) {
            throw std::out_of_range("Point ID out of range");
        }
        return data_points[point_id];
    }
    
    size_t get_point_count() const { return data_points.size(); }
    size_t get_dimension() const { return dimension; }
    size_t get_table_count() const { return hash_families.size(); }
};

} // namespace advanced_hash

#endif // ADVANCED_HASH_H