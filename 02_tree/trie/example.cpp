#include "trie.h"
#include "compressed_trie.h"
#include "ac_automaton.h"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

using namespace std;
using namespace chrono;

// 测试基本Trie
void test_basic_trie() {
    cout << "\n=== 测试基本Trie ===" << endl;
    
    Trie trie;
    
    // 插入单词
    vector<string> words = {"apple", "app", "application", "apply", "banana", "band", "bandana"};
    
    cout << "插入单词: ";
    for (const string& word : words) {
        trie.insert(word);
        cout << word << " ";
    }
    cout << endl;
    
    // 搜索测试
    cout << "\n搜索测试:" << endl;
    vector<string> search_words = {"app", "apple", "application", "appl", "banana", "cat"};
    
    for (const string& word : search_words) {
        bool found = trie.search(word);
        bool has_prefix = trie.starts_with(word);
        cout << "  \"" << word << "\": 存在=" << (found ? "是" : "否") 
             << ", 前缀=" << (has_prefix ? "是" : "否") << endl;
    }
    
    // 前缀查询
    cout << "\n前缀查询:" << endl;
    vector<string> prefixes = {"app", "ban", "band"};
    
    for (const string& prefix : prefixes) {
        auto words_with_prefix = trie.get_words_with_prefix(prefix);
        cout << "  前缀 \"" << prefix << "\": ";
        for (const string& word : words_with_prefix) {
            cout << word << " ";
        }
        cout << endl;
    }
    
    // 自动补全
    cout << "\n自动补全:" << endl;
    auto completions = trie.auto_complete("app", 3);
    cout << "  \"app\" 的补全建议: ";
    for (const string& completion : completions) {
        cout << completion << " ";
    }
    cout << endl;
    
    // 统计信息
    cout << "\nTrie统计信息:" << endl;
    cout << "  单词数量: " << trie.word_count() << endl;
    cout << "  节点数量: " << trie.node_count() << endl;
    cout << "  最大深度: " << trie.max_depth() << endl;
    cout << "  平均单词长度: " << trie.average_word_length() << endl;
    cout << "  内存使用: " << trie.memory_usage() << " 字节" << endl;
    
    // 删除测试
    cout << "\n删除测试:" << endl;
    cout << "删除 \"app\"" << endl;
    trie.erase("app");
    cout << "删除后搜索 \"app\": " << (trie.search("app") ? "存在" : "不存在") << endl;
    cout << "删除后搜索 \"apple\": " << (trie.search("apple") ? "存在" : "不存在") << endl;
    
    // 打印结构
    cout << "\nTrie结构:" << endl;
    trie.print_structure();
}

// 测试TrieMap
void test_trie_map() {
    cout << "\n=== 测试TrieMap ===" << endl;
    
    TrieMap trie_map;
    
    // 插入包含特殊字符的单词
    vector<string> words = {"hello", "world", "你好", "世界", "café", "naïve", "123", "a1b2c3"};
    
    cout << "插入单词: ";
    for (const string& word : words) {
        trie_map.insert(word);
        cout << word << " ";
    }
    cout << endl;
    
    // 搜索测试
    cout << "\n搜索测试:" << endl;
    for (const string& word : words) {
        cout << "  \"" << word << "\": " << (trie_map.search(word) ? "存在" : "不存在") << endl;
    }
    
    // 前缀查询
    cout << "\n前缀查询:" << endl;
    vector<string> prefixes = {"he", "你", "ca", "a1"};
    
    for (const string& prefix : prefixes) {
        auto words_with_prefix = trie_map.get_words_with_prefix(prefix);
        cout << "  前缀 \"" << prefix << "\": ";
        for (const string& word : words_with_prefix) {
            cout << word << " ";
        }
        cout << endl;
    }
    
    cout << "\nTrieMap统计: 单词数=" << trie_map.word_count() 
         << ", 节点数=" << trie_map.node_count() << endl;
}

