#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <queue>
#include <functional>
#include <cassert>
#include <random>
#include <chrono>

namespace trie {

// 基础字典树节点
struct TrieNode {
    std::vector<std::shared_ptr<TrieNode>> children;
    bool is_end;
    int count;  // 以此节点结尾的单词数量
    int prefix_count;  // 经过此节点的单词数量
    
    TrieNode(int alphabet_size = 26) 
        : children(alphabet_size), is_end(false), count(0), prefix_count(0) {}
};

// 基础字典树
class Trie {
private:
    std::shared_ptr<TrieNode> root;
    int alphabet_size;
    
    int char_to_index(char c) {
        return c - 'a';
    }
    
    char index_to_char(int idx) {
        return 'a' + idx;
    }
    
    void collect_words(std::shared_ptr<TrieNode> node, const std::string& prefix, 
                      std::vector<std::string>& result) {
        if (!node) return;
        
        if (node->is_end) {
            for (int i = 0; i < node->count; ++i) {
                result.push_back(prefix);
            }
        }
        
        for (int i = 0; i < alphabet_size; ++i) {
            if (node->children[i]) {
                collect_words(node->children[i], prefix + index_to_char(i), result);
            }
        }
    }
    
    void collect_words_with_limit(std::shared_ptr<TrieNode> node, const std::string& prefix,
                                 std::vector<std::string>& result, int limit) {
        if (!node || result.size() >= limit) return;
        
        if (node->is_end) {
            result.push_back(prefix);
        }
        
        for (int i = 0; i < alphabet_size && result.size() < limit; ++i) {
            if (node->children[i]) {
                collect_words_with_limit(node->children[i], prefix + index_to_char(i), result, limit);
            }
        }
    }
    
public:
    explicit Trie(int alpha_size = 26) : alphabet_size(alpha_size) {
        root = std::make_shared<TrieNode>(alphabet_size);
    }
    
    // 插入单词
    void insert(const std::string& word) {
        auto node = root;
        
        for (char c : word) {
            int idx = char_to_index(c);
            if (idx < 0 || idx >= alphabet_size) continue;
            
            if (!node->children[idx]) {
                node->children[idx] = std::make_shared<TrieNode>(alphabet_size);
            }
            node = node->children[idx];
            node->prefix_count++;
        }
        
        node->is_end = true;
        node->count++;
    }
    
    // 查找单词
    bool search(const std::string& word) {
        auto node = find_node(word);
        return node && node->is_end;
    }
    
    // 查找前缀
    bool starts_with(const std::string& prefix) {
        return find_node(prefix) != nullptr;
    }
    
    // 删除单词
    bool remove(const std::string& word) {
        return remove_helper(root, word, 0);
    }
    
    // 获取以指定前缀开头的所有单词
    std::vector<std::string> get_words_with_prefix(const std::string& prefix) {
        std::vector<std::string> result;
        auto node = find_node(prefix);
        if (node) {
            collect_words(node, prefix, result);
        }
        return result;
    }
    
    // 自动补全(限制返回数量)
    std::vector<std::string> auto_complete(const std::string& prefix, int limit = 10) {
        std::vector<std::string> result;
        auto node = find_node(prefix);
        if (node) {
            collect_words_with_limit(node, prefix, result, limit);
        }
        return result;
    }
    
    // 统计以指定前缀开头的单词数量
    int count_words_with_prefix(const std::string& prefix) {
        auto node = find_node(prefix);
        return node ? node->prefix_count : 0;
    }
    
    // 获取单词出现次数
    int count_word(const std::string& word) {
        auto node = find_node(word);
        return (node && node->is_end) ? node->count : 0;
    }
    
    // 获取所有单词
    std::vector<std::string> get_all_words() {
        std::vector<std::string> result;
        collect_words(root, "", result);
        return result;
    }
    
    // 清空字典树
    void clear() {
        root = std::make_shared<TrieNode>(alphabet_size);
    }
    
