#ifndef COMPRESSED_TRIE_H
#define COMPRESSED_TRIE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <algorithm>

using namespace std;

// 压缩字典树节点
struct CompressedTrieNode {
    unordered_map<string, shared_ptr<CompressedTrieNode>> children;
    bool is_end_of_word;
    int word_count;
    string edge_label;  // 边上的标签
    
    CompressedTrieNode(const string& label = "") 
        : is_end_of_word(false), word_count(0), edge_label(label) {}
};

// 压缩字典树类（Radix Tree / Patricia Tree）
class CompressedTrie {
private:
    shared_ptr<CompressedTrieNode> root;
    int total_words;
    
    // 查找最长公共前缀
    int longest_common_prefix_length(const string& str1, const string& str2) const {
        int len = 0;
        int min_len = min(str1.length(), str2.length());
        
        while (len < min_len && str1[len] == str2[len]) {
            len++;
        }
        
        return len;
    }
    
    // 分割节点
    shared_ptr<CompressedTrieNode> split_node(
        shared_ptr<CompressedTrieNode> node, 
        const string& edge_label, 
        int split_pos) {
        
        // 创建新的中间节点
        auto new_node = make_shared<CompressedTrieNode>(edge_label.substr(0, split_pos));
        
        // 更新原节点的边标签
        string remaining_label = edge_label.substr(split_pos);
        node->edge_label = remaining_label;
        
        // 将原节点作为新节点的子节点
        new_node->children[remaining_label] = node;
        
        return new_node;
    }
    
    // 递归插入
    shared_ptr<CompressedTrieNode> insert_helper(
        shared_ptr<CompressedTrieNode> node, 
        const string& word, 
        int start_pos) {
        
        if (start_pos >= word.length()) {
            // 到达单词末尾
            if (!node->is_end_of_word) {
                node->is_end_of_word = true;
                total_words++;
            }
            node->word_count++;
            return node;
        }
        
        string remaining_word = word.substr(start_pos);
        
        // 查找匹配的子节点
        for (auto& pair : node->children) {
            const string& edge_label = pair.first;
            auto child = pair.second;
            
            int common_len = longest_common_prefix_length(remaining_word, edge_label);
            
            if (common_len > 0) {
                if (common_len == edge_label.length()) {
                    // 完全匹配边标签，继续在子节点中插入
                    insert_helper(child, word, start_pos + common_len);
                    return node;
                } else {
                    // 部分匹配，需要分割节点
                    auto new_child = split_node(child, edge_label, common_len);
                    
                    // 更新父节点的子节点映射
                    node->children.erase(edge_label);
                    node->children[edge_label.substr(0, common_len)] = new_child;
                    
                    // 在新节点中继续插入
                    insert_helper(new_child, word, start_pos + common_len);
                    return node;
                }
            }
        }
        
        // 没有找到匹配的边，创建新的子节点
        auto new_child = make_shared<CompressedTrieNode>(remaining_word);
        new_child->is_end_of_word = true;
        new_child->word_count = 1;
        total_words++;
        
        node->children[remaining_word] = new_child;
        
        return node;
    }
    
    // 递归查找
    shared_ptr<CompressedTrieNode> find_node(
        shared_ptr<CompressedTrieNode> node, 
        const string& word, 
        int start_pos) const {
        
        if (start_pos >= word.length()) {
            return node;
        }
        
        string remaining_word = word.substr(start_pos);
        
        for (const auto& pair : node->children) {
            const string& edge_label = pair.first;
            auto child = pair.second;
            
            if (remaining_word.length() >= edge_label.length() &&
                remaining_word.substr(0, edge_label.length()) == edge_label) {
                return find_node(child, word, start_pos + edge_label.length());
            }
        }
        
        return nullptr;
    }
    
