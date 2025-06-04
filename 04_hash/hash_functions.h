#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <functional>

namespace hash_functions {

// 基础哈希函数类
template<typename T>
class HashFunction {
public:
    virtual ~HashFunction() = default;
    virtual size_t operator()(const T& key) const = 0;
    virtual size_t hash(const T& key) const { return (*this)(key); }
};

// 除法哈希
template<typename T>
class DivisionHash : public HashFunction<T> {
private:
    size_t table_size;
    
public:
    explicit DivisionHash(size_t size) : table_size(size) {}
    
    size_t operator()(const T& key) const override {
        return std::hash<T>{}(key) % table_size;
    }
};

// 乘法哈希
template<typename T>
class MultiplicationHash : public HashFunction<T> {
private:
    size_t table_size;
    static constexpr double A = 0.6180339887; // (√5 - 1) / 2
    
public:
    explicit MultiplicationHash(size_t size) : table_size(size) {}
    
    size_t operator()(const T& key) const override {
        size_t hash_value = std::hash<T>{}(key);
        double temp = hash_value * A;
        temp = temp - std::floor(temp);
        return static_cast<size_t>(table_size * temp);
    }
};

// 通用哈希族
template<typename T>
class UniversalHash : public HashFunction<T> {
private:
    size_t a, b, p, m;
    
    // 找到下一个质数
    static size_t next_prime(size_t n) {
        while (!is_prime(n)) {
            n++;
        }
        return n;
    }
    
    static bool is_prime(size_t n) {
        if (n < 2) return false;
        if (n == 2) return true;
        if (n % 2 == 0) return false;
        
        for (size_t i = 3; i * i <= n; i += 2) {
            if (n % i == 0) return false;
        }
        return true;
    }
    
public:
    explicit UniversalHash(size_t table_size) : m(table_size) {
        p = next_prime(table_size * 2);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<size_t> dis_a(1, p - 1);
        std::uniform_int_distribution<size_t> dis_b(0, p - 1);
        
        a = dis_a(gen);
        b = dis_b(gen);
    }
    
    size_t operator()(const T& key) const override {
        size_t hash_value = std::hash<T>{}(key);
        return ((a * hash_value + b) % p) % m;
    }
};

// 字符串哈希函数
namespace string_hash {

// DJB2 哈希
class DJB2Hash : public HashFunction<std::string> {
public:
    size_t operator()(const std::string& str) const override {
        size_t hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
        }
        return hash;
    }
};

// SDBM 哈希
class SDBMHash : public HashFunction<std::string> {
public:
    size_t operator()(const std::string& str) const override {
        size_t hash = 0;
        for (char c : str) {
            hash = static_cast<unsigned char>(c) + (hash << 6) + (hash << 16) - hash;
        }
        return hash;
    }
};

// FNV-1a 哈希
class FNV1aHash : public HashFunction<std::string> {
private:
    static constexpr size_t FNV_OFFSET_BASIS = 2166136261u;
    static constexpr size_t FNV_PRIME = 16777619u;
    
public:
    size_t operator()(const std::string& str) const override {
        size_t hash = FNV_OFFSET_BASIS;
        for (char c : str) {
            hash ^= static_cast<unsigned char>(c);
            hash *= FNV_PRIME;
        }
        return hash;
    }
};

// Murmur3 哈希（简化版）
class Murmur3Hash : public HashFunction<std::string> {
private:
    static constexpr uint32_t C1 = 0xcc9e2d51;
    static constexpr uint32_t C2 = 0x1b873593;
    static constexpr uint32_t R1 = 15;
    static constexpr uint32_t R2 = 13;
    static constexpr uint32_t M = 5;
    static constexpr uint32_t N = 0xe6546b64;
    
    static uint32_t rotl32(uint32_t x, int8_t r) {
        return (x << r) | (x >> (32 - r));
    }
    
    static uint32_t fmix32(uint32_t h) {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }
    
public:
    size_t operator()(const std::string& str) const override {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(str.c_str());
        const int len = static_cast<int>(str.length());
        const uint32_t seed = 0;
        
        const int nblocks = len / 4;
        uint32_t h1 = seed;
        
        // 处理4字节块
        for (int i = 0; i < nblocks; i++) {
            uint32_t k1;
            std::memcpy(&k1, data + i * 4, sizeof(k1));
            
            k1 *= C1;
            k1 = rotl32(k1, R1);
            k1 *= C2;
            
            h1 ^= k1;
            h1 = rotl32(h1, R2);
            h1 = h1 * M + N;
        }
        
        // 处理剩余字节
        const uint8_t* tail = data + nblocks * 4;
        uint32_t k1 = 0;
        
        switch (len & 3) {
            case 3: k1 ^= tail[2] << 16;
            case 2: k1 ^= tail[1] << 8;
            case 1: k1 ^= tail[0];
                k1 *= C1;
                k1 = rotl32(k1, R1);
                k1 *= C2;
                h1 ^= k1;
        }
        
        h1 ^= len;
        h1 = fmix32(h1);
        
        return h1;
    }
};

// 滚动哈希（Rabin-Karp）
class RollingHash {
private:
    static constexpr size_t BASE = 256;
    static constexpr size_t MOD = 1000000007;
    
