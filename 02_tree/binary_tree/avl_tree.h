#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "binary_search_tree.h"
#include <algorithm>
#include <iostream>

// AVL树节点
template<typename T>
struct AVLNode {
    T data;
    AVLNode* left;
    AVLNode* right;
    int height;
    
    AVLNode(const T& value) : data(value), left(nullptr), right(nullptr), height(1) {}
};

// AVL树类
template<typename T>
class AVLTree {
private:
    AVLNode<T>* root;
    size_t node_count;
    
public:
    // 构造函数
    AVLTree() : root(nullptr), node_count(0) {}
    
    // 拷贝构造函数
    AVLTree(const AVLTree& other) : root(nullptr), node_count(0) {
        root = copy_tree(other.root);
        node_count = other.node_count;
    }
    
    // 赋值操作符
    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            clear();
            root = copy_tree(other.root);
            node_count = other.node_count;
        }
        return *this;
    }
    
    // 析构函数
    ~AVLTree() {
        clear();
    }
    
    // 基本操作
    void insert(const T& value) {
        root = insert_recursive(root, value);
    }
    
    void remove(const T& value) {
        root = remove_recursive(root, value);
    }
    
    bool contains(const T& value) const {
        return search(root, value) != nullptr;
    }
    
    AVLNode<T>* search(const T& value) const {
        return search(root, value);
    }
    
    // 状态查询
    bool empty() const {
        return root == nullptr;
    }
    
    size_t size() const {
        return node_count;
    }
    
    int height() const {
        return get_height(root);
    }
    
    void clear() {
        destroy_tree(root);
        root = nullptr;
        node_count = 0;
    }
    
    // 遍历操作
    std::vector<T> inorder() const {
        std::vector<T> result;
        inorder_recursive(root, result);
        return result;
    }
    
    std::vector<T> preorder() const {
        std::vector<T> result;
        preorder_recursive(root, result);
        return result;
    }
    
    std::vector<T> postorder() const {
        std::vector<T> result;
        postorder_recursive(root, result);
        return result;
    }
    
    // 查找最值
    T find_min() const {
        if (empty()) {
            throw std::runtime_error("Tree is empty");
        }
        AVLNode<T>* min_node = find_min_node(root);
        return min_node->data;
    }
    
    T find_max() const {
        if (empty()) {
            throw std::runtime_error("Tree is empty");
        }
        AVLNode<T>* max_node = find_max_node(root);
        return max_node->data;
    }
    
    // 范围查询
    std::vector<T> range_query(const T& min_val, const T& max_val) const {
        std::vector<T> result;
        range_search(root, min_val, max_val, result);
        return result;
    }
    
    // 验证AVL性质
    bool is_valid_avl() const {
        return check_avl_property(root);
    }
    
    // 获取平衡因子
    int get_balance_factor(AVLNode<T>* node) const {
        if (!node) return 0;
        return get_height(node->left) - get_height(node->right);
    }
    
    // 统计信息
    void print_tree_info() const {
        std::cout << "AVL Tree Info:" << std::endl;
        std::cout << "  Size: " << size() << std::endl;
        std::cout << "  Height: " << height() << std::endl;
        std::cout << "  Empty: " << (empty() ? "Yes" : "No") << std::endl;
        std::cout << "  Valid AVL: " << (is_valid_avl() ? "Yes" : "No") << std::endl;
    }
    
    // 打印树结构
    void print_tree() const {
        if (empty()) {
            std::cout << "(empty tree)" << std::endl;
            return;
        }
        print_tree_structure(root, 0);
    }
    
    // 获取根节点（用于测试）
    AVLNode<T>* get_root() const {
        return root;
    }
    