    // 递归删除
    bool delete_helper(
        shared_ptr<CompressedTrieNode> node, 
        const string& word, 
        int start_pos) {
        
        if (start_pos >= word.length()) {
            if (!node->is_end_of_word) {
                return false;  // 单词不存在
            }
            
            node->is_end_of_word = false;
            node->word_count--;
            total_words--;
            
            // 如果没有子节点且不是单词结尾，可以删除
            return node->children.empty() && !node->is_end_of_word;
        }
        
        string remaining_word = word.substr(start_pos);
        
        for (auto it = node->children.begin(); it != node->children.end(); ++it) {
            const string& edge_label = it->first;
            auto child = it->second;
            
            if (remaining_word.length() >= edge_label.length() &&
                remaining_word.substr(0, edge_label.length()) == edge_label) {
                
                bool should_delete_child = delete_helper(child, word, start_pos + edge_label.length());
                
                if (should_delete_child) {
                    node->children.erase(it);
                    
                    // 如果只有一个子节点且当前节点不是单词结尾，可以合并
                    if (node->children.size() == 1 && !node->is_end_of_word) {
                        auto remaining_child = node->children.begin();
                        string merged_label = node->edge_label + remaining_child->first;
                        
                        // 合并节点
                        *node = *remaining_child->second;
                        node->edge_label = merged_label;
                    }
                }
                
                return false;
            }
        }
        
        return false;
    }
    
    // 收集所有单词
    void collect_words(
        shared_ptr<CompressedTrieNode> node, 
        const string& prefix, 
        vector<string>& result) const {
        
        if (!node) return;
        
        string current_prefix = prefix + node->edge_label;
        
        if (node->is_end_of_word) {
            result.push_back(current_prefix);
        }
        
        for (const auto& pair : node->children) {
            collect_words(pair.second, current_prefix, result);
        }
    }
    
    // 收集有限数量的单词
    void collect_words_limited(
        shared_ptr<CompressedTrieNode> node, 
        const string& prefix, 
        vector<string>& result, 
        int limit) const {
        
        if (!node || result.size() >= limit) return;
        
        string current_prefix = prefix + node->edge_label;
        
        if (node->is_end_of_word) {
            result.push_back(current_prefix);
            if (result.size() >= limit) return;
        }
        
        for (const auto& pair : node->children) {
            collect_words_limited(pair.second, current_prefix, result, limit);
            if (result.size() >= limit) return;
        }
    }
    
    // 计算节点数量
    int count_nodes(shared_ptr<CompressedTrieNode> node) const {
        if (!node) return 0;
        
        int count = 1;
        for (const auto& pair : node->children) {
            count += count_nodes(pair.second);
        }
        return count;
    }
    
    // 计算最大深度
    int max_depth(shared_ptr<CompressedTrieNode> node) const {
        if (!node) return 0;
        
        int max_child_depth = 0;
        for (const auto& pair : node->children) {
            max_child_depth = max(max_child_depth, max_depth(pair.second));
        }
        return 1 + max_child_depth;
    }
    
    // 打印树结构
    void print_tree(
        shared_ptr<CompressedTrieNode> node, 
        const string& prefix, 
        int depth) const {
        
        if (!node) return;
        
        // 打印当前节点
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }
        
        cout << "[" << node->edge_label << "]";
        if (node->is_end_of_word) {
            cout << " (END: " << node->word_count << ")";
        }
        cout << endl;
        
        // 递归打印子节点
        for (const auto& pair : node->children) {
            print_tree(pair.second, prefix + node->edge_label, depth + 1);
        }
    }
    
    // 复制树
    shared_ptr<CompressedTrieNode> copy_tree(shared_ptr<CompressedTrieNode> node) const {
        if (!node) return nullptr;
        
        auto new_node = make_shared<CompressedTrieNode>(node->edge_label);
        new_node->is_end_of_word = node->is_end_of_word;
        new_node->word_count = node->word_count;
        
        for (const auto& pair : node->children) {
            new_node->children[pair.first] = copy_tree(pair.second);
        }
        
        return new_node;
    }
    