    // 检查是否为空
    bool empty() {
        return root->prefix_count == 0;
    }
    
    // 获取节点总数
    int get_node_count() {
        return count_nodes(root);
    }
    
    // 打印字典树结构
    void print_structure() {
        std::cout << "字典树结构:\n";
        print_helper(root, "", 0);
    }
    
private:
    std::shared_ptr<TrieNode> find_node(const std::string& word) {
        auto node = root;
        
        for (char c : word) {
            int idx = char_to_index(c);
            if (idx < 0 || idx >= alphabet_size || !node->children[idx]) {
                return nullptr;
            }
            node = node->children[idx];
        }
        
        return node;
    }
    
    bool remove_helper(std::shared_ptr<TrieNode> node, const std::string& word, int depth) {
        if (!node) return false;
        
        if (depth == word.length()) {
            if (!node->is_end) return false;
            
            node->count--;
            if (node->count == 0) {
                node->is_end = false;
            }
            
            // 如果节点没有子节点且不是结尾，可以删除
            return !node->is_end && node->prefix_count == 0;
        }
        
        int idx = char_to_index(word[depth]);
        if (idx < 0 || idx >= alphabet_size) return false;
        
        bool should_delete = remove_helper(node->children[idx], word, depth + 1);
        
        if (should_delete) {
            node->children[idx] = nullptr;
            node->prefix_count--;
            
            // 如果当前节点没有子节点且不是结尾，可以删除
            return !node->is_end && node->prefix_count == 0;
        }
        
        return false;
    }
    
    int count_nodes(std::shared_ptr<TrieNode> node) {
        if (!node) return 0;
        
        int count = 1;
        for (auto child : node->children) {
            count += count_nodes(child);
        }
        return count;
    }
    
    void print_helper(std::shared_ptr<TrieNode> node, const std::string& prefix, int depth) {
        if (!node) return;
        
        if (node->is_end) {
            std::cout << std::string(depth * 2, ' ') << "[" << prefix << "] (count: " 
                     << node->count << ")\n";
        }
        
        for (int i = 0; i < alphabet_size; ++i) {
            if (node->children[i]) {
                std::cout << std::string(depth * 2, ' ') << index_to_char(i) << "\n";
                print_helper(node->children[i], prefix + index_to_char(i), depth + 1);
            }
        }
    }
};

// 压缩字典树(Patricia Trie)
class CompressedTrie {
private:
    struct CompressedNode {
        std::unordered_map<char, std::shared_ptr<CompressedNode>> children;
        std::string edge_label;
        bool is_end;
        int count;
        
        CompressedNode() : is_end(false), count(0) {}
    };
    
    std::shared_ptr<CompressedNode> root;
    
    std::shared_ptr<CompressedNode> find_node(const std::string& word) {
        auto node = root;
        int pos = 0;
        
        while (pos < word.length() && node) {
            char c = word[pos];
            if (node->children.find(c) == node->children.end()) {
                return nullptr;
            }
            
            node = node->children[c];
            const std::string& label = node->edge_label;
            
            if (pos + label.length() > word.length()) {
                return nullptr;
            }
            
            if (word.substr(pos, label.length()) != label) {
                return nullptr;
            }
            
            pos += label.length();
        }
        
        return pos == word.length() ? node : nullptr;
    }
    
public:
    CompressedTrie() {
        root = std::make_shared<CompressedNode>();
    }
    
