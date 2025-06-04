#ifndef PERSISTENT_SEGMENT_TREE_H
#define PERSISTENT_SEGMENT_TREE_H

#include <memory>
#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <climits>
#include <stdexcept>
#include <map>

using namespace std;

// 可持久化线段树节点
template<typename T>
struct PersistentSegmentTreeNode {
    T value;
    shared_ptr<PersistentSegmentTreeNode> left;
    shared_ptr<PersistentSegmentTreeNode> right;
    
    PersistentSegmentTreeNode(T val = T{}) 
        : value(val), left(nullptr), right(nullptr) {}
    
    PersistentSegmentTreeNode(T val, shared_ptr<PersistentSegmentTreeNode> l, shared_ptr<PersistentSegmentTreeNode> r)
        : value(val), left(l), right(r) {}
};

// 可持久化线段树模板类
template<typename T>
class PersistentSegmentTree {
private:
    using NodePtr = shared_ptr<PersistentSegmentTreeNode<T>>;
    
    vector<NodePtr> roots;  // 存储所有版本的根节点
    int n;  // 数组大小
    function<T(T, T)> combine;
    T default_value;
    
    // 创建新节点
    NodePtr create_node(T val = T{}, NodePtr left = nullptr, NodePtr right = nullptr) {
        return make_shared<PersistentSegmentTreeNode<T>>(val, left, right);
    }
    
    // 构建初始线段树
    NodePtr build(const vector<T>& arr, int start, int end) {
        if (start == end) {
            return create_node(arr[start]);
        }
        
        int mid = start + (end - start) / 2;
        NodePtr left_child = build(arr, start, mid);
        NodePtr right_child = build(arr, mid + 1, end);
        
        T combined_value = combine(left_child->value, right_child->value);
        return create_node(combined_value, left_child, right_child);
    }
    
    // 单点更新，返回新的根节点
    NodePtr update(NodePtr node, int start, int end, int idx, T val) {
        if (start == end) {
            return create_node(val);
        }
        
        int mid = start + (end - start) / 2;
        NodePtr new_left = node->left;
        NodePtr new_right = node->right;
        
        if (idx <= mid) {
            new_left = update(node->left, start, mid, idx, val);
        } else {
            new_right = update(node->right, mid + 1, end, idx, val);
        }
        
        T combined_value = combine(new_left->value, new_right->value);
        return create_node(combined_value, new_left, new_right);
    }
    
    // 区间查询
    T query(NodePtr node, int start, int end, int l, int r) {
        if (!node || l > end || r < start) {
            return default_value;
        }
        
        if (l <= start && end <= r) {
            return node->value;
        }
        
        int mid = start + (end - start) / 2;
        T left_result = query(node->left, start, mid, l, r);
        T right_result = query(node->right, mid + 1, end, l, r);
        
        return combine(left_result, right_result);
    }
    
    // 单点查询
    T query_point(NodePtr node, int start, int end, int idx) {
        if (!node) return default_value;
        
        if (start == end) {
            return node->value;
        }
        
        int mid = start + (end - start) / 2;
        if (idx <= mid) {
            return query_point(node->left, start, mid, idx);
        } else {
            return query_point(node->right, mid + 1, end, idx);
        }
    }
    
    // 计算节点数量
    int count_nodes(NodePtr node) const {
        if (!node) return 0;
        return 1 + count_nodes(node->left) + count_nodes(node->right);
    }
    
    // 打印树结构
    void print_tree(NodePtr node, int start, int end, int depth) {
        if (!node) return;
        
        for (int i = 0; i < depth; ++i) {
            cout << "  ";
        }
        
        cout << "[" << start << ", " << end << "]: " << node->value << endl;
        
        if (start != end) {
            int mid = start + (end - start) / 2;
            print_tree(node->left, start, mid, depth + 1);
            print_tree(node->right, mid + 1, end, depth + 1);
        }
    }
    
    // 获取数组表示
    void get_array(NodePtr node, int start, int end, vector<T>& result) {
        if (!node) return;
        
        if (start == end) {
            result[start] = node->value;
            return;
        }
        
        int mid = start + (end - start) / 2;
        get_array(node->left, start, mid, result);
        get_array(node->right, mid + 1, end, result);
    }
    
