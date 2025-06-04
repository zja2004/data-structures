#ifndef RED_BLACK_TREE_H
#define RED_BLACK_TREE_H

#include <iostream>
#include <vector>
#include <stack>
#include <functional>
#include <algorithm>

// 红黑树节点颜色
enum class Color { RED, BLACK };

// 红黑树节点
template<typename T>
struct RBNode {
    T data;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;
    
    RBNode(const T& value, Color c = Color::RED) 
        : data(value), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
};

// 红黑树类
template<typename T>
class RedBlackTree {
private:
    RBNode<T>* root;
    RBNode<T>* nil; // 哨兵节点
    size_t node_count;
    
public:
    // 构造函数
    RedBlackTree() : node_count(0) {
        nil = new RBNode<T>(T{}, Color::BLACK);
        root = nil;
    }
    
    // 拷贝构造函数
    RedBlackTree(const RedBlackTree& other) : node_count(0) {
        nil = new RBNode<T>(T{}, Color::BLACK);
        root = nil;
        copy_tree(other.root, other.nil);
        node_count = other.node_count;
    }
    
    // 赋值操作符
    RedBlackTree& operator=(const RedBlackTree& other) {
        if (this != &other) {
            clear();
            copy_tree(other.root, other.nil);
            node_count = other.node_count;
        }
        return *this;
    }
    
    // 析构函数
    ~RedBlackTree() {
        clear();
        delete nil;
    }
    
    // 基本操作
    void insert(const T& value) {
        RBNode<T>* new_node = new RBNode<T>(value);
        new_node->left = nil;
        new_node->right = nil;
        
        rb_insert(new_node);
        ++node_count;
    }
    
    void remove(const T& value) {
        RBNode<T>* node = search_node(value);
        if (node != nil) {
            rb_delete(node);
            --node_count;
        }
    }
    
    bool contains(const T& value) const {
        return search_node(value) != nil;
    }
    
    RBNode<T>* search(const T& value) const {
        RBNode<T>* node = search_node(value);
        return (node == nil) ? nullptr : node;
    }
    
    // 状态查询
    bool empty() const {
        return root == nil;
    }
    
    size_t size() const {
        return node_count;
    }
    
    int height() const {
        return calculate_height(root);
    }
    
    int black_height() const {
        return calculate_black_height(root);
    }
    
    void clear() {
        destroy_tree(root);
        root = nil;
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
        RBNode<T>* min_node = find_minimum(root);
        return min_node->data;
    }
    
    T find_max() const {
        if (empty()) {
            throw std::runtime_error("Tree is empty");
        }
        RBNode<T>* max_node = find_maximum(root);
        return max_node->data;
    }
    
    // 前驱和后继
    T predecessor(const T& value) const {
        RBNode<T>* node = search_node(value);
        if (node == nil) {
            throw std::runtime_error("Value not found");
        }
        RBNode<T>* pred = find_predecessor(node);
        if (pred == nil) {
            throw std::runtime_error("No predecessor");
        }
        return pred->data;
    }
    
    T successor(const T& value) const {
        RBNode<T>* node = search_node(value);
        if (node == nil) {
            throw std::runtime_error("Value not found");
        }
        RBNode<T>* succ = find_successor(node);
        if (succ == nil) {
            throw std::runtime_error("No successor");
        }
        return succ->data;
    }
    
    // 范围查询
    std::vector<T> range_query(const T& min_val, const T& max_val) const {
        std::vector<T> result;
        range_search(root, min_val, max_val, result);
        return result;
    }
    
    // 验证红黑树性质
    bool is_valid_rb_tree() const {
        if (root != nil && root->color != Color::BLACK) {
            return false; // 性质2：根节点是黑色
        }
        
        int black_height_val = -1;
        return check_rb_properties(root, 0, black_height_val);
    }
    
