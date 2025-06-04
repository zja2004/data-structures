#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <algorithm>
#include <queue>
#include <set>

using namespace std;

// 字典树节点（数组实现）
struct TrieNode {
    static const int ALPHABET_SIZE = 26;
    TrieNode* children[ALPHABET_SIZE];
    bool is_end_of_word;
    int word_count;  // 以此节点结尾的单词数量
    int prefix_count;  // 经过此节点的单词数量
    
    TrieNode() : is_end_of_word(false), word_count(0), prefix_count(0) {
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            children[i] = nullptr;
        }
    }
    
    ~TrieNode() {
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            delete children[i];
        }
    }
};

// 字典树节点（哈希表实现）
struct TrieNodeMap {
    unordered_map<char, TrieNodeMap*> children;
    bool is_end_of_word;
    int word_count;
    int prefix_count;
    
    TrieNodeMap() : is_end_of_word(false), word_count(0), prefix_count(0) {}
    
    ~TrieNodeMap() {
        for (auto& pair : children) {
            delete pair.second;
        }
    }
};

// 标准字典树类（数组实现）
class Trie {
private:
    TrieNode* root;
    int total_words;
    
    // 字符转索引
    int char_to_index(char c) const {
        return c - 'a';
    }
    
    // 索引转字符
    char index_to_char(int index) const {
        return 'a' + index;
    }
    
    // 递归删除节点
    bool delete_helper(TrieNode* node, const string& word, int index) {
        if (index == word.length()) {
            // 到达单词末尾
            if (!node->is_end_of_word) {
                return false;  // 单词不存在
            }
            
            node->is_end_of_word = false;
            node->word_count--;
            total_words--;
            
            // 如果没有子节点，可以删除此节点
            for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
                if (node->children[i]) {
                    return false;
                }
            }
            return true;
        }
        
        int char_index = char_to_index(word[index]);
        TrieNode* child = node->children[char_index];
        
        if (!child) {
            return false;  // 单词不存在
        }
        
        bool should_delete_child = delete_helper(child, word, index + 1);
        
        if (should_delete_child) {
            delete child;
            node->children[char_index] = nullptr;
            node->prefix_count--;
            
            // 检查当前节点是否可以删除
            if (!node->is_end_of_word) {
                for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
                    if (node->children[i]) {
                        return false;
                    }
                }
                return true;
            }
        }
        
        return false;
    }
    
    // 收集所有以指定前缀开始的单词
    void collect_words(TrieNode* node, const string& prefix, vector<string>& result) const {
        if (!node) return;
        
        if (node->is_end_of_word) {
            result.push_back(prefix);
        }
        
        for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                collect_words(node->children[i], prefix + index_to_char(i), result);
            }
        }
    }
    
    // 收集有限数量的单词（用于自动补全）
    void collect_words_limited(TrieNode* node, const string& prefix, 
                              vector<string>& result, int limit) const {
        if (!node || result.size() >= limit) return;
        
        if (node->is_end_of_word) {
            result.push_back(prefix);
            if (result.size() >= limit) return;
        }
        
        for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                collect_words_limited(node->children[i], prefix + index_to_char(i), result, limit);
                if (result.size() >= limit) return;
            }
        }
    }
    
    // 计算节点数量
    int count_nodes(TrieNode* node) const {
        if (!node) return 0;
        
        int count = 1;
        for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
            count += count_nodes(node->children[i]);
        }
        return count;
    }
    
    // 计算最大深度
    int max_depth(TrieNode* node) const {
        if (!node) return 0;
        
        int max_child_depth = 0;
        for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                max_child_depth = max(max_child_depth, max_depth(node->children[i]));
            }
        }
        return 1 + max_child_depth;
    }
    
    // 打印树结构
    void print_tree(TrieNode* node, const string& prefix, int depth) const {
        if (!node) return;
        
        // 打印当前节点信息
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }
        cout << "[" << (prefix.empty() ? "ROOT" : string(1, prefix.back())) << "]";
        if (node->is_end_of_word) {
            cout << " (END: " << node->word_count << ")";
        }
        cout << " (PREFIX: " << node->prefix_count << ")" << endl;
        
        // 递归打印子节点
        for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                print_tree(node->children[i], prefix + index_to_char(i), depth + 1);
            }
        }
    }
    
    // 复制树结构
    TrieNode* copy_tree(TrieNode* node) const {
        if (!node) return nullptr;
        
        TrieNode* new_node = new TrieNode();
        new_node->is_end_of_word = node->is_end_of_word;
        new_node->word_count = node->word_count;
        new_node->prefix_count = node->prefix_count;
        
        for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
            new_node->children[i] = copy_tree(node->children[i]);
        }
        
        return new_node;
    }
    