// 测试压缩Trie
void test_compressed_trie() {
    cout << "\n=== 测试压缩Trie ===" << endl;
    
    CompressedTrie ctrie;
    
    // 插入单词
    vector<string> words = {"apple", "application", "apply", "appreciate", "approach", "banana", "band", "bandana"};
    
    cout << "插入单词: ";
    for (const string& word : words) {
        ctrie.insert(word);
        cout << word << " ";
    }
    cout << endl;
    
    // 搜索测试
    cout << "\n搜索测试:" << endl;
    vector<string> search_words = {"apple", "app", "application", "banana", "band"};
    
    for (const string& word : search_words) {
        bool found = ctrie.search(word);
        bool has_prefix = ctrie.starts_with(word);
        cout << "  \"" << word << "\": 存在=" << (found ? "是" : "否") 
             << ", 前缀=" << (has_prefix ? "是" : "否") << endl;
    }
    
    // 前缀查询
    cout << "\n前缀查询:" << endl;
    auto app_words = ctrie.get_words_with_prefix("app");
    cout << "  前缀 \"app\": ";
    for (const string& word : app_words) {
        cout << word << " ";
    }
    cout << endl;
    
    // 统计信息
    cout << "\n压缩Trie统计信息:" << endl;
    cout << "  单词数量: " << ctrie.word_count() << endl;
    cout << "  节点数量: " << ctrie.node_count() << endl;
    cout << "  最大深度: " << ctrie.max_depth() << endl;
    cout << "  压缩比: " << ctrie.compression_ratio() << endl;
    cout << "  总边长度: " << ctrie.total_edge_length() << endl;
    cout << "  内存使用: " << ctrie.memory_usage() << " 字节" << endl;
    
    // 打印结构
    cout << "\n压缩Trie结构:" << endl;
    ctrie.print_structure();
    
    cout << "\n压缩Trie统计:" << endl;
    ctrie.print_statistics();
}

// 测试AC自动机
void test_ac_automaton() {
    cout << "\n=== 测试AC自动机 ===" << endl;
    
    ACAutomaton ac;
    
    // 添加模式串
    vector<string> patterns = {"he", "she", "his", "hers"};
    
    cout << "添加模式串: ";
    for (const string& pattern : patterns) {
        ac.add_pattern(pattern);
        cout << pattern << " ";
    }
    cout << endl;
    
    // 构建自动机
    ac.build();
    
    // 搜索文本
    string text = "ushers";
    cout << "\n在文本 \"" << text << "\" 中搜索:" << endl;
    
    auto matches = ac.search(text);
    cout << "找到 " << matches.size() << " 个匹配:" << endl;
    
    for (const auto& match : matches) {
        cout << "  位置 [" << match.start_pos << ", " << match.end_pos 
             << "]: \"" << match.pattern << "\"" << endl;
    }
    
    // 高亮显示
    string highlighted = ac.highlight_matches(text, "[", "]");
    cout << "高亮显示: " << highlighted << endl;
    
    // 替换模式串
    string replaced = ac.replace_patterns(text, "***");
    cout << "替换后: " << replaced << endl;
    
    // 统计信息
    cout << "\nAC自动机统计:" << endl;
    ac.print_statistics();
}

// 测试敏感词过滤
void test_sensitive_word_filter() {
    cout << "\n=== 测试敏感词过滤 ===" << endl;
    
    vector<string> sensitive_words = {"bad", "evil", "hate", "stupid"};
    string text = "This is a bad example with evil content and hate speech, don't be stupid.";
    
    cout << "原文本: " << text << endl;
    cout << "敏感词: ";
    for (const string& word : sensitive_words) {
        cout << word << " ";
    }
    cout << endl;
    
    // 检查是否包含敏感词
    bool has_sensitive = contains_sensitive_words(text, sensitive_words);
    cout << "包含敏感词: " << (has_sensitive ? "是" : "否") << endl;
    
    // 过滤敏感词
    string filtered = filter_sensitive_words(text, sensitive_words, "***");
    cout << "过滤后: " << filtered << endl;
    
    // 详细匹配信息
    auto matches = multi_pattern_search(text, sensitive_words);
    cout << "\n详细匹配信息:" << endl;
    for (const auto& match : matches) {
        cout << "  位置 [" << match.start_pos << ", " << match.end_pos 
             << "]: \"" << match.pattern << "\"" << endl;
    }
}