public:
    // 构造函数
    CompressedTrie() : root(make_shared<CompressedTrieNode>()), total_words(0) {}
    
    // 拷贝构造函数
    CompressedTrie(const CompressedTrie& other) : total_words(other.total_words) {
        root = copy_tree(other.root);
    }
    
    // 赋值操作符
    CompressedTrie& operator=(const CompressedTrie& other) {
        if (this != &other) {
            root = copy_tree(other.root);
            total_words = other.total_words;
        }
        return *this;
    }
    
    // 移动构造函数
    CompressedTrie(CompressedTrie&& other) noexcept 
        : root(move(other.root)), total_words(other.total_words) {
        other.total_words = 0;
    }
    
    // 移动赋值操作符
    CompressedTrie& operator=(CompressedTrie&& other) noexcept {
        if (this != &other) {
            root = move(other.root);
            total_words = other.total_words;
            other.total_words = 0;
        }
        return *this;
    }
    
    // 插入单词
    void insert(const string& word) {
        if (word.empty()) return;
        insert_helper(root, word, 0);
    }
    
    // 查找单词
    bool search(const string& word) const {
        if (word.empty()) return false;
        
        auto node = find_node(root, word, 0);
        return node && node->is_end_of_word;
    }
    
    // 检查前缀
    bool starts_with(const string& prefix) const {
        if (prefix.empty()) return true;
        
        shared_ptr<CompressedTrieNode> current = root;
        int pos = 0;
        
        while (pos < prefix.length()) {
            bool found = false;
            
            for (const auto& pair : current->children) {
                const string& edge_label = pair.first;
                auto child = pair.second;
                
                int common_len = longest_common_prefix_length(
                    prefix.substr(pos), edge_label);
                
                if (common_len > 0) {
                    pos += common_len;
                    current = child;
                    found = true;
                    
                    if (common_len < edge_label.length()) {
                        // 前缀在边的中间结束
                        return pos >= prefix.length();
                    }
                    break;
                }
            }
            
            if (!found) {
                return false;
            }
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
        
        auto node = find_node(root, word, 0);
        return (node && node->is_end_of_word) ? node->word_count : 0;
    }
    
    // 获取所有以指定前缀开始的单词
    vector<string> get_words_with_prefix(const string& prefix) const {
        vector<string> result;
        
        if (prefix.empty()) {
            collect_words(root, "", result);
            return result;
        }
        
        shared_ptr<CompressedTrieNode> current = root;
        string matched_prefix = "";
        int pos = 0;
        
        while (pos < prefix.length()) {
            bool found = false;
            
            for (const auto& pair : current->children) {
                const string& edge_label = pair.first;
                auto child = pair.second;
                
                int common_len = longest_common_prefix_length(
                    prefix.substr(pos), edge_label);
                
                if (common_len > 0) {
                    matched_prefix += edge_label.substr(0, common_len);
                    pos += common_len;
                    current = child;
                    found = true;
                    
                    if (common_len < edge_label.length()) {
                        // 前缀在边的中间结束
                        if (pos >= prefix.length()) {
                            // 找到了完整前缀，但在边的中间
                            // 需要创建一个虚拟节点来收集结果
                            string remaining_edge = edge_label.substr(common_len);
                            if (child->is_end_of_word && 
                                matched_prefix == prefix) {
                                result.push_back(matched_prefix);
                            }
                            collect_words(child, matched_prefix, result);
                        }
                        return result;
                    }
                    break;
                }
            }
            
            if (!found) {
                return result;
            }
        }
        
        // 找到了前缀对应的节点
        if (current->is_end_of_word && matched_prefix == prefix) {
            result.push_back(matched_prefix);
        }
        
        collect_words(current, matched_prefix, result);
        
        // 移除前缀本身（如果它在结果中）
        auto it = find(result.begin(), result.end(), prefix);
        if (it != result.end() && current->is_end_of_word) {
            // 保留前缀本身，因为它是一个有效单词
        }
        
        return result;
    }
    
    // 自动补全
    vector<string> auto_complete(const string& prefix, int limit = 10) const {
        vector<string> result;
        
        if (prefix.empty()) {
            collect_words_limited(root, "", result, limit);
            return result;
        }
        
        shared_ptr<CompressedTrieNode> current = root;
        string matched_prefix = "";
        int pos = 0;
        
        while (pos < prefix.length()) {
            bool found = false;
            
            for (const auto& pair : current->children) {
                const string& edge_label = pair.first;
                auto child = pair.second;
                
                int common_len = longest_common_prefix_length(
                    prefix.substr(pos), edge_label);
                
                if (common_len > 0) {
                    matched_prefix += edge_label.substr(0, common_len);
                    pos += common_len;
                    current = child;
                    found = true;
                    
                    if (common_len < edge_label.length()) {
                        if (pos >= prefix.length()) {
                            collect_words_limited(child, matched_prefix, result, limit);
                        }
                        return result;
                    }
                    break;
                }
            }
            
            if (!found) {
                return result;
            }
        }
        
        collect_words_limited(current, matched_prefix, result, limit);
        return result;
    }
    
    // 获取所有单词
    vector<string> get_all_words() const {
        vector<string> result;
        collect_words(root, "", result);
        return result;
    }
    
    // 状态查询
    bool empty() const {
        return total_words == 0;
    }
    
    int size() const {
        return total_words;
    }
    
    void clear() {
        root = make_shared<CompressedTrieNode>();
        total_words = 0;
    }
    
    // 统计信息
    int node_count() const {
        return count_nodes(root);
    }
    
    int depth() const {
        return max_depth(root) - 1;
    }
    
    double compression_ratio() const {
        if (total_words == 0) return 0.0;
        return static_cast<double>(node_count()) / total_words;
    }
    
    // 计算总的边标签长度
    int total_edge_length() const {
        return calculate_edge_length(root);
    }
    
    // 内存使用估算
    size_t memory_usage() const {
        return node_count() * sizeof(CompressedTrieNode) + 
               total_edge_length() * sizeof(char);
    }
    
    // 打印树结构
    void print_structure() const {
        cout << "Compressed Trie Structure (" << total_words << " words, " 
             << node_count() << " nodes):" << endl;
        print_tree(root, "", 0);
    }
    
    // 打印统计信息
    void print_statistics() const {
        cout << "Compressed Trie Statistics:" << endl;
        cout << "  Total words: " << total_words << endl;
        cout << "  Total nodes: " << node_count() << endl;
        cout << "  Max depth: " << depth() << endl;
        cout << "  Compression ratio: " << compression_ratio() << endl;
        cout << "  Total edge length: " << total_edge_length() << endl;
        cout << "  Memory usage: " << memory_usage() << " bytes" << endl;
    }
    
private:
    int calculate_edge_length(shared_ptr<CompressedTrieNode> node) const {
        if (!node) return 0;
        
        int length = node->edge_label.length();
        for (const auto& pair : node->children) {
            length += calculate_edge_length(pair.second);
        }
        return length;
    }
};

