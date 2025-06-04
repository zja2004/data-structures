#ifndef BTREE_H
#define BTREE_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <stack>

using namespace std;

// B树节点
template<typename T, int MinDegree>
struct BTreeNode {
    static_assert(MinDegree >= 2, "MinDegree must be at least 2");
    
    vector<T> keys;                    // 键值数组
    vector<BTreeNode*> children;       // 子节点指针数组
    bool is_leaf;                      // 是否为叶子节点
    int key_count;                     // 当前键值数量
    
    static const int MAX_KEYS = 2 * MinDegree - 1;     // 最大键数
    static const int MIN_KEYS = MinDegree - 1;          // 最小键数
    static const int MAX_CHILDREN = 2 * MinDegree;      // 最大子节点数
    static const int MIN_CHILDREN = MinDegree;          // 最小子节点数
    
    BTreeNode(bool leaf = true) : is_leaf(leaf), key_count(0) {
        keys.resize(MAX_KEYS);
        if (!is_leaf) {
            children.resize(MAX_CHILDREN, nullptr);
        }
    }
    
    ~BTreeNode() {
        if (!is_leaf) {
            for (int i = 0; i <= key_count; ++i) {
                delete children[i];
            }
        }
    }
    
    // 查找键值在节点中的位置
    int find_key_position(const T& key) {
        int pos = 0;
        while (pos < key_count && keys[pos] < key) {
            pos++;
        }
        return pos;
    }
    
    // 检查节点是否已满
    bool is_full() const {
        return key_count == MAX_KEYS;
    }
    
    // 检查节点是否键数过少
    bool is_minimal() const {
        return key_count == MIN_KEYS;
    }
    
    // 在指定位置插入键值
    void insert_key_at(int pos, const T& key) {
        for (int i = key_count; i > pos; --i) {
            keys[i] = keys[i - 1];
        }
        keys[pos] = key;
        key_count++;
    }
    
    // 在指定位置插入子节点
    void insert_child_at(int pos, BTreeNode* child) {
        for (int i = key_count + 1; i > pos; --i) {
            children[i] = children[i - 1];
        }
        children[pos] = child;
    }
    
    // 删除指定位置的键值
    void remove_key_at(int pos) {
        for (int i = pos; i < key_count - 1; ++i) {
            keys[i] = keys[i + 1];
        }
        key_count--;
    }
    
    // 删除指定位置的子节点
    void remove_child_at(int pos) {
        for (int i = pos; i < key_count; ++i) {
            children[i] = children[i + 1];
        }
        children[key_count] = nullptr;
    }
};

// B树类
template<typename T, int MinDegree = 3>
class BTree {
private:
    using Node = BTreeNode<T, MinDegree>;
    Node* root;
    size_t tree_size;
    
    // 分裂满节点
    void split_child(Node* parent, int index) {
        Node* full_child = parent->children[index];
        Node* new_child = new Node(full_child->is_leaf);
        
        // 将后半部分键值移动到新节点
        new_child->key_count = MinDegree - 1;
        for (int i = 0; i < MinDegree - 1; ++i) {
            new_child->keys[i] = full_child->keys[i + MinDegree];
        }
        
        // 如果不是叶子节点，也要移动子节点
        if (!full_child->is_leaf) {
            for (int i = 0; i < MinDegree; ++i) {
                new_child->children[i] = full_child->children[i + MinDegree];
            }
        }
        
        // 更新原节点的键数
        full_child->key_count = MinDegree - 1;
        
        // 将中间键值提升到父节点
        T middle_key = full_child->keys[MinDegree - 1];
        
        // 在父节点中插入新的子节点
        parent->insert_child_at(index + 1, new_child);
        parent->insert_key_at(index, middle_key);
    }
    