private:
    // 获取节点高度
    int get_height(AVLNode<T>* node) const {
        return node ? node->height : 0;
    }
    
    // 更新节点高度
    void update_height(AVLNode<T>* node) {
        if (node) {
            node->height = 1 + std::max(get_height(node->left), get_height(node->right));
        }
    }
    
    // 右旋转
    AVLNode<T>* rotate_right(AVLNode<T>* y) {
        AVLNode<T>* x = y->left;
        AVLNode<T>* T2 = x->right;
        
        // 执行旋转
        x->right = y;
        y->left = T2;
        
        // 更新高度
        update_height(y);
        update_height(x);
        
        return x;
    }
    
    // 左旋转
    AVLNode<T>* rotate_left(AVLNode<T>* x) {
        AVLNode<T>* y = x->right;
        AVLNode<T>* T2 = y->left;
        
        // 执行旋转
        y->left = x;
        x->right = T2;
        
        // 更新高度
        update_height(x);
        update_height(y);
        
        return y;
    }
    
    // 递归插入
    AVLNode<T>* insert_recursive(AVLNode<T>* node, const T& value) {
        // 1. 执行标准BST插入
        if (!node) {
            ++node_count;
            return new AVLNode<T>(value);
        }
        
        if (value < node->data) {
            node->left = insert_recursive(node->left, value);
        } else if (value > node->data) {
            node->right = insert_recursive(node->right, value);
        } else {
            // 相等值，不插入
            return node;
        }
        
        // 2. 更新当前节点的高度
        update_height(node);
        
        // 3. 获取平衡因子
        int balance = get_balance_factor(node);
        
        // 4. 如果不平衡，有4种情况需要处理
        
        // Left Left Case
        if (balance > 1 && value < node->left->data) {
            return rotate_right(node);
        }
        
        // Right Right Case
        if (balance < -1 && value > node->right->data) {
            return rotate_left(node);
        }
        
        // Left Right Case
        if (balance > 1 && value > node->left->data) {
            node->left = rotate_left(node->left);
            return rotate_right(node);
        }
        
        // Right Left Case
        if (balance < -1 && value < node->right->data) {
            node->right = rotate_right(node->right);
            return rotate_left(node);
        }
        
        return node;
    }
    
    // 递归删除
    AVLNode<T>* remove_recursive(AVLNode<T>* node, const T& value) {
        // 1. 执行标准BST删除
        if (!node) return node;
        
        if (value < node->data) {
            node->left = remove_recursive(node->left, value);
        } else if (value > node->data) {
            node->right = remove_recursive(node->right, value);
        } else {
            // 找到要删除的节点
            --node_count;
            
            if (!node->left || !node->right) {
                AVLNode<T>* temp = node->left ? node->left : node->right;
                
                if (!temp) {
                    // 没有子节点
                    temp = node;
                    node = nullptr;
                } else {
                    // 一个子节点
                    *node = *temp;
                }
                delete temp;
            } else {
                // 两个子节点：获取中序后继
                AVLNode<T>* temp = find_min_node(node->right);
                
                // 复制后继的数据到当前节点
                node->data = temp->data;
                
                // 删除后继
                ++node_count; // 先加回来，因为递归会减
                node->right = remove_recursive(node->right, temp->data);
            }
        }
        
        if (!node) return node;
        
        // 2. 更新当前节点的高度
        update_height(node);
        
        // 3. 获取平衡因子
        int balance = get_balance_factor(node);
        
        // 4. 如果不平衡，有4种情况需要处理
        
        // Left Left Case
        if (balance > 1 && get_balance_factor(node->left) >= 0) {
            return rotate_right(node);
        }
        
        // Left Right Case
        if (balance > 1 && get_balance_factor(node->left) < 0) {
            node->left = rotate_left(node->left);
            return rotate_right(node);
        }
        
        // Right Right Case
        if (balance < -1 && get_balance_factor(node->right) <= 0) {
            return rotate_left(node);
        }
        
        // Right Left Case
        if (balance < -1 && get_balance_factor(node->right) > 0) {
            node->right = rotate_right(node->right);
            return rotate_left(node);
        }
        
        return node;
    }
    
    // 搜索节点
    AVLNode<T>* search(AVLNode<T>* node, const T& value) const {
        if (!node || node->data == value) {
            return node;
        }
        
        if (value < node->data) {
            return search(node->left, value);
        } else {
            return search(node->right, value);
        }
    }
    
    // 查找最小节点
    AVLNode<T>* find_min_node(AVLNode<T>* node) const {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }
    
    // 查找最大节点
    AVLNode<T>* find_max_node(AVLNode<T>* node) const {
        while (node && node->right) {
            node = node->right;
        }
        return node;
    }
    
    // 范围搜索
    void range_search(AVLNode<T>* node, const T& min_val, const T& max_val, 
                     std::vector<T>& result) const {
        if (!node) return;
        
        if (node->data >= min_val && node->data <= max_val) {
            result.push_back(node->data);
        }
        
        if (node->data > min_val) {
            range_search(node->left, min_val, max_val, result);
        }
        
        if (node->data < max_val) {
            range_search(node->right, min_val, max_val, result);
        }
    }
    
    // 中序遍历
    void inorder_recursive(AVLNode<T>* node, std::vector<T>& result) const {
        if (node) {
            inorder_recursive(node->left, result);
            result.push_back(node->data);
            inorder_recursive(node->right, result);
        }
    }
    
    // 前序遍历
    void preorder_recursive(AVLNode<T>* node, std::vector<T>& result) const {
        if (node) {
            result.push_back(node->data);
            preorder_recursive(node->left, result);
            preorder_recursive(node->right, result);
        }
    }
    
    // 后序遍历
    void postorder_recursive(AVLNode<T>* node, std::vector<T>& result) const {
        if (node) {
            postorder_recursive(node->left, result);
            postorder_recursive(node->right, result);
            result.push_back(node->data);
        }
    }
    
    // 复制树
    AVLNode<T>* copy_tree(AVLNode<T>* node) {
        if (!node) return nullptr;
        
        AVLNode<T>* new_node = new AVLNode<T>(node->data);
        new_node->height = node->height;
        new_node->left = copy_tree(node->left);
        new_node->right = copy_tree(node->right);
        
        return new_node;
    }
    
    // 销毁树
    void destroy_tree(AVLNode<T>* node) {
        if (node) {
            destroy_tree(node->left);
            destroy_tree(node->right);
            delete node;
        }
    }
    
    // 验证AVL性质
    bool check_avl_property(AVLNode<T>* node) const {
        if (!node) return true;
        
        int balance = get_balance_factor(node);
        if (abs(balance) > 1) {
            return false;
        }
        
        return check_avl_property(node->left) && check_avl_property(node->right);
    }
    
    // 打印树结构
    void print_tree_structure(AVLNode<T>* node, int depth) const {
        if (!node) return;
        
        print_tree_structure(node->right, depth + 1);
        
        for (int i = 0; i < depth; ++i) {
            std::cout << "    ";
        }
        std::cout << node->data << "(h:" << node->height 
                  << ", b:" << get_balance_factor(node) << ")" << std::endl;
        
        print_tree_structure(node->left, depth + 1);
    }
};

