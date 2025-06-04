#ifndef AC_AUTOMATON_H
#define AC_AUTOMATON_H

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <memory>
#include <iostream>
#include <algorithm>
#include <set>

using namespace std;

// AC自动机节点
struct ACNode {
    unordered_map<char, shared_ptr<ACNode>> children;
    shared_ptr<ACNode> failure;  // 失败指针
    vector<int> output;  // 输出模式串的索引
    bool is_end_of_pattern;
    int pattern_index;  // 模式串索引
    int depth;  // 节点深度
    
    ACNode() : failure(nullptr), is_end_of_pattern(false), pattern_index(-1), depth(0) {}
};

// 匹配结果
struct MatchResult {
    int start_pos;  // 匹配开始位置
    int end_pos;    // 匹配结束位置
    int pattern_index;  // 模式串索引
    string pattern;  // 匹配的模式串
    
    MatchResult(int start, int end, int idx, const string& pat)
        : start_pos(start), end_pos(end), pattern_index(idx), pattern(pat) {}
};

// AC自动机类
class ACAutomaton {
private:
    shared_ptr<ACNode> root;
    vector<string> patterns;  // 存储所有模式串
    bool is_built;  // 是否已构建失败指针
    
    // 构建失败指针
    void build_failure_links() {
        queue<shared_ptr<ACNode>> q;
        
        // 初始化根节点的直接子节点
        for (auto& pair : root->children) {
            auto child = pair.second;
            child->failure = root;
            q.push(child);
        }
        
        // BFS构建失败指针
        while (!q.empty()) {
            auto current = q.front();
            q.pop();
            
            for (auto& pair : current->children) {
                char ch = pair.first;
                auto child = pair.second;
                
                // 寻找失败指针
                auto failure_node = current->failure;
                while (failure_node != root && 
                       failure_node->children.find(ch) == failure_node->children.end()) {
                    failure_node = failure_node->failure;
                }
                
                if (failure_node->children.find(ch) != failure_node->children.end() &&
                    failure_node->children[ch] != child) {
                    child->failure = failure_node->children[ch];
                } else {
                    child->failure = root;
                }
                
                // 合并输出
                child->output = child->failure->output;
                if (child->is_end_of_pattern) {
                    child->output.push_back(child->pattern_index);
                }
                
                q.push(child);
            }
        }
        
        is_built = true;
    }
    
    // 获取转移状态
    shared_ptr<ACNode> get_next_state(shared_ptr<ACNode> current, char ch) {
        while (current != root && 
               current->children.find(ch) == current->children.end()) {
            current = current->failure;
        }
        
        if (current->children.find(ch) != current->children.end()) {
            return current->children[ch];
        }
        
        return root;
    }
    
    // 打印节点信息
    void print_node(shared_ptr<ACNode> node, const string& prefix, int depth) const {
        if (!node) return;
        
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }
        
        cout << "[" << (prefix.empty() ? "ROOT" : string(1, prefix.back())) << "]";
        
        if (node->is_end_of_pattern) {
            cout << " (PATTERN: " << patterns[node->pattern_index] << ")";
        }
        
        if (!node->output.empty()) {
            cout << " (OUTPUT: ";
            for (size_t i = 0; i < node->output.size(); ++i) {
                cout << patterns[node->output[i]];
                if (i < node->output.size() - 1) cout << ", ";
            }
            cout << ")";
        }
        
        cout << " (DEPTH: " << node->depth << ")";
        cout << endl;
        