    void insert(const std::string& word) {
        auto node = root;
        int pos = 0;
        
        while (pos < word.length()) {
            char c = word[pos];
            
            if (node->children.find(c) == node->children.end()) {
                // 创建新节点
                auto new_node = std::make_shared<CompressedNode>();
                new_node->edge_label = word.substr(pos);
                new_node->is_end = true;
                new_node->count = 1;
                node->children[c] = new_node;
                return;
            }
            
            auto child = node->children[c];
            const std::string& label = child->edge_label;
            
            // 找到公共前缀
            int common_len = 0;
            int max_len = std::min(label.length(), word.length() - pos);
            
            while (common_len < max_len && 
                   label[common_len] == word[pos + common_len]) {
                common_len++;
            }
            
            if (common_len == label.length()) {
                // 完全匹配，继续向下
                node = child;
                pos += common_len;
            } else {
                // 需要分裂节点
                auto split_node = std::make_shared<CompressedNode>();
                split_node->edge_label = label.substr(0, common_len);
                
                // 更新原节点
                child->edge_label = label.substr(common_len);
                split_node->children[child->edge_label[0]] = child;
                
                // 创建新分支
                if (pos + common_len < word.length()) {
                    auto new_node = std::make_shared<CompressedNode>();
                    new_node->edge_label = word.substr(pos + common_len);
                    new_node->is_end = true;
                    new_node->count = 1;
                    split_node->children[new_node->edge_label[0]] = new_node;
                } else {
                    split_node->is_end = true;
                    split_node->count = 1;
                }
                
                node->children[c] = split_node;
                return;
            }
        }
        
        node->is_end = true;
        node->count++;
    }
    
    bool search(const std::string& word) {
        auto node = find_node(word);
        return node && node->is_end;
    }
    
    bool starts_with(const std::string& prefix) {
        return find_node(prefix) != nullptr;
    }
};

// AC自动机(Aho-Corasick)
class AhoCorasick {
private:
    struct ACNode {
        std::vector<std::shared_ptr<ACNode>> children;
        std::shared_ptr<ACNode> fail;
        std::vector<int> output;
        
        ACNode(int alphabet_size = 26) : children(alphabet_size), fail(nullptr) {}
    };
    
    std::shared_ptr<ACNode> root;
    std::vector<std::string> patterns;
    int alphabet_size;
    
    int char_to_index(char c) {
        return c - 'a';
    }
    
    void build_fail_function() {
        std::queue<std::shared_ptr<ACNode>> q;
        
        // 初始化第一层
        for (int i = 0; i < alphabet_size; ++i) {
            if (root->children[i]) {
                root->children[i]->fail = root;
                q.push(root->children[i]);
            } else {
                root->children[i] = root;
            }
        }
        
        // BFS构建失败函数
        while (!q.empty()) {
            auto node = q.front();
            q.pop();
            
            for (int i = 0; i < alphabet_size; ++i) {
                if (node->children[i]) {
                    auto child = node->children[i];
                    auto fail_node = node->fail;
                    
                    while (fail_node != root && !fail_node->children[i]) {
                        fail_node = fail_node->fail;
                    }
                    
                    child->fail = fail_node->children[i] ? fail_node->children[i] : root;
                    
                    // 合并输出
                    child->output.insert(child->output.end(),
                                        child->fail->output.begin(),
                                        child->fail->output.end());
                    
                    q.push(child);
                }
            }
        }
    }
    
public:
    explicit AhoCorasick(int alpha_size = 26) : alphabet_size(alpha_size) {
        root = std::make_shared<ACNode>(alphabet_size);
    }
    
    void add_pattern(const std::string& pattern) {
        patterns.push_back(pattern);
        auto node = root;
        
        for (char c : pattern) {
            int idx = char_to_index(c);
            if (idx < 0 || idx >= alphabet_size) continue;
            
            if (!node->children[idx]) {
                node->children[idx] = std::make_shared<ACNode>(alphabet_size);
            }
            node = node->children[idx];
        }
        
        node->output.push_back(patterns.size() - 1);
    }
    
    void build() {
        build_fail_function();
    }
    