public:
    // 构造函数
    Trie() : root(new TrieNode()), total_words(0) {}
    
    // 拷贝构造函数
    Trie(const Trie& other) : total_words(other.total_words) {
        root = copy_tree(other.root);
    }
    
    // 赋值操作符
    Trie& operator=(const Trie& other) {
        if (this != &other) {
            delete root;
            root = copy_tree(other.root);
            total_words = other.total_words;
        }
        return *this;
    }
    
    // 移动构造函数
    Trie(Trie&& other) noexcept : root(other.root), total_words(other.total_words) {
        other.root = nullptr;
        other.total_words = 0;
    }
    
    // 移动赋值操作符
    Trie& operator=(Trie&& other) noexcept {
        if (this != &other) {
            delete root;
            root = other.root;
            total_words = other.total_words;
            other.root = nullptr;
            other.total_words = 0;
        }
        return *this;
    }
    
    // 析构函数
    ~Trie() {
        delete root;
    }
    
    // 插入单词
    void insert(const string& word) {
        if (word.empty()) return;
        
        TrieNode* current = root;
        
        for (char c : word) {
            if (c < 'a' || c > 'z') {
                throw invalid_argument("Only lowercase letters are supported");
            }
            
            int index = char_to_index(c);
            
            if (!current->children[index]) {
                current->children[index] = new TrieNode();
            }
            
            current = current->children[index];
            current->prefix_count++;
        }
        
        if (!current->is_end_of_word) {
            current->is_end_of_word = true;
            total_words++;
        }
        current->word_count++;
    }
    
    // 查找单词
    bool search(const string& word) const {
        if (word.empty()) return false;
        
        TrieNode* current = root;
        
        for (char c : word) {
            if (c < 'a' || c > 'z') return false;
            
            int index = char_to_index(c);
            
            if (!current->children[index]) {
                return false;
            }
            
            current = current->children[index];
        }
        
        return current->is_end_of_word;
    }
    
    // 检查前缀是否存在
    bool starts_with(const string& prefix) const {
        if (prefix.empty()) return true;
        
        TrieNode* current = root;
        
        for (char c : prefix) {
            if (c < 'a' || c > 'z') return false;
            
            int index = char_to_index(c);
            
            if (!current->children[index]) {
                return false;
            }
            
            current = current->children[index];
        }
        
        return true;
    }
    
    // 删除单词
    bool erase(const string& word) {
        if (word.empty()) return false;
        
        return delete_helper(root, word, 0);
    }
    
    // 获取单词数量
    int get_word_count(const string& word) const {
        if (word.empty()) return 0;
        
        TrieNode* current = root;
        
        for (char c : word) {
            if (c < 'a' || c > 'z') return 0;
            
            int index = char_to_index(c);
            
            if (!current->children[index]) {
                return 0;
            }
            
            current = current->children[index];
        }
        
        return current->is_end_of_word ? current->word_count : 0;
    }
    
    // 获取前缀数量
    int get_prefix_count(const string& prefix) const {
        if (prefix.empty()) return total_words;
        
        TrieNode* current = root;
        
        for (char c : prefix) {
            if (c < 'a' || c > 'z') return 0;
            
            int index = char_to_index(c);
            
            if (!current->children[index]) {
                return 0;
            }
            
            current = current->children[index];
        }
        
        return current->prefix_count;
    }
    
    // 获取所有以指定前缀开始的单词
    vector<string> get_words_with_prefix(const string& prefix) const {
        vector<string> result;
        
        TrieNode* current = root;
        
        // 找到前缀对应的节点
        for (char c : prefix) {
            if (c < 'a' || c > 'z') return result;
            
            int index = char_to_index(c);
            
            if (!current->children[index]) {
                return result;
            }
            
            current = current->children[index];
        }
        
        // 收集所有以此前缀开始的单词
        collect_words(current, prefix, result);
        
        return result;
    }
    
    // 自动补全（返回前limit个匹配项）
    vector<string> auto_complete(const string& prefix, int limit = 10) const {
        vector<string> result;
        
        TrieNode* current = root;
        
        // 找到前缀对应的节点
        for (char c : prefix) {
            if (c < 'a' || c > 'z') return result;
            
            int index = char_to_index(c);
            
            if (!current->children[index]) {
                return result;
            }
            
            current = current->children[index];
        }
        
        // 收集有限数量的单词
        collect_words_limited(current, prefix, result, limit);
        
        return result;
    }
    
    // 获取所有单词
    vector<string> get_all_words() const {
        return get_words_with_prefix("");
    }
    
    // 查找最长公共前缀
    string longest_common_prefix() const {
        string lcp = "";
        TrieNode* current = root;
        
        while (true) {
            int child_count = 0;
            int next_index = -1;
            
            // 计算子节点数量
            for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
                if (current->children[i]) {
                    child_count++;
                    next_index = i;
                }
            }
            
            // 如果有多个子节点或当前节点是单词结尾，停止
            if (child_count != 1 || current->is_end_of_word) {
                break;
            }
            
            lcp += index_to_char(next_index);
            current = current->children[next_index];
        }
        
        return lcp;
    }
    
    // 状态查询
    bool empty() const {
        return total_words == 0;
    }
    
    int size() const {
        return total_words;
    }
    
    void clear() {
        delete root;
        root = new TrieNode();
        total_words = 0;
    }
    
    // 统计信息
    int node_count() const {
        return count_nodes(root);
    }
    
    int depth() const {
        return max_depth(root) - 1;  // 减去根节点
    }
    
    double average_word_length() const {
        if (total_words == 0) return 0.0;
        
        vector<string> words = get_all_words();
        int total_length = 0;
        
        for (const string& word : words) {
            total_length += word.length();
        }
        
        return static_cast<double>(total_length) / total_words;
    }
    
    // 内存使用估算（字节）
    size_t memory_usage() const {
        return node_count() * sizeof(TrieNode);
    }
    
    // 打印字典树结构
    void print_structure() const {
        cout << "Trie Structure (" << total_words << " words, " 
             << node_count() << " nodes):" << endl;
        print_tree(root, "", 0);
    }
    
    // 打印统计信息
    void print_statistics() const {
        cout << "Trie Statistics:" << endl;
        cout << "  Total words: " << total_words << endl;
        cout << "  Total nodes: " << node_count() << endl;
        cout << "  Max depth: " << depth() << endl;
        cout << "  Average word length: " << average_word_length() << endl;
        cout << "  Memory usage: " << memory_usage() << " bytes" << endl;
        
        if (total_words > 0) {
            cout << "  Compression ratio: " 
                 << static_cast<double>(node_count()) / total_words << endl;
        }
    }
    
    // 验证字典树完整性
    bool validate() const {
        return validate_node(root, 0);
    }
    