        for (const auto& pair : node->children) {
            print_node(pair.second, prefix + pair.first, depth + 1);
        }
    }
    
    // 计算节点数量
    int count_nodes(shared_ptr<ACNode> node) const {
        if (!node) return 0;
        
        int count = 1;
        for (const auto& pair : node->children) {
            count += count_nodes(pair.second);
        }
        return count;
    }
    
    // 复制树结构
    shared_ptr<ACNode> copy_tree(shared_ptr<ACNode> node) const {
        if (!node) return nullptr;
        
        auto new_node = make_shared<ACNode>();
        new_node->is_end_of_pattern = node->is_end_of_pattern;
        new_node->pattern_index = node->pattern_index;
        new_node->depth = node->depth;
        new_node->output = node->output;
        
        for (const auto& pair : node->children) {
            new_node->children[pair.first] = copy_tree(pair.second);
        }
        
        return new_node;
    }
    
    // 重建失败指针（用于拷贝后）
    void rebuild_failure_links() {
        if (is_built) {
            is_built = false;
            build_failure_links();
        }
    }
    
public:
    // 构造函数
    ACAutomaton() : root(make_shared<ACNode>()), is_built(false) {}
    
    // 拷贝构造函数
    ACAutomaton(const ACAutomaton& other) 
        : patterns(other.patterns), is_built(false) {
        root = copy_tree(other.root);
        if (other.is_built) {
            build_failure_links();
        }
    }
    
    // 赋值操作符
    ACAutomaton& operator=(const ACAutomaton& other) {
        if (this != &other) {
            patterns = other.patterns;
            root = copy_tree(other.root);
            is_built = false;
            if (other.is_built) {
                build_failure_links();
            }
        }
        return *this;
    }
    
    // 移动构造函数
    ACAutomaton(ACAutomaton&& other) noexcept
        : root(move(other.root)), patterns(move(other.patterns)), is_built(other.is_built) {
        other.is_built = false;
    }
    
    // 移动赋值操作符
    ACAutomaton& operator=(ACAutomaton&& other) noexcept {
        if (this != &other) {
            root = move(other.root);
            patterns = move(other.patterns);
            is_built = other.is_built;
            other.is_built = false;
        }
        return *this;
    }
    
    // 添加模式串
    void add_pattern(const string& pattern) {
        if (pattern.empty()) return;
        
        patterns.push_back(pattern);
        int pattern_index = patterns.size() - 1;
        
        shared_ptr<ACNode> current = root;
        
        for (char ch : pattern) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = make_shared<ACNode>();
                current->children[ch]->depth = current->depth + 1;
            }
            current = current->children[ch];
        }
        
        current->is_end_of_pattern = true;
        current->pattern_index = pattern_index;
        
        // 标记需要重新构建
        is_built = false;
    }
    
    // 批量添加模式串
    void add_patterns(const vector<string>& pattern_list) {
        for (const string& pattern : pattern_list) {
            add_pattern(pattern);
        }
    }
    
    // 构建自动机（构建失败指针）
    void build() {
        if (!is_built) {
            build_failure_links();
        }
    }
    
    // 搜索文本中的所有匹配
    vector<MatchResult> search(const string& text) {
        if (!is_built) {
            build();
        }
        
        vector<MatchResult> results;
        shared_ptr<ACNode> current = root;
        
        for (int i = 0; i < text.length(); ++i) {
            char ch = text[i];
            current = get_next_state(current, ch);
            
            // 检查所有可能的匹配
            for (int pattern_idx : current->output) {
                const string& pattern = patterns[pattern_idx];
                int start_pos = i - pattern.length() + 1;
                results.emplace_back(start_pos, i, pattern_idx, pattern);
            }
        }
        
        return results;
    }
    
    // 检查文本是否包含任何模式串
    bool contains_any(const string& text) {
        if (!is_built) {
            build();
        }
        
        shared_ptr<ACNode> current = root;
        
        for (char ch : text) {
            current = get_next_state(current, ch);
            
            if (!current->output.empty()) {
                return true;
            }
        }
        
        return false;
    }
    
    // 查找第一个匹配
    pair<bool, MatchResult> find_first(const string& text) {
        if (!is_built) {
            build();
        }
        
        shared_ptr<ACNode> current = root;
        
        for (int i = 0; i < text.length(); ++i) {
            char ch = text[i];
            current = get_next_state(current, ch);
            
            if (!current->output.empty()) {
                int pattern_idx = current->output[0];
                const string& pattern = patterns[pattern_idx];
                int start_pos = i - pattern.length() + 1;
                return {true, MatchResult(start_pos, i, pattern_idx, pattern)};
            }
        }
        
        return {false, MatchResult(-1, -1, -1, "")};
    }
    
    // 统计匹配数量
    int count_matches(const string& text) {
        auto results = search(text);
        return results.size();
    }
    
    // 获取所有匹配的模式串（去重）
    set<string> get_matched_patterns(const string& text) {
        auto results = search(text);
        set<string> matched;
        
        for (const auto& result : results) {
            matched.insert(result.pattern);
        }
        
        return matched;
    }
    
    // 替换文本中的模式串
    string replace_patterns(const string& text, const string& replacement) {
        auto results = search(text);
        
        if (results.empty()) {
            return text;
        }
        
        // 按位置排序（从后往前替换避免位置偏移）
        sort(results.begin(), results.end(), 
             [](const MatchResult& a, const MatchResult& b) {
                 return a.start_pos > b.start_pos;
             });
        
        string result = text;
        
        for (const auto& match : results) {
            result.replace(match.start_pos, 
                          match.end_pos - match.start_pos + 1, 
                          replacement);
        }
        
        return result;
    }
    
    // 高亮显示匹配的模式串
    string highlight_matches(const string& text, 
                           const string& start_tag = "<mark>", 
                           const string& end_tag = "</mark>") {
        auto results = search(text);
        
        if (results.empty()) {
            return text;
        }
        
        // 按位置排序（从后往前处理）
        sort(results.begin(), results.end(), 
             [](const MatchResult& a, const MatchResult& b) {
                 return a.start_pos > b.start_pos;
             });
        
        string result = text;
        
        for (const auto& match : results) {
            result.insert(match.end_pos + 1, end_tag);
            result.insert(match.start_pos, start_tag);
        }
        
        return result;
    }
    
    // 获取模式串列表
    const vector<string>& get_patterns() const {
        return patterns;
    }
    
    // 状态查询
    bool empty() const {
        return patterns.empty();
    }
    
    int pattern_count() const {
        return patterns.size();
    }
    
    bool is_automaton_built() const {
        return is_built;
    }
    
    // 清空所有模式串
    void clear() {
        patterns.clear();
        root = make_shared<ACNode>();
        is_built = false;
    }
    
    // 统计信息
    int node_count() const {
        return count_nodes(root);
    }
    
    int max_pattern_length() const {
        if (patterns.empty()) return 0;
        
        int max_len = 0;
        for (const string& pattern : patterns) {
            max_len = max(max_len, static_cast<int>(pattern.length()));
        }
        return max_len;
    }
    
    double average_pattern_length() const {
        if (patterns.empty()) return 0.0;
        
        int total_length = 0;
        for (const string& pattern : patterns) {
            total_length += pattern.length();
        }
        
        return static_cast<double>(total_length) / patterns.size();
    }
    
    // 内存使用估算
    size_t memory_usage() const {
        size_t usage = node_count() * sizeof(ACNode);
        
        // 添加模式串存储的内存
        for (const string& pattern : patterns) {
            usage += pattern.length() * sizeof(char);
        }
        
        return usage;
    }
    
    // 打印自动机结构
    void print_structure() const {
        cout << "AC Automaton Structure (" << patterns.size() << " patterns, " 
             << node_count() << " nodes):" << endl;
        
        cout << "Patterns: ";
        for (size_t i = 0; i < patterns.size(); ++i) {
            cout << "\"" << patterns[i] << "\"";
            if (i < patterns.size() - 1) cout << ", ";
        }
        cout << endl << endl;
        
        print_node(root, "", 0);
    }
    
    // 打印统计信息
    void print_statistics() const {
        cout << "AC Automaton Statistics:" << endl;
        cout << "  Pattern count: " << pattern_count() << endl;
        cout << "  Node count: " << node_count() << endl;
        cout << "  Max pattern length: " << max_pattern_length() << endl;
        cout << "  Average pattern length: " << average_pattern_length() << endl;
        cout << "  Memory usage: " << memory_usage() << " bytes" << endl;
        cout << "  Automaton built: " << (is_built ? "Yes" : "No") << endl;
        
        if (!patterns.empty()) {
            cout << "  Compression ratio: " 
                 << static_cast<double>(node_count()) / patterns.size() << endl;
        }
    }
    
    // 验证自动机完整性
    bool validate() const {
        if (!is_built) {
            return false;
        }
        
        // 检查所有节点的失败指针是否正确设置
        return validate_node(root);
    }
    