// 实用函数

// 从字符串数组构建压缩字典树
CompressedTrie build_compressed_trie_from_words(const vector<string>& words) {
    CompressedTrie trie;
    for (const string& word : words) {
        trie.insert(word);
    }
    return trie;
}

// 比较标准字典树和压缩字典树的性能
struct TrieComparison {
    int standard_nodes;
    int compressed_nodes;
    size_t standard_memory;
    size_t compressed_memory;
    double compression_ratio;
    
    void print() const {
        cout << "Trie Comparison:" << endl;
        cout << "  Standard Trie Nodes: " << standard_nodes << endl;
        cout << "  Compressed Trie Nodes: " << compressed_nodes << endl;
        cout << "  Standard Memory: " << standard_memory << " bytes" << endl;
        cout << "  Compressed Memory: " << compressed_memory << " bytes" << endl;
        cout << "  Node Reduction: " 
             << (1.0 - static_cast<double>(compressed_nodes) / standard_nodes) * 100 
             << "%" << endl;
        cout << "  Memory Reduction: " 
             << (1.0 - static_cast<double>(compressed_memory) / standard_memory) * 100 
             << "%" << endl;
    }
};

// 压缩字典树迭代器
class CompressedTrieIterator {
private:
    vector<string> words;
    size_t current_index;
    
public:
    explicit CompressedTrieIterator(const CompressedTrie& trie) : current_index(0) {
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

#endif // COMPRESSED_TRIE_H