    // 合并两个版本的线段树
    NodePtr merge_trees(NodePtr node1, NodePtr node2, int start, int end) {
        if (!node1 && !node2) return nullptr;
        if (!node1) return node2;
        if (!node2) return node1;
        
        if (start == end) {
            return create_node(combine(node1->value, node2->value));
        }
        
        int mid = start + (end - start) / 2;
        NodePtr left_child = merge_trees(node1->left, node2->left, start, mid);
        NodePtr right_child = merge_trees(node1->right, node2->right, mid + 1, end);
        
        T combined_value = combine(
            left_child ? left_child->value : default_value,
            right_child ? right_child->value : default_value
        );
        
        return create_node(combined_value, left_child, right_child);
    }
    
public:
    // 构造函数
    PersistentSegmentTree(const vector<T>& arr,
                         function<T(T, T)> combine_func = [](T a, T b) { return a + b; },
                         T default_val = T{})
        : n(arr.size()), combine(combine_func), default_value(default_val) {
        
        if (n == 0) {
            throw invalid_argument("Array cannot be empty");
        }
        
        // 构建初始版本
        roots.push_back(build(arr, 0, n - 1));
    }
    
    // 从大小构造
    PersistentSegmentTree(int size, T initial_value = T{},
                         function<T(T, T)> combine_func = [](T a, T b) { return a + b; },
                         T default_val = T{})
        : n(size), combine(combine_func), default_value(default_val) {
        
        if (n <= 0) {
            throw invalid_argument("Size must be positive");
        }
        
        vector<T> arr(n, initial_value);
        roots.push_back(build(arr, 0, n - 1));
    }
    
    // 拷贝构造函数
    PersistentSegmentTree(const PersistentSegmentTree& other)
        : roots(other.roots), n(other.n), combine(other.combine), default_value(other.default_value) {}
    
    // 赋值操作符
    PersistentSegmentTree& operator=(const PersistentSegmentTree& other) {
        if (this != &other) {
            roots = other.roots;
            n = other.n;
            combine = other.combine;
            default_value = other.default_value;
        }
        return *this;
    }
    
    // 移动构造函数
    PersistentSegmentTree(PersistentSegmentTree&& other) noexcept
        : roots(move(other.roots)), n(other.n), combine(move(other.combine)), default_value(move(other.default_value)) {
        other.n = 0;
    }
    
    // 移动赋值操作符
    PersistentSegmentTree& operator=(PersistentSegmentTree&& other) noexcept {
        if (this != &other) {
            roots = move(other.roots);
            n = other.n;
            combine = move(other.combine);
            default_value = move(other.default_value);
            other.n = 0;
        }
        return *this;
    }
    