    // 在非满节点中插入键值
    void insert_non_full(Node* node, const T& key) {
        int pos = node->key_count - 1;
        
        if (node->is_leaf) {
            // 叶子节点：直接插入
            while (pos >= 0 && key < node->keys[pos]) {
                node->keys[pos + 1] = node->keys[pos];
                pos--;
            }
            node->keys[pos + 1] = key;
            node->key_count++;
        } else {
            // 内部节点：找到合适的子节点
            while (pos >= 0 && key < node->keys[pos]) {
                pos--;
            }
            pos++;
            
            // 如果子节点已满，先分裂
            if (node->children[pos]->is_full()) {
                split_child(node, pos);
                if (key > node->keys[pos]) {
                    pos++;
                }
            }
            
            insert_non_full(node->children[pos], key);
        }
    }
    
    // 搜索键值
    pair<Node*, int> search_helper(Node* node, const T& key) {
        if (!node) {
            return {nullptr, -1};
        }
        
        int pos = node->find_key_position(key);
        
        // 找到键值
        if (pos < node->key_count && node->keys[pos] == key) {
            return {node, pos};
        }
        
        // 叶子节点但未找到
        if (node->is_leaf) {
            return {nullptr, -1};
        }
        
        // 递归搜索子节点
        return search_helper(node->children[pos], key);
    }
    
    // 从子节点借键
    void borrow_from_prev(Node* parent, int index) {
        Node* child = parent->children[index];
        Node* sibling = parent->children[index - 1];
        
        // 将父节点的键值下移到子节点
        for (int i = child->key_count; i > 0; --i) {
            child->keys[i] = child->keys[i - 1];
        }
        child->keys[0] = parent->keys[index - 1];
        
        // 如果不是叶子节点，也要移动子节点指针
        if (!child->is_leaf) {
            for (int i = child->key_count + 1; i > 0; --i) {
                child->children[i] = child->children[i - 1];
            }
            child->children[0] = sibling->children[sibling->key_count];
        }
        
        // 将兄弟节点的最大键值上移到父节点
        parent->keys[index - 1] = sibling->keys[sibling->key_count - 1];
        
        child->key_count++;
        sibling->key_count--;
    }
    
    void borrow_from_next(Node* parent, int index) {
        Node* child = parent->children[index];
        Node* sibling = parent->children[index + 1];
        
        // 将父节点的键值下移到子节点
        child->keys[child->key_count] = parent->keys[index];
        
        // 如果不是叶子节点，也要移动子节点指针
        if (!child->is_leaf) {
            child->children[child->key_count + 1] = sibling->children[0];
        }
        
        // 将兄弟节点的最小键值上移到父节点
        parent->keys[index] = sibling->keys[0];
        
        // 移动兄弟节点的键值和子节点
        for (int i = 0; i < sibling->key_count - 1; ++i) {
            sibling->keys[i] = sibling->keys[i + 1];
        }
        
        if (!sibling->is_leaf) {
            for (int i = 0; i < sibling->key_count; ++i) {
                sibling->children[i] = sibling->children[i + 1];
            }
        }
        
        child->key_count++;
        sibling->key_count--;
    }
    
    // 合并节点
    void merge(Node* parent, int index) {
        Node* child = parent->children[index];
        Node* sibling = parent->children[index + 1];
        
        // 将父节点的键值下移到子节点
        child->keys[MinDegree - 1] = parent->keys[index];
        
        // 将兄弟节点的键值复制到子节点
        for (int i = 0; i < sibling->key_count; ++i) {
            child->keys[i + MinDegree] = sibling->keys[i];
        }
        
        // 如果不是叶子节点，也要复制子节点指针
        if (!child->is_leaf) {
            for (int i = 0; i <= sibling->key_count; ++i) {
                child->children[i + MinDegree] = sibling->children[i];
            }
        }
        
        // 更新键数
        child->key_count += sibling->key_count + 1;
        
        // 从父节点中删除键值和子节点指针
        parent->remove_key_at(index);
        parent->remove_child_at(index + 1);
        
        // 删除兄弟节点（但不删除其子节点，因为已经转移）
        sibling->key_count = 0;  // 防止析构函数删除子节点
        delete sibling;
    }
    
    // 获取前驱键值
    T get_predecessor(Node* node, int index) {
        Node* current = node->children[index];
        while (!current->is_leaf) {
            current = current->children[current->key_count];
        }
        return current->keys[current->key_count - 1];
    }
    