    // 统计信息
    void print_tree_info() const {
        std::cout << "Red-Black Tree Info:" << std::endl;
        std::cout << "  Size: " << size() << std::endl;
        std::cout << "  Height: " << height() << std::endl;
        std::cout << "  Black Height: " << black_height() << std::endl;
        std::cout << "  Empty: " << (empty() ? "Yes" : "No") << std::endl;
        std::cout << "  Valid RB Tree: " << (is_valid_rb_tree() ? "Yes" : "No") << std::endl;
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
    RBNode<T>* get_root() const {
        return (root == nil) ? nullptr : root;
    }
    
private:
    // 左旋转
    void left_rotate(RBNode<T>* x) {
        RBNode<T>* y = x->right;
        x->right = y->left;
        
        if (y->left != nil) {
            y->left->parent = x;
        }
        
        y->parent = x->parent;
        
        if (x->parent == nil) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        
        y->left = x;
        x->parent = y;
    }
    
    // 右旋转
    void right_rotate(RBNode<T>* y) {
        RBNode<T>* x = y->left;
        y->left = x->right;
        
        if (x->right != nil) {
            x->right->parent = y;
        }
        
        x->parent = y->parent;
        
        if (y->parent == nil) {
            root = x;
        } else if (y == y->parent->right) {
            y->parent->right = x;
        } else {
            y->parent->left = x;
        }
        
        x->right = y;
        y->parent = x;
    }
    
    // 红黑树插入
    void rb_insert(RBNode<T>* z) {
        RBNode<T>* y = nil;
        RBNode<T>* x = root;
        
        // 标准BST插入
        while (x != nil) {
            y = x;
            if (z->data < x->data) {
                x = x->left;
            } else if (z->data > x->data) {
                x = x->right;
            } else {
                // 值已存在，不插入
                delete z;
                --node_count; // 因为外部已经增加了
                return;
            }
        }
        
        z->parent = y;
        
        if (y == nil) {
            root = z;
        } else if (z->data < y->data) {
            y->left = z;
        } else {
            y->right = z;
        }
        
        z->color = Color::RED;
        rb_insert_fixup(z);
    }
    
    // 插入修复
    void rb_insert_fixup(RBNode<T>* z) {
        while (z->parent != nil && z->parent->color == Color::RED) {
            if (z->parent == z->parent->parent->left) {
                RBNode<T>* y = z->parent->parent->right; // 叔叔节点
                
                if (y->color == Color::RED) {
                    // 情况1：叔叔是红色
                    z->parent->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->right) {
                        // 情况2：叔叔是黑色，z是右孩子
                        z = z->parent;
                        left_rotate(z);
                    }
                    // 情况3：叔叔是黑色，z是左孩子
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    right_rotate(z->parent->parent);
                }
            } else {
                // 对称情况
                RBNode<T>* y = z->parent->parent->left; // 叔叔节点
                
                if (y->color == Color::RED) {
                    z->parent->color = Color::BLACK;
                    y->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        right_rotate(z);
                    }
                    z->parent->color = Color::BLACK;
                    z->parent->parent->color = Color::RED;
                    left_rotate(z->parent->parent);
                }
            }
        }
        root->color = Color::BLACK;
    }
    
    // 红黑树删除
    void rb_delete(RBNode<T>* z) {
        RBNode<T>* y = z;
        RBNode<T>* x;
        Color y_original_color = y->color;
        
        if (z->left == nil) {
            x = z->right;
            rb_transplant(z, z->right);
        } else if (z->right == nil) {
            x = z->left;
            rb_transplant(z, z->left);
        } else {
            y = find_minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            
            if (y->parent == z) {
                x->parent = y;
            } else {
                rb_transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            
            rb_transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        
        delete z;
        
        if (y_original_color == Color::BLACK) {
            rb_delete_fixup(x);
        }
    }
    
    // 删除修复
    void rb_delete_fixup(RBNode<T>* x) {
        while (x != root && x->color == Color::BLACK) {
            if (x == x->parent->left) {
                RBNode<T>* w = x->parent->right; // 兄弟节点
                
                if (w->color == Color::RED) {
                    // 情况1：兄弟是红色
                    w->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    left_rotate(x->parent);
                    w = x->parent->right;
                }
                
                if (w->left->color == Color::BLACK && w->right->color == Color::BLACK) {
                    // 情况2：兄弟的两个孩子都是黑色
                    w->color = Color::RED;
                    x = x->parent;
                } else {
                    if (w->right->color == Color::BLACK) {
                        // 情况3：兄弟的左孩子是红色，右孩子是黑色
                        w->left->color = Color::BLACK;
                        w->color = Color::RED;
                        right_rotate(w);
                        w = x->parent->right;
                    }
                    // 情况4：兄弟的右孩子是红色
                    w->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    w->right->color = Color::BLACK;
                    left_rotate(x->parent);
                    x = root;
                }
            } else {
                // 对称情况
                RBNode<T>* w = x->parent->left;
                
                if (w->color == Color::RED) {
                    w->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    right_rotate(x->parent);
                    w = x->parent->left;
                }
                
                if (w->right->color == Color::BLACK && w->left->color == Color::BLACK) {
                    w->color = Color::RED;
                    x = x->parent;
                } else {
                    if (w->left->color == Color::BLACK) {
                        w->right->color = Color::BLACK;
                        w->color = Color::RED;
                        left_rotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    w->left->color = Color::BLACK;
                    right_rotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = Color::BLACK;
    }
    
    // 子树替换
    void rb_transplant(RBNode<T>* u, RBNode<T>* v) {
        if (u->parent == nil) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }
    
    // 搜索节点
    RBNode<T>* search_node(const T& value) const {
        RBNode<T>* current = root;
        while (current != nil && current->data != value) {
            if (value < current->data) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return current;
    }
    
    // 查找最小节点
    RBNode<T>* find_minimum(RBNode<T>* node) const {
        while (node != nil && node->left != nil) {
            node = node->left;
        }
        return node;
    }
    
    // 查找最大节点
    RBNode<T>* find_maximum(RBNode<T>* node) const {
        while (node != nil && node->right != nil) {
            node = node->right;
        }
        return node;
    }
    
    // 查找前驱
    RBNode<T>* find_predecessor(RBNode<T>* node) const {
        if (node->left != nil) {
            return find_maximum(node->left);
        }
        
        RBNode<T>* y = node->parent;
        while (y != nil && node == y->left) {
            node = y;
            y = y->parent;
        }
        return y;
    }
    
    // 查找后继
    RBNode<T>* find_successor(RBNode<T>* node) const {
        if (node->right != nil) {
            return find_minimum(node->right);
        }
        
        RBNode<T>* y = node->parent;
        while (y != nil && node == y->right) {
            node = y;
            y = y->parent;
        }
        return y;
    }
    
    // 范围搜索
    void range_search(RBNode<T>* node, const T& min_val, const T& max_val, 
                     std::vector<T>& result) const {
        if (node == nil) return;
        
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
    
    // 遍历函数
    void inorder_recursive(RBNode<T>* node, std::vector<T>& result) const {
        if (node != nil) {
            inorder_recursive(node->left, result);
            result.push_back(node->data);
            inorder_recursive(node->right, result);
        }
    }
    
    void preorder_recursive(RBNode<T>* node, std::vector<T>& result) const {
        if (node != nil) {
            result.push_back(node->data);
            preorder_recursive(node->left, result);
            preorder_recursive(node->right, result);
        }
    }
    
    void postorder_recursive(RBNode<T>* node, std::vector<T>& result) const {
        if (node != nil) {
            postorder_recursive(node->left, result);
            postorder_recursive(node->right, result);
            result.push_back(node->data);
        }
    }
    
    // 计算高度
    int calculate_height(RBNode<T>* node) const {
        if (node == nil) return 0;
        return 1 + std::max(calculate_height(node->left), calculate_height(node->right));
    }
    
    // 计算黑高度
    int calculate_black_height(RBNode<T>* node) const {
        if (node == nil) return 0;
        
        int left_bh = calculate_black_height(node->left);
        int right_bh = calculate_black_height(node->right);
        
        int bh = std::max(left_bh, right_bh);
        if (node->color == Color::BLACK) {
            bh++;
        }
        
        return bh;
    }
    
    // 复制树
    void copy_tree(RBNode<T>* other_node, RBNode<T>* other_nil) {
        if (other_node != other_nil) {
            insert(other_node->data);
            copy_tree(other_node->left, other_nil);
            copy_tree(other_node->right, other_nil);
        }
    }
    
    // 销毁树
    void destroy_tree(RBNode<T>* node) {
        if (node != nil) {
            destroy_tree(node->left);
            destroy_tree(node->right);
            delete node;
        }
    }
    
    // 验证红黑树性质
    bool check_rb_properties(RBNode<T>* node, int black_count, int& black_height) const {
        if (node == nil) {
            if (black_height == -1) {
                black_height = black_count;
            }
            return black_count == black_height;
        }
        
        // 性质4：红节点的孩子必须是黑色
        if (node->color == Color::RED) {
            if ((node->left != nil && node->left->color == Color::RED) ||
                (node->right != nil && node->right->color == Color::RED)) {
                return false;
            }
        }
        
        int new_black_count = black_count;
        if (node->color == Color::BLACK) {
            new_black_count++;
        }
        
        return check_rb_properties(node->left, new_black_count, black_height) &&
               check_rb_properties(node->right, new_black_count, black_height);
    }
    
    // 打印树结构
    void print_tree_structure(RBNode<T>* node, int depth) const {
        if (node == nil) return;
        
        print_tree_structure(node->right, depth + 1);
        
        for (int i = 0; i < depth; ++i) {
            std::cout << "    ";
        }
        std::cout << node->data << "(" 
                  << (node->color == Color::RED ? "R" : "B") << ")" << std::endl;
        
        print_tree_structure(node->left, depth + 1);
    }
};

// 红黑树迭代器
template<typename T>
class RBIterator {
private:
    std::stack<RBNode<T>*> stack;
    RBNode<T>* nil;
    
    void push_left(RBNode<T>* node) {
        while (node != nil) {
            stack.push(node);
            node = node->left;
        }
    }
    
public:
    RBIterator(RBNode<T>* root, RBNode<T>* nil_node) : nil(nil_node) {
        push_left(root);
    }
    
    bool has_next() const {
        return !stack.empty();
    }
    
    T next() {
        if (!has_next()) {
            throw std::runtime_error("No more elements");
        }
        
        RBNode<T>* node = stack.top();
        stack.pop();
        
        push_left(node->right);
        
        return node->data;
    }
};

// 实用函数

// 从有序数组构建红黑树
template<typename T>
RedBlackTree<T> build_rb_from_sorted_array(const std::vector<T>& sorted_array) {
    RedBlackTree<T> tree;
    for (const T& value : sorted_array) {
        tree.insert(value);
    }
    return tree;
}

// 合并两个红黑树
template<typename T>
RedBlackTree<T> merge_rb_trees(const RedBlackTree<T>& tree1, const RedBlackTree<T>& tree2) {
    std::vector<T> seq1 = tree1.inorder();
    std::vector<T> seq2 = tree2.inorder();
    
    std::vector<T> merged;
    std::merge(seq1.begin(), seq1.end(), seq2.begin(), seq2.end(), 
               std::back_inserter(merged));
    
    // 去重
    merged.erase(std::unique(merged.begin(), merged.end()), merged.end());
    
    return build_rb_from_sorted_array(merged);
}

#endif // RED_BLACK_TREE_H