    // 单点更新，创建新版本
    int update(int version, int idx, T val) {
        if (version < 0 || version >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        if (idx < 0 || idx >= n) {
            throw out_of_range("Index out of range");
        }
        
        NodePtr new_root = update(roots[version], 0, n - 1, idx, val);
        roots.push_back(new_root);
        return roots.size() - 1;  // 返回新版本号
    }
    
    // 区间查询
    T query(int version, int l, int r) {
        if (version < 0 || version >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        if (l < 0 || r >= n || l > r) {
            throw out_of_range("Invalid query range");
        }
        
        return query(roots[version], 0, n - 1, l, r);
    }
    
    // 单点查询
    T query(int version, int idx) {
        if (version < 0 || version >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        if (idx < 0 || idx >= n) {
            throw out_of_range("Index out of range");
        }
        
        return query_point(roots[version], 0, n - 1, idx);
    }
    
    // 获取版本数量
    int version_count() const {
        return roots.size();
    }
    
    // 获取数组大小
    int size() const {
        return n;
    }
    
    // 获取某个版本的数组表示
    vector<T> get_array(int version) {
        if (version < 0 || version >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        
        vector<T> result(n);
        get_array(roots[version], 0, n - 1, result);
        return result;
    }
    
    // 合并两个版本，创建新版本
    int merge_versions(int version1, int version2) {
        if (version1 < 0 || version1 >= roots.size() ||
            version2 < 0 || version2 >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        
        NodePtr merged_root = merge_trees(roots[version1], roots[version2], 0, n - 1);
        roots.push_back(merged_root);
        return roots.size() - 1;
    }
    
    // 回滚到某个版本（删除之后的版本）
    void rollback_to_version(int version) {
        if (version < 0 || version >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        
        roots.erase(roots.begin() + version + 1, roots.end());
    }
    
    // 创建分支（复制某个版本）
    int create_branch(int version) {
        if (version < 0 || version >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        
        roots.push_back(roots[version]);
        return roots.size() - 1;
    }
    
    // 计算某个版本的节点数量
    int node_count(int version) const {
        if (version < 0 || version >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        
        return count_nodes(roots[version]);
    }
    
    // 计算总节点数量
    int total_node_count() const {
        int total = 0;
        for (const auto& root : roots) {
            total += count_nodes(root);
        }
        return total;
    }
    
    // 打印某个版本的树结构
    void print_version(int version) {
        if (version < 0 || version >= roots.size()) {
            throw out_of_range("Invalid version");
        }
        
        cout << "Persistent Segment Tree Version " << version << ":" << endl;
        print_tree(roots[version], 0, n - 1, 0);
    }
    
    // 打印所有版本信息
    void print_all_versions() {
        cout << "Persistent Segment Tree (" << roots.size() << " versions):" << endl;
        for (int i = 0; i < roots.size(); ++i) {
            cout << "Version " << i << ": " << node_count(i) << " nodes" << endl;
        }
    }
    
    // 内存使用估算
    size_t memory_usage() const {
        return total_node_count() * sizeof(PersistentSegmentTreeNode<T>);
    }
    
    // 清空所有版本
    void clear() {
        roots.clear();
    }
};

// 特化的可持久化线段树类型

// 可持久化区间和线段树
template<typename T>
class PersistentSumSegmentTree : public PersistentSegmentTree<T> {
public:
    PersistentSumSegmentTree(const vector<T>& arr)
        : PersistentSegmentTree<T>(arr, [](T a, T b) { return a + b; }, T{}) {}
    
    PersistentSumSegmentTree(int size, T initial_value = T{})
        : PersistentSegmentTree<T>(size, initial_value, [](T a, T b) { return a + b; }, T{}) {}
};

// 可持久化区间最大值线段树
template<typename T>
class PersistentMaxSegmentTree : public PersistentSegmentTree<T> {
public:
    PersistentMaxSegmentTree(const vector<T>& arr)
        : PersistentSegmentTree<T>(arr, [](T a, T b) { return max(a, b); }, numeric_limits<T>::lowest()) {}
    
    PersistentMaxSegmentTree(int size, T initial_value = numeric_limits<T>::lowest())
        : PersistentSegmentTree<T>(size, initial_value, [](T a, T b) { return max(a, b); }, numeric_limits<T>::lowest()) {}
};

// 可持久化区间最小值线段树
template<typename T>
class PersistentMinSegmentTree : public PersistentSegmentTree<T> {
public:
    PersistentMinSegmentTree(const vector<T>& arr)
        : PersistentSegmentTree<T>(arr, [](T a, T b) { return min(a, b); }, numeric_limits<T>::max()) {}
    
    PersistentMinSegmentTree(int size, T initial_value = numeric_limits<T>::max())
        : PersistentSegmentTree<T>(size, initial_value, [](T a, T b) { return min(a, b); }, numeric_limits<T>::max()) {}
};

// 可持久化数组（基于可持久化线段树）
template<typename T>
class PersistentArray {
private:
    PersistentSegmentTree<T> tree;
    
public:
    // 构造函数
    PersistentArray(const vector<T>& arr)
        : tree(arr, [](T a, T b) { return a; }, T{}) {}  // 使用第一个值作为合并结果
    
    PersistentArray(int size, T initial_value = T{})
        : tree(size, initial_value, [](T a, T b) { return a; }, T{}) {}
    
    // 设置元素，返回新版本
    int set(int version, int idx, T val) {
        return tree.update(version, idx, val);
    }
    
    // 获取元素
    T get(int version, int idx) {
        return tree.query(version, idx);
    }
    
    // 获取版本数量
    int version_count() const {
        return tree.version_count();
    }
    
    // 获取数组大小
    int size() const {
        return tree.size();
    }
    
    // 获取某个版本的完整数组
    vector<T> get_array(int version) {
        return tree.get_array(version);
    }
    
    // 创建分支
    int create_branch(int version) {
        return tree.create_branch(version);
    }
    
    // 回滚到某个版本
    void rollback_to_version(int version) {
        tree.rollback_to_version(version);
    }
};

// 可持久化栈
template<typename T>
class PersistentStack {
private:
    struct StackNode {
        T value;
        shared_ptr<StackNode> next;
        
        StackNode(T val, shared_ptr<StackNode> n = nullptr) : value(val), next(n) {}
    };
    
    vector<shared_ptr<StackNode>> versions;
    
public:
    // 构造函数
    PersistentStack() {
        versions.push_back(nullptr);  // 空栈版本
    }
    
    // 压栈，返回新版本
    int push(int version, T val) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        
        auto new_node = make_shared<StackNode>(val, versions[version]);
        versions.push_back(new_node);
        return versions.size() - 1;
    }
    
    // 弹栈，返回新版本
    int pop(int version) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        if (!versions[version]) {
            throw runtime_error("Cannot pop from empty stack");
        }
        
        versions.push_back(versions[version]->next);
        return versions.size() - 1;
    }
    
    // 获取栈顶元素
    T top(int version) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        if (!versions[version]) {
            throw runtime_error("Stack is empty");
        }
        
        return versions[version]->value;
    }
    
    // 检查栈是否为空
    bool empty(int version) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        
        return versions[version] == nullptr;
    }
    
    // 获取栈大小
    int size(int version) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        
        int count = 0;
        auto current = versions[version];
        while (current) {
            count++;
            current = current->next;
        }
        return count;
    }
    
    // 获取版本数量
    int version_count() const {
        return versions.size();
    }
    
    // 获取栈的所有元素（从栈顶到栈底）
    vector<T> get_elements(int version) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        
        vector<T> result;
        auto current = versions[version];
        while (current) {
            result.push_back(current->value);
            current = current->next;
        }
        return result;
    }
    
    // 创建分支
    int create_branch(int version) {
        if (version < 0 || version >= versions.size()) {
            throw out_of_range("Invalid version");
        }
        
        versions.push_back(versions[version]);
        return versions.size() - 1;
    }
};

// 实用函数

// 构建可持久化求和线段树
template<typename T>
PersistentSumSegmentTree<T> build_persistent_sum_tree(const vector<T>& arr) {
    return PersistentSumSegmentTree<T>(arr);
}

// 构建可持久化最大值线段树
template<typename T>
PersistentMaxSegmentTree<T> build_persistent_max_tree(const vector<T>& arr) {
    return PersistentMaxSegmentTree<T>(arr);
}

// 构建可持久化最小值线段树
template<typename T>
PersistentMinSegmentTree<T> build_persistent_min_tree(const vector<T>& arr) {
    return PersistentMinSegmentTree<T>(arr);
}

// 版本管理器
template<typename T>
class VersionManager {
private:
    map<string, int> named_versions;  // 命名版本
    vector<string> version_descriptions;  // 版本描述
    PersistentSegmentTree<T>* tree;
    
public:
    VersionManager(PersistentSegmentTree<T>* segment_tree) : tree(segment_tree) {
        version_descriptions.push_back("Initial version");
    }
    
    // 创建命名版本
    void create_named_version(const string& name, int version) {
        if (version < 0 || version >= tree->version_count()) {
            throw out_of_range("Invalid version");
        }
        named_versions[name] = version;
    }
    
    // 获取命名版本
    int get_named_version(const string& name) {
        auto it = named_versions.find(name);
        if (it == named_versions.end()) {
            throw invalid_argument("Named version not found");
        }
        return it->second;
    }
    
    // 添加版本描述
    void add_version_description(const string& description) {
        version_descriptions.push_back(description);
    }
    
    // 获取版本描述
    string get_version_description(int version) {
        if (version < 0 || version >= version_descriptions.size()) {
            return "No description";
        }
        return version_descriptions[version];
    }
    
    // 打印版本信息
    void print_version_info() {
        cout << "Version Information:" << endl;
        for (int i = 0; i < tree->version_count(); ++i) {
            cout << "Version " << i << ": " << get_version_description(i) << endl;
        }
        
        cout << "\nNamed Versions:" << endl;
        for (const auto& pair : named_versions) {
            cout << "  " << pair.first << " -> Version " << pair.second << endl;
        }
    }
};

#endif // PERSISTENT_SEGMENT_TREE_H