private:
    bool validate_node(TrieNode* node, int expected_prefix_count) const {
        if (!node) return true;
        
        int actual_prefix_count = 0;
        
        // 计算实际的前缀数量
        if (node->is_end_of_word) {
            actual_prefix_count += node->word_count;
        }
        
        for (int i = 0; i < TrieNode::ALPHABET_SIZE; ++i) {
            if (node->children[i]) {
                actual_prefix_count += node->children[i]->prefix_count;
                
                if (!validate_node(node->children[i], node->children[i]->prefix_count)) {
                    return false;
                }
            }
        }
        
        return actual_prefix_count == node->prefix_count;
    }
};

// 支持任意字符的字典树（哈希表实现）
class TrieMap {
private:
    TrieNodeMap* root;
    int total_words;
    
    // 递归删除
    bool delete_helper(TrieNodeMap* node, const string& word, int index) {
        if (index == word.length()) {
            if (!node->is_end_of_word) {
                return false;
            }
            
            node->is_end_of_word = false;
            node->word_count--;
            total_words--;
            
            return node->children.empty();
        }
        
        char c = word[index];
        auto it = node->children.find(c);
        
        if (it == node->children.end()) {
            return false;
        }
        
        bool should_delete_child = delete_helper(it->second, word, index + 1);
        
        if (should_delete_child) {
            delete it->second;
            node->children.erase(it);
            node->prefix_count--;
            
            return !node->is_end_of_word && node->children.empty();
        }
        
        return false;
    }
    
    // 收集单词
    void collect_words(TrieNodeMap* node, const string& prefix, vector<string>& result) const {
        if (!node) return;
        
        if (node->is_end_of_word) {
            result.push_back(prefix);
        }
        
        for (const auto& pair : node->children) {
            collect_words(pair.second, prefix + pair.first, result);
        }
    }
    