private:
    bool validate_node(shared_ptr<ACNode> node) const {
        if (!node) return false;
        
        // 根节点的失败指针应该为空或指向自己
        if (node == root && node->failure != nullptr && node->failure != root) {
            return false;
        }
        
        // 非根节点应该有失败指针
        if (node != root && node->failure == nullptr) {
            return false;
        }
        
        // 递归验证子节点
        for (const auto& pair : node->children) {
            if (!validate_node(pair.second)) {
                return false;
            }
        }
        
        return true;
    }
};

// 实用函数

// 从模式串数组构建AC自动机
ACAutomaton build_ac_automaton(const vector<string>& patterns) {
    ACAutomaton ac;
    ac.add_patterns(patterns);
    ac.build();
    return ac;
}

// 多模式串匹配（简化接口）
vector<MatchResult> multi_pattern_search(const string& text, const vector<string>& patterns) {
    ACAutomaton ac = build_ac_automaton(patterns);
    return ac.search(text);
}

// 检查文本是否包含任何敏感词
bool contains_sensitive_words(const string& text, const vector<string>& sensitive_words) {
    ACAutomaton ac = build_ac_automaton(sensitive_words);
    return ac.contains_any(text);
}

// 过滤敏感词
string filter_sensitive_words(const string& text, 
                             const vector<string>& sensitive_words, 
                             const string& replacement = "***") {
    ACAutomaton ac = build_ac_automaton(sensitive_words);
    return ac.replace_patterns(text, replacement);
}