// 性能测试
void performance_test() {
    cout << "\n=== 性能测试 ===" << endl;
    
    // 生成测试数据
    vector<string> words;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> len_dist(3, 10);
    uniform_int_distribution<> char_dist(0, 25);
    
    // 生成1000个随机单词
    for (int i = 0; i < 1000; ++i) {
        int len = len_dist(gen);
        string word;
        for (int j = 0; j < len; ++j) {
            word += 'a' + char_dist(gen);
        }
        words.push_back(word);
    }
    
    // 测试Trie性能
    auto start = high_resolution_clock::now();
    
    Trie trie;
    for (const string& word : words) {
        trie.insert(word);
    }
    
    auto end = high_resolution_clock::now();
    auto trie_insert_time = duration_cast<microseconds>(end - start).count();
    
    // 测试搜索性能
    start = high_resolution_clock::now();
    
    int found_count = 0;
    for (const string& word : words) {
        if (trie.search(word)) {
            found_count++;
        }
    }
    
    end = high_resolution_clock::now();
    auto trie_search_time = duration_cast<microseconds>(end - start).count();
    
    // 测试压缩Trie性能
    start = high_resolution_clock::now();
    
    CompressedTrie ctrie;
    for (const string& word : words) {
        ctrie.insert(word);
    }
    
    end = high_resolution_clock::now();
    auto ctrie_insert_time = duration_cast<microseconds>(end - start).count();
    
    // 测试压缩Trie搜索性能
    start = high_resolution_clock::now();
    
    int cfound_count = 0;
    for (const string& word : words) {
        if (ctrie.search(word)) {
            cfound_count++;
        }
    }
    
    end = high_resolution_clock::now();
    auto ctrie_search_time = duration_cast<microseconds>(end - start).count();
    
    // 输出结果
    cout << "\n性能测试结果 (" << words.size() << " 个单词):" << endl;
    cout << "\nTrie:" << endl;
    cout << "  插入时间: " << trie_insert_time << " 微秒" << endl;
    cout << "  搜索时间: " << trie_search_time << " 微秒" << endl;
    cout << "  找到单词: " << found_count << endl;
    cout << "  节点数量: " << trie.node_count() << endl;
    cout << "  内存使用: " << trie.memory_usage() << " 字节" << endl;
    
    cout << "\n压缩Trie:" << endl;
    cout << "  插入时间: " << ctrie_insert_time << " 微秒" << endl;
    cout << "  搜索时间: " << ctrie_search_time << " 微秒" << endl;
    cout << "  找到单词: " << cfound_count << endl;
    cout << "  节点数量: " << ctrie.node_count() << endl;
    cout << "  压缩比: " << ctrie.compression_ratio() << endl;
    cout << "  内存使用: " << ctrie.memory_usage() << " 字节" << endl;
    
    cout << "\n内存节省: " << (1.0 - static_cast<double>(ctrie.memory_usage()) / trie.memory_usage()) * 100 
         << "%" << endl;
}

// 测试Trie迭代器
void test_trie_iterator() {
    cout << "\n=== 测试Trie迭代器 ===" << endl;
    
    Trie trie;
    vector<string> words = {"apple", "app", "application", "banana", "band"};
    
    for (const string& word : words) {
        trie.insert(word);
    }
    
    cout << "使用迭代器遍历Trie中的所有单词:" << endl;
    TrieIterator iter(trie);
    
    while (iter.has_next()) {
        cout << "  " << iter.next() << endl;
    }
    
    cout << "\n重置迭代器并再次遍历:" << endl;
    iter.reset();
    
    auto all_words = iter.get_all_words();
    cout << "所有单词 (" << all_words.size() << " 个): ";
    for (const string& word : all_words) {
        cout << word << " ";
    }
    cout << endl;
}