    std::vector<std::pair<int, int>> search(const std::string& text) {
        std::vector<std::pair<int, int>> matches;  // (position, pattern_id)
        auto node = root;
        
        for (int i = 0; i < text.length(); ++i) {
            int idx = char_to_index(text[i]);
            if (idx < 0 || idx >= alphabet_size) continue;
            
            while (node != root && !node->children[idx]) {
                node = node->fail;
            }
            
            if (node->children[idx]) {
                node = node->children[idx];
            }
            
            for (int pattern_id : node->output) {
                matches.emplace_back(i - patterns[pattern_id].length() + 1, pattern_id);
            }
        }
        
        return matches;
    }
    
    const std::vector<std::string>& get_patterns() const {
        return patterns;
    }
};

// 字典树应用类
class TrieApplications {
public:
    // 单词搜索游戏
    class WordSearch {
    private:
        Trie trie;
        std::vector<std::vector<char>> board;
        std::vector<std::vector<bool>> visited;
        std::vector<std::string> result;
        int m, n;
        
        void dfs(int i, int j, std::string& path, std::shared_ptr<TrieNode> node) {
            if (i < 0 || i >= m || j < 0 || j >= n || visited[i][j]) {
                return;
            }
            
            char c = board[i][j];
            int idx = c - 'a';
            
            if (idx < 0 || idx >= 26 || !node->children[idx]) {
                return;
            }
            
            node = node->children[idx];
            path.push_back(c);
            visited[i][j] = true;
            
            if (node->is_end && node->count > 0) {
                result.push_back(path);
                node->count = 0;  // 避免重复
            }
            
            // 四个方向搜索
            int dx[] = {-1, 1, 0, 0};
            int dy[] = {0, 0, -1, 1};
            
            for (int k = 0; k < 4; ++k) {
                dfs(i + dx[k], j + dy[k], path, node);
            }
            
            path.pop_back();
            visited[i][j] = false;
        }
        
    public:
        std::vector<std::string> find_words(std::vector<std::vector<char>>& board,
                                           std::vector<std::string>& words) {
            this->board = board;
            m = board.size();
            n = board[0].size();
            visited.assign(m, std::vector<bool>(n, false));
            result.clear();
            
            // 构建字典树
            for (const std::string& word : words) {
                trie.insert(word);
            }
            
            // 从每个位置开始搜索
            for (int i = 0; i < m; ++i) {
                for (int j = 0; j < n; ++j) {
                    std::string path;
                    dfs(i, j, path, trie.root);
                }
            }
            
            return result;
        }
    };
    
    // 最长公共前缀
    static std::string longest_common_prefix(std::vector<std::string>& strs) {
        if (strs.empty()) return "";
        
        Trie trie;
        for (const std::string& str : strs) {
            trie.insert(str);
        }
        
        std::string result;
        auto node = trie.root;
        
        while (true) {
            int child_count = 0;
            int next_idx = -1;
            
            for (int i = 0; i < 26; ++i) {
                if (node->children[i]) {
                    child_count++;
                    next_idx = i;
                }
            }
            
            if (child_count != 1 || node->is_end) {
                break;
            }
            
            result += ('a' + next_idx);
            node = node->children[next_idx];
        }
        
        return result;
    }
    