    size_t hash_value;
    size_t power;
    size_t length;
    
public:
    RollingHash() : hash_value(0), power(1), length(0) {}
    
    // 初始化哈希值
    void init(const std::string& str) {
        hash_value = 0;
        power = 1;
        length = str.length();
        
        for (char c : str) {
            hash_value = (hash_value * BASE + static_cast<unsigned char>(c)) % MOD;
        }
        
        for (size_t i = 1; i < length; i++) {
            power = (power * BASE) % MOD;
        }
    }
    
    // 滚动更新哈希值
    void roll(char old_char, char new_char) {
        // 移除旧字符
        hash_value = (hash_value + MOD - (static_cast<unsigned char>(old_char) * power) % MOD) % MOD;
        // 添加新字符
        hash_value = (hash_value * BASE + static_cast<unsigned char>(new_char)) % MOD;
    }
    
    size_t get_hash() const {
        return hash_value;
    }
};

} // namespace string_hash

// 数值哈希函数
namespace numeric_hash {

// 整数哈希
class IntegerHash : public HashFunction<int> {
public:
    size_t operator()(const int& key) const override {
        uint32_t x = static_cast<uint32_t>(key);
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        return x;
    }
};

// 64位整数哈希
class LongHash : public HashFunction<long long> {
public:
    size_t operator()(const long long& key) const override {
        uint64_t x = static_cast<uint64_t>(key);
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        x = x ^ (x >> 31);
        return x;
    }
};

// 浮点数哈希
class FloatHash : public HashFunction<double> {
public:
    size_t operator()(const double& key) const override {
        if (key == 0.0) return 0;
        
        uint64_t bits;
        std::memcpy(&bits, &key, sizeof(bits));
        return std::hash<uint64_t>{}(bits);
    }
};

} // namespace numeric_hash

// 复合类型哈希
namespace composite_hash {

// 对哈希
template<typename T1, typename T2>
class PairHash : public HashFunction<std::pair<T1, T2>> {
public:
    size_t operator()(const std::pair<T1, T2>& p) const override {
        size_t h1 = std::hash<T1>{}(p.first);
        size_t h2 = std::hash<T2>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

// 向量哈希
template<typename T>
class VectorHash : public HashFunction<std::vector<T>> {
public:
    size_t operator()(const std::vector<T>& vec) const override {
        size_t hash = 0;
        for (const auto& element : vec) {
            hash ^= std::hash<T>{}(element) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

} // namespace composite_hash

// 哈希函数质量测试
class HashQualityTester {
public:
    template<typename T, typename HashFunc>
    static double test_distribution(const HashFunc& hash_func, 
                                   const std::vector<T>& keys, 
                                   size_t table_size) {
        std::vector<int> bucket_counts(table_size, 0);
        
        for (const auto& key : keys) {
            size_t hash_value = hash_func(key) % table_size;
            bucket_counts[hash_value]++;
        }
        
        // 计算卡方统计量
        double expected = static_cast<double>(keys.size()) / table_size;
        double chi_square = 0.0;
        
        for (int count : bucket_counts) {
            double diff = count - expected;
            chi_square += (diff * diff) / expected;
        }
        
        return chi_square;
    }
    
    template<typename T, typename HashFunc>
    static double test_avalanche_effect(const HashFunc& hash_func, 
                                       const std::vector<T>& keys) {
        int total_bits_changed = 0;
        int total_comparisons = 0;
        
        for (size_t i = 0; i < keys.size() - 1; i++) {
            size_t hash1 = hash_func(keys[i]);
            size_t hash2 = hash_func(keys[i + 1]);
            
            size_t xor_result = hash1 ^ hash2;
            total_bits_changed += __builtin_popcountll(xor_result);
            total_comparisons++;
        }
        
        return static_cast<double>(total_bits_changed) / (total_comparisons * sizeof(size_t) * 8);
    }
};

// 哈希函数工厂
class HashFunctionFactory {
public:
    enum class HashType {
        DIVISION,
        MULTIPLICATION,
        UNIVERSAL,
        DJB2,
        FNV1A,
        MURMUR3
    };
    
    template<typename T>
    static std::unique_ptr<HashFunction<T>> create(HashType type, size_t table_size = 1000) {
        switch (type) {
            case HashType::DIVISION:
                return std::make_unique<DivisionHash<T>>(table_size);
            case HashType::MULTIPLICATION:
                return std::make_unique<MultiplicationHash<T>>(table_size);
            case HashType::UNIVERSAL:
                return std::make_unique<UniversalHash<T>>(table_size);
            default:
                return std::make_unique<DivisionHash<T>>(table_size);
        }
    }
    
    static std::unique_ptr<HashFunction<std::string>> create_string_hash(HashType type) {
        switch (type) {
            case HashType::DJB2:
                return std::make_unique<string_hash::DJB2Hash>();
            case HashType::FNV1A:
                return std::make_unique<string_hash::FNV1aHash>();
            case HashType::MURMUR3:
                return std::make_unique<string_hash::Murmur3Hash>();
            default:
                return std::make_unique<string_hash::DJB2Hash>();
        }
    }
};

} // namespace hash_functions

#endif // HASH_FUNCTIONS_H