// 测试AC自动机迭代器
void test_ac_iterator() {
    cout << "\n=== 测试AC自动机迭代器 ===" << endl;
    
    ACAutomaton ac;
    vector<string> patterns = {"he", "she", "his", "hers"};
    ac.add_patterns(patterns);
    ac.build();
    
    string text = "she sells seashells by the seashore, he helps her";
    cout << "文本: " << text << endl;
    cout << "模式串: ";
    for (const string& pattern : patterns) {
        cout << pattern << " ";
    }
    cout << endl;
    
    ACMatchIterator match_iter(ac, text);
    
    cout << "\n使用迭代器遍历所有匹配:" << endl;
    while (match_iter.has_next()) {
        auto match = match_iter.next();
        cout << "  位置 [" << match.start_pos << ", " << match.end_pos 
             << "]: \"" << match.pattern << "\"" << endl;
    }
    
    cout << "\n总匹配数: " << match_iter.size() << endl;
}

// 高级应用示例
void advanced_applications() {
    cout << "\n=== 高级应用示例 ===" << endl;
    
    // 1. 拼写检查和建议
    cout << "\n1. 拼写检查和建议:" << endl;
    
    Trie dictionary;
    vector<string> dict_words = {"apple", "application", "apply", "appreciate", "approach", "banana", "band", "bandana"};
    
    for (const string& word : dict_words) {
        dictionary.insert(word);
    }
    
    vector<string> input_words = {"aple", "aplicaton", "banan", "aproach"};
    
    for (const string& input : input_words) {
        cout << "  输入: \"" << input << "\"";
        
        if (dictionary.search(input)) {
            cout << " - 拼写正确" << endl;
        } else {
            cout << " - 拼写错误";
            
            // 简单的建议：查找相似前缀
            string prefix = input.substr(0, min(input.length(), 3UL));
            auto suggestions = dictionary.get_words_with_prefix(prefix);
            
            if (!suggestions.empty()) {
                cout << ", 建议: ";
                for (size_t i = 0; i < min(suggestions.size(), 3UL); ++i) {
                    cout << suggestions[i];
                    if (i < min(suggestions.size(), 3UL) - 1) cout << ", ";
                }
            }
            cout << endl;
        }
    }
    
    // 2. 文本分析和关键词提取
    cout << "\n2. 文本分析和关键词提取:" << endl;
    
    vector<string> keywords = {"data", "structure", "algorithm", "tree", "graph", "sort", "search"};
    string article = "This article discusses data structures and algorithms, including trees, graphs, sorting, and searching techniques.";
    
    ACAutomaton keyword_ac = build_ac_automaton(keywords);
    auto keyword_matches = keyword_ac.search(article);
    
    cout << "  文章: " << article << endl;
    cout << "  关键词: ";
    for (const string& kw : keywords) {
        cout << kw << " ";
    }
    cout << endl;
    
    auto found_keywords = keyword_ac.get_matched_patterns(article);
    cout << "  找到的关键词: ";
    for (const string& kw : found_keywords) {
        cout << kw << " ";
    }
    cout << endl;
    
    // 3. URL路由匹配
    cout << "\n3. URL路由匹配:" << endl;
    
    Trie route_trie;
    vector<string> routes = {"/api/users", "/api/users/profile", "/api/posts", "/api/posts/comments", "/admin/dashboard"};
    
    for (const string& route : routes) {
        route_trie.insert(route);
    }
    
    vector<string> requests = {"/api/users", "/api/users/profile", "/api/posts/123", "/admin", "/unknown"};
    
    for (const string& request : requests) {
        cout << "  请求: \"" << request << "\"";
        
        if (route_trie.search(request)) {
            cout << " - 精确匹配" << endl;
        } else {
            // 查找最长前缀匹配
            string longest_match = "";
            for (const string& route : routes) {
                if (request.find(route) == 0 && route.length() > longest_match.length()) {
                    longest_match = route;
                }
            }
            
            if (!longest_match.empty()) {
                cout << " - 前缀匹配: " << longest_match << endl;
            } else {
                cout << " - 无匹配" << endl;
            }
        }
    }
}

int main() {
    cout << "Trie数据结构演示程序" << endl;
    cout << "===================" << endl;
    
    try {
        test_basic_trie();
        test_trie_map();
        test_compressed_trie();
        test_ac_automaton();
        test_sensitive_word_filter();
        test_trie_iterator();
        test_ac_iterator();
        performance_test();
        advanced_applications();
        
        cout << "\n所有测试完成！" << endl;
        
    } catch (const exception& e) {
        cerr << "错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}