    // 复制树
    TrieNodeMap* copy_tree(TrieNodeMap* node) const {
        if (!node) return nullptr;
        
        TrieNodeMap* new_node = new TrieNodeMap();
        new_node->is_end_of_word = node->is_end_of_word;
        new_node->word_count = node->word_count;
        new_node->prefix_count = node->prefix_count;
        
        for (const auto& pair : node->children) {
            new_node->children[pair.first] = copy_tree(pair.second);
        }
        
        return new_node;
    }
    
public:
    // 构造函数
    TrieMap() : root(new TrieNodeMap()), total_words(0) {}
    
    // 拷贝构造函数
    TrieMap(const TrieMap& other) : total_words(other.total_words) {
        root = copy_tree(other.root);
    }
    
    // 赋值操作符
    TrieMap& operator=(const TrieMap& other) {
        if (this != &other) {
            delete root;
            root = copy_tree(other.root);
            total_words = other.total_words;
        }
        return *this;
    }
    
    // 析构函数
    ~TrieMap() {
        delete root;
    }
    
    // 插入单词
    void insert(const string& word) {
        if (word.empty()) return;
        
        TrieNodeMap* current = root;
        
        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = new TrieNodeMap();
            }
            
            current = current->children[c];
            current->prefix_count++;
        }
        
        if (!current->is_end_of_word) {
            current->is_end_of_word = true;
            total_words++;
        }
        current->word_count++;
    }
    
    // 查找单词
    bool search(const string& word) const {
        if (word.empty()) return false;
        
        TrieNodeMap* current = root;
        
        for (char c : word) {
            auto it = current->children.find(c);
            if (it == current->children.end()) {
                return false;
            }
            current = it->second;
        }
        
        return current->is_end_of_word;
    }
    
    // 检查前缀
    bool starts_with(const string& prefix) const {
        if (prefix.empty()) return true;
        
        TrieNodeMap* current = root;
        
        for (char c : prefix) {
            auto it = current->children.find(c);
            if (it == current->children.end()) {
                return false;
            }
            current = it->second;
        }
        
        return true;
    }
    
    // 删除单词
    bool erase(const string& word) {
        if (word.empty()) return false;
        return delete_helper(root, word, 0);
    }
    
    // 获取所有以指定前缀开始的单词
    vector<string> get_words_with_prefix(const string& prefix) const {
        vector<string> result;
        
        TrieNodeMap* current = root;
        
        for (char c : prefix) {
            auto it = current->children.find(c);
            if (it == current->children.end()) {
                return result;
            }
            current = it->second;
        }
        
        collect_words(current, prefix, result);
        return result;
    }
    
    // 基本状态查询
    bool empty() const { return total_words == 0; }
    int size() const { return total_words; }
    
    void clear() {
        delete root;
        root = new TrieNodeMap();
        total_words = 0;
    }
};

// 实用函数

// 从字符串数组构建字典树
Trie build_trie_from_words(const vector<string>& words) {
    Trie trie;
    for (const string& word : words) {
        trie.insert(word);
    }
    return trie;
}

// 查找两个字符串的最长公共前缀
string longest_common_prefix(const string& str1, const string& str2) {
    string lcp = "";
    int min_len = min(str1.length(), str2.length());
    
    for (int i = 0; i < min_len; ++i) {
        if (str1[i] == str2[i]) {
            lcp += str1[i];
        } else {
            break;
        }
    }
    
    return lcp;
}

// 查找字符串数组的最长公共前缀
string longest_common_prefix(const vector<string>& strs) {
    if (strs.empty()) return "";
    if (strs.size() == 1) return strs[0];
    
    Trie trie;
    for (const string& str : strs) {
        trie.insert(str);
    }
    
    return trie.longest_common_prefix();
}

// 字典树迭代器
class TrieIterator {
private:
    vector<string> words;
    size_t current_index;
    
public:
    explicit TrieIterator(const Trie& trie) : current_index(0) {
        words = trie.get_all_words();
        sort(words.begin(), words.end());
    }
    
    bool has_next() const {
        return current_index < words.size();
    }
    
    string next() {
        if (!has_next()) {
            throw runtime_error("No more words");
        }
        return words[current_index++];
    }
    
    void reset() {
        current_index = 0;
    }
    
    size_t size() const {
        return words.size();
    }
};

#endif // TRIE_H