    // 获取后继键值
    T get_successor(Node* node, int index) {
        Node* current = node->children[index + 1];
        while (!current->is_leaf) {
            current = current->children[0];
        }
        return current->keys[0];
    }
    
    // 删除键值的辅助函数
    void remove_helper(Node* node, const T& key) {
        int pos = node->find_key_position(key);
        
        if (pos < node->key_count && node->keys[pos] == key) {
            // 情况1：键值在当前节点中
            if (node->is_leaf) {
                // 情况1a：叶子节点，直接删除
                node->remove_key_at(pos);
            } else {
                // 情况1b：内部节点
                if (node->children[pos]->key_count >= MinDegree) {
                    // 情况1b1：左子节点有足够的键
                    T predecessor = get_predecessor(node, pos);
                    node->keys[pos] = predecessor;
                    remove_helper(node->children[pos], predecessor);
                } else if (node->children[pos + 1]->key_count >= MinDegree) {
                    // 情况1b2：右子节点有足够的键
                    T successor = get_successor(node, pos);
                    node->keys[pos] = successor;
                    remove_helper(node->children[pos + 1], successor);
                } else {
                    // 情况1b3：两个子节点都只有最少键数，合并
                    merge(node, pos);
                    remove_helper(node->children[pos], key);
                }
            }
        } else {
            // 情况2：键值不在当前节点中
            if (node->is_leaf) {
                // 键值不存在
                return;
            }
            
            bool is_last_child = (pos == node->key_count);
            
            if (node->children[pos]->key_count < MinDegree) {
                // 子节点键数不足，需要调整
                if (pos > 0 && node->children[pos - 1]->key_count >= MinDegree) {
                    // 从前一个兄弟借键
                    borrow_from_prev(node, pos);
                } else if (pos < node->key_count && node->children[pos + 1]->key_count >= MinDegree) {
                    // 从后一个兄弟借键
                    borrow_from_next(node, pos);
                } else {
                    // 合并节点
                    if (pos < node->key_count) {
                        merge(node, pos);
                    } else {
                        merge(node, pos - 1);
                        pos--;
                    }
                }
            }
            
            // 递归删除
            if (is_last_child && pos > node->key_count) {
                remove_helper(node->children[pos - 1], key);
            } else {
                remove_helper(node->children[pos], key);
            }
        }
    }
    
    // 中序遍历辅助函数
    void inorder_helper(Node* node, function<void(const T&)> visit) {
        if (!node) return;
        
        for (int i = 0; i < node->key_count; ++i) {
            if (!node->is_leaf) {
                inorder_helper(node->children[i], visit);
            }
            visit(node->keys[i]);
        }
        
        if (!node->is_leaf) {
            inorder_helper(node->children[node->key_count], visit);
        }
    }
    
    // 计算树的高度
    int height_helper(Node* node) {
        if (!node) return 0;
        if (node->is_leaf) return 1;
        return 1 + height_helper(node->children[0]);
    }
    
    // 计算节点数量
    size_t count_nodes_helper(Node* node) {
        if (!node) return 0;
        
        size_t count = 1;
        if (!node->is_leaf) {
            for (int i = 0; i <= node->key_count; ++i) {
                count += count_nodes_helper(node->children[i]);
            }
        }
        return count;
    }
    