    // 单词替换
    static std::string replace_words(std::vector<std::string>& dictionary, 
                                    std::string sentence) {
        Trie trie;
        for (const std::string& root : dictionary) {
            trie.insert(root);
        }
        
        std::istringstream iss(sentence);
        std::string word;
        std::vector<std::string> words;
        
        while (iss >> word) {
            words.push_back(word);
        }
        
        for (std::string& word : words) {
            auto node = trie.root;
            std::string prefix;
            
            for (char c : word) {
                int idx = c - 'a';
                if (idx < 0 || idx >= 26 || !node->children[idx]) {
                    break;
                }
                
                prefix += c;
                node = node->children[idx];
                
                if (node->is_end) {
                    word = prefix;
                    break;
                }
            }
        }
        
        std::string result;
        for (int i = 0; i < words.size(); ++i) {
            if (i > 0) result += " ";
            result += words[i];
        }
        
        return result;
    }
};

// 性能测试类
class TrieTester {
public:
    static void performance_test(int word_count, int avg_length) {
        std::cout << "\n字典树性能测试 (words=" << word_count 
                 << ", avg_length=" << avg_length << ")\n";
        
        // 生成测试数据
        std::vector<std::string> words;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> length_dis(avg_length / 2, avg_length * 2);
        std::uniform_int_distribution<int> char_dis(0, 25);
        
        for (int i = 0; i < word_count; ++i) {
            int length = length_dis(gen);
            std::string word;
            for (int j = 0; j < length; ++j) {
                word += ('a' + char_dis(gen));
            }
            words.push_back(word);
        }
        
        // 测试插入性能
        auto start = std::chrono::high_resolution_clock::now();
        Trie trie;
        
        for (const std::string& word : words) {
            trie.insert(word);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "插入用时: " << duration.count() << " 微秒\n";
        std::cout << "节点数量: " << trie.get_node_count() << "\n";
        
        // 测试查找性能
        start = std::chrono::high_resolution_clock::now();
        
        int found_count = 0;
        for (const std::string& word : words) {
            if (trie.search(word)) {
                found_count++;
            }
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "查找用时: " << duration.count() << " 微秒\n";
        std::cout << "找到单词: " << found_count << "/" << word_count << "\n";
        
        // 测试前缀查询性能
        start = std::chrono::high_resolution_clock::now();
        
        int prefix_count = 0;
        for (int i = 0; i < 1000; ++i) {
            std::string prefix = words[i % words.size()].substr(0, avg_length / 2);
            prefix_count += trie.count_words_with_prefix(prefix);
        }
        
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "前缀查询用时: " << duration.count() << " 微秒\n";
        std::cout << "平均前缀匹配数: " << prefix_count / 1000.0 << "\n";
    }
    
    static void correctness_test() {
        std::cout << "\n字典树正确性测试\n";
        
        Trie trie;
        
        // 测试插入和查找
        trie.insert("apple");
        trie.insert("app");
        trie.insert("application");
        trie.insert("apply");
        
        assert(trie.search("apple"));
        assert(trie.search("app"));
        assert(!trie.search("appl"));
        assert(trie.starts_with("app"));
        assert(!trie.starts_with("orange"));
        
        // 测试前缀统计
        assert(trie.count_words_with_prefix("app") == 4);
        assert(trie.count_words_with_prefix("appl") == 3);
        
        // 测试自动补全
        auto completions = trie.auto_complete("app", 3);
        assert(completions.size() <= 3);
        
        // 测试删除
        assert(trie.remove("app"));
        assert(!trie.search("app"));
        assert(trie.search("apple"));
        
        std::cout << "所有测试通过！\n";
    }
    
    static void application_test() {
        std::cout << "\n字典树应用测试\n";
        
        // 测试AC自动机
        AhoCorasick ac;
        ac.add_pattern("he");
        ac.add_pattern("she");
        ac.add_pattern("his");
        ac.add_pattern("hers");
        ac.build();
        
        std::string text = "ushers";
        auto matches = ac.search(text);
        
        std::cout << "AC自动机匹配结果:\n";
        for (const auto& match : matches) {
            std::cout << "位置 " << match.first << ": " 
                     << ac.get_patterns()[match.second] << "\n";
        }
        
        // 测试最长公共前缀
        std::vector<std::string> strs = {"flower", "flow", "flight"};
        std::string lcp = TrieApplications::longest_common_prefix(strs);
        std::cout << "最长公共前缀: " << lcp << "\n";
        
        // 测试单词替换
        std::vector<std::string> dictionary = {"cat", "bat", "rat"};
        std::string sentence = "the cattle was rattled by the battery";
        std::string replaced = TrieApplications::replace_words(dictionary, sentence);
        std::cout << "替换后: " << replaced << "\n";
    }
};

} // namespace trie

#endif // TRIE_H