// 性能测试结果
struct ACPerformanceResult {
    double build_time_ms;
    double search_time_ms;
    int match_count;
    int node_count;
    size_t memory_usage;
    
    void print() const {
        cout << "AC Automaton Performance:" << endl;
        cout << "  Build time: " << build_time_ms << " ms" << endl;
        cout << "  Search time: " << search_time_ms << " ms" << endl;
        cout << "  Matches found: " << match_count << endl;
        cout << "  Node count: " << node_count << endl;
        cout << "  Memory usage: " << memory_usage << " bytes" << endl;
    }
};

// AC自动机迭代器（遍历所有匹配结果）
class ACMatchIterator {
private:
    vector<MatchResult> matches;
    size_t current_index;
    
public:
    ACMatchIterator(const ACAutomaton& ac, const string& text) : current_index(0) {
        matches = ac.search(text);
        sort(matches.begin(), matches.end(), 
             [](const MatchResult& a, const MatchResult& b) {
                 return a.start_pos < b.start_pos;
             });
    }
    
    bool has_next() const {
        return current_index < matches.size();
    }
    
    MatchResult next() {
        if (!has_next()) {
            throw runtime_error("No more matches");
        }
        return matches[current_index++];
    }
    
    void reset() {
        current_index = 0;
    }
    
    size_t size() const {
        return matches.size();
    }
    
    const vector<MatchResult>& get_all_matches() const {
        return matches;
    }
};

#endif // AC_AUTOMATON_H