    // 验证B树性质
    bool validate_helper(Node* node, const T* min_key, const T* max_key) {
        if (!node) return true;
        
        // 检查键数范围
        if (node != root && node->key_count < MinDegree - 1) {
            return false;
        }
        if (node->key_count > 2 * MinDegree - 1) {
            return false;
        }
        
        // 检查键值有序性
        for (int i = 0; i < node->key_count - 1; ++i) {
            if (node->keys[i] >= node->keys[i + 1]) {
                return false;
            }
        }
        
        // 检查键值范围
        if (min_key && node->keys[0] <= *min_key) {
            return false;
        }
        if (max_key && node->keys[node->key_count - 1] >= *max_key) {
            return false;
        }
        
        // 递归检查子节点
        if (!node->is_leaf) {
            for (int i = 0; i <= node->key_count; ++i) {
                const T* child_min = (i == 0) ? min_key : &node->keys[i - 1];
                const T* child_max = (i == node->key_count) ? max_key : &node->keys[i];
                
                if (!validate_helper(node->children[i], child_min, child_max)) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    // 复制子树
    Node* copy_tree(Node* node) {
        if (!node) return nullptr;
        
        Node* new_node = new Node(node->is_leaf);
        new_node->key_count = node->key_count;
        
        for (int i = 0; i < node->key_count; ++i) {
            new_node->keys[i] = node->keys[i];
        }
        
        if (!node->is_leaf) {
            for (int i = 0; i <= node->key_count; ++i) {
                new_node->children[i] = copy_tree(node->children[i]);
            }
        }
        
        return new_node;
    }
    
public:
    // 构造函数
    BTree() : root(nullptr), tree_size(0) {}
    
    // 拷贝构造函数
    BTree(const BTree& other) : root(copy_tree(other.root)), tree_size(other.tree_size) {}
    
    // 移动构造函数
    BTree(BTree&& other) noexcept : root(other.root), tree_size(other.tree_size) {
        other.root = nullptr;
        other.tree_size = 0;
    }
    
    // 拷贝赋值运算符
    BTree& operator=(const BTree& other) {
        if (this != &other) {
            clear();
            root = copy_tree(other.root);
            tree_size = other.tree_size;
        }
        return *this;
    }
    
    // 移动赋值运算符
    BTree& operator=(BTree&& other) noexcept {
        if (this != &other) {
            clear();
            root = other.root;
            tree_size = other.tree_size;
            other.root = nullptr;
            other.tree_size = 0;
        }
        return *this;
    }
    
    // 析构函数
    ~BTree() {
        clear();
    }
    
    // 插入键值
    void insert(const T& key) {
        if (!root) {
            root = new Node(true);
            root->keys[0] = key;
            root->key_count = 1;
        } else {
            if (root->is_full()) {
                Node* new_root = new Node(false);
                new_root->children[0] = root;
                split_child(new_root, 0);
                root = new_root;
            }
            insert_non_full(root, key);
        }
        tree_size++;
    }
    
    // 搜索键值
    bool search(const T& key) {
        auto result = search_helper(root, key);
        return result.first != nullptr;
    }
    
    // 删除键值
    void remove(const T& key) {
        if (!root) return;
        
        remove_helper(root, key);
        
        // 如果根节点为空，更新根节点
        if (root->key_count == 0) {
            Node* old_root = root;
            if (root->is_leaf) {
                root = nullptr;
            } else {
                root = root->children[0];
            }
            old_root->key_count = 0;  // 防止析构函数删除子节点
            delete old_root;
        }
        
        if (tree_size > 0) {
            tree_size--;
        }
    }
    
    // 中序遍历
    void inorder_traverse(function<void(const T&)> visit) {
        inorder_helper(root, visit);
    }
    
    // 层序遍历
    void level_order_traverse(function<void(const T&)> visit) {
        if (!root) return;
        
        queue<Node*> q;
        q.push(root);
        
        while (!q.empty()) {
            Node* node = q.front();
            q.pop();
            
            for (int i = 0; i < node->key_count; ++i) {
                visit(node->keys[i]);
            }
            
            if (!node->is_leaf) {
                for (int i = 0; i <= node->key_count; ++i) {
                    q.push(node->children[i]);
                }
            }
        }
    }
    
    // 范围查询
    vector<T> range_search(const T& min_key, const T& max_key) {
        vector<T> result;
        inorder_traverse([&](const T& key) {
            if (key >= min_key && key <= max_key) {
                result.push_back(key);
            }
        });
        return result;
    }
    
    // 获取最小值
    T get_min() {
        if (!root) {
            throw runtime_error("Tree is empty");
        }
        
        Node* current = root;
        while (!current->is_leaf) {
            current = current->children[0];
        }
        return current->keys[0];
    }
    
    // 获取最大值
    T get_max() {
        if (!root) {
            throw runtime_error("Tree is empty");
        }
        
        Node* current = root;
        while (!current->is_leaf) {
            current = current->children[current->key_count];
        }
        return current->keys[current->key_count - 1];
    }
    
    // 检查是否为空
    bool empty() const {
        return root == nullptr;
    }
    
    // 获取大小
    size_t size() const {
        return tree_size;
    }
    
    // 获取高度
    int height() {
        return height_helper(root);
    }
    
    // 获取节点数量
    size_t node_count() {
        return count_nodes_helper(root);
    }
    
    // 清空树
    void clear() {
        delete root;
        root = nullptr;
        tree_size = 0;
    }
    
    // 验证B树性质
    bool validate() {
        return validate_helper(root, nullptr, nullptr);
    }
    
    // 打印树结构
    void print_structure() {
        if (!root) {
            cout << "Empty tree\n";
            return;
        }
        
        queue<pair<Node*, int>> q;
        q.push({root, 0});
        int current_level = 0;
        
        while (!q.empty()) {
            auto [node, level] = q.front();
            q.pop();
            
            if (level > current_level) {
                cout << "\n";
                current_level = level;
            }
            
            cout << "[";
            for (int i = 0; i < node->key_count; ++i) {
                if (i > 0) cout << ",";
                cout << node->keys[i];
            }
            cout << "] ";
            
            if (!node->is_leaf) {
                for (int i = 0; i <= node->key_count; ++i) {
                    q.push({node->children[i], level + 1});
                }
            }
        }
        cout << "\n";
    }
    
    // 获取统计信息
    struct Statistics {
        size_t total_keys;
        size_t total_nodes;
        int tree_height;
        double avg_keys_per_node;
        double space_utilization;
    };
    
    Statistics get_statistics() {
        Statistics stats;
        stats.total_keys = tree_size;
        stats.total_nodes = node_count();
        stats.tree_height = height();
        stats.avg_keys_per_node = stats.total_nodes > 0 ? 
            static_cast<double>(stats.total_keys) / stats.total_nodes : 0.0;
        stats.space_utilization = stats.total_nodes > 0 ? 
            stats.avg_keys_per_node / (2 * MinDegree - 1) : 0.0;
        
        return stats;
    }
};

// 类型别名
template<typename T>
using BTree3 = BTree<T, 3>;  // 2-3-4树

template<typename T>
using BTree5 = BTree<T, 5>;  // 4-5-6-7-8-9树

template<typename T>
using BTree10 = BTree<T, 10>; // 高分支因子B树

// 实用函数

// 从数组构建B树
template<typename T, int MinDegree>
BTree<T, MinDegree> build_btree(const vector<T>& arr) {
    BTree<T, MinDegree> tree;
    for (const T& val : arr) {
        tree.insert(val);
    }
    return tree;
}

// B树排序
template<typename T, int MinDegree>
vector<T> btree_sort(vector<T> arr) {
    BTree<T, MinDegree> tree = build_btree<T, MinDegree>(arr);
    
    vector<T> result;
    result.reserve(arr.size());
    
    tree.inorder_traverse([&result](const T& key) {
        result.push_back(key);
    });
    
    return result;
}

// 合并两个B树
template<typename T, int MinDegree>
BTree<T, MinDegree> merge_btrees(BTree<T, MinDegree>& tree1, BTree<T, MinDegree>& tree2) {
    BTree<T, MinDegree> result;
    
    tree1.inorder_traverse([&result](const T& key) {
        result.insert(key);
    });
    
    tree2.inorder_traverse([&result](const T& key) {
        result.insert(key);
    });
    
    return result;
}

// 查找第K小元素
template<typename T, int MinDegree>
T find_kth_smallest(BTree<T, MinDegree>& tree, size_t k) {
    if (k == 0 || k > tree.size()) {
        throw out_of_range("k is out of range");
    }
    
    size_t count = 0;
    T result;
    
    tree.inorder_traverse([&](const T& key) {
        count++;
        if (count == k) {
            result = key;
        }
    });
    
    return result;
}

#endif // BTREE_H