// AVL树迭代器
template<typename T>
class AVLIterator {
private:
    std::stack<AVLNode<T>*> stack;
    
    void push_left(AVLNode<T>* node) {
        while (node) {
            stack.push(node);
            node = node->left;
        }
    }
    
public:
    AVLIterator(AVLNode<T>* root) {
        push_left(root);
    }
    
    bool has_next() const {
        return !stack.empty();
    }
    
    T next() {
        if (!has_next()) {
            throw std::runtime_error("No more elements");
        }
        
        AVLNode<T>* node = stack.top();
        stack.pop();
        
        push_left(node->right);
        
        return node->data;
    }
};

// 实用函数

// 从有序数组构建AVL树
template<typename T>
AVLTree<T> build_avl_from_sorted_array(const std::vector<T>& sorted_array) {
    AVLTree<T> tree;
    for (const T& value : sorted_array) {
        tree.insert(value);
    }
    return tree;
}

// 合并两个AVL树
template<typename T>
AVLTree<T> merge_avl_trees(const AVLTree<T>& tree1, const AVLTree<T>& tree2) {
    std::vector<T> seq1 = tree1.inorder();
    std::vector<T> seq2 = tree2.inorder();
    
    std::vector<T> merged;
    std::merge(seq1.begin(), seq1.end(), seq2.begin(), seq2.end(), 
               std::back_inserter(merged));
    
    // 去重
    merged.erase(std::unique(merged.begin(), merged.end()), merged.end());
    
    return build_avl_from_sorted_array(merged);
}

// 比较两个AVL树是否相同
template<typename T>
bool are_identical_avl(const AVLTree<T>& tree1, const AVLTree<T>& tree2) {
    std::function<bool(AVLNode<T>*, AVLNode<T>*)> compare = 
        [&](AVLNode<T>* node1, AVLNode<T>* node2) -> bool {
        if (!node1 && !node2) return true;
        if (!node1 || !node2) return false;
        
        return node1->data == node2->data &&
               compare(node1->left, node2->left) &&
               compare(node1->right, node2->right);
    };
    
    return compare(tree1.get_root(), tree2.get_root());
}

#endif // AVL_TREE_H