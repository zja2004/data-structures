#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>
#include <climits>

// 二叉树节点定义
template<typename T>
struct TreeNode {
    T data;
    TreeNode* left;
    TreeNode* right;
    
    TreeNode(const T& value) 
        : data(value), left(nullptr), right(nullptr) {}
    
    TreeNode(const T& value, TreeNode* l, TreeNode* r) 
        : data(value), left(l), right(r) {}
};

// 基本二叉树类
template<typename T>
class BinaryTree {
protected:
    TreeNode<T>* root;
    size_t node_count;
    
public:
    // 构造函数
    BinaryTree() : root(nullptr), node_count(0) {}
    
    // 从数组构造（层序遍历，空节点用特殊值表示）
    BinaryTree(const std::vector<T>& level_order, const T& null_value) 
        : root(nullptr), node_count(0) {
        build_from_level_order(level_order, null_value);
    }
    
    // 拷贝构造函数
    BinaryTree(const BinaryTree& other) : root(nullptr), node_count(0) {
        if (other.root) {
            root = copy_tree(other.root);
            node_count = other.node_count;
        }
    }
    
    // 赋值操作符
    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            clear();
            if (other.root) {
                root = copy_tree(other.root);
                node_count = other.node_count;
            }
        }
        return *this;
    }
    
    // 析构函数
    virtual ~BinaryTree() {
        clear();
    }
    
    // 基本操作
    bool empty() const {
        return root == nullptr;
    }
    
    size_t size() const {
        return node_count;
    }
    
    void clear() {
        destroy_tree(root);
        root = nullptr;
        node_count = 0;
    }
    
    TreeNode<T>* get_root() const {
        return root;
    }
    
    // 树的属性
    int height() const {
        return calculate_height(root);
    }
    
    int depth(TreeNode<T>* node) const {
        return calculate_depth(root, node, 0);
    }
    
    bool is_balanced() const {
        return check_balanced(root).first;
    }
    
    bool is_complete() const {
        return check_complete();
    }
    
    bool is_full() const {
        return check_full(root);
    }
    
    // 查找操作
    TreeNode<T>* find(const T& value) const {
        return find_node(root, value);
    }
    
    bool contains(const T& value) const {
        return find(value) != nullptr;
    }
    
    // 遍历方法（递归版本）
    void preorder_traversal(std::function<void(const T&)> visit) const {
        preorder_recursive(root, visit);
    }
    
    void inorder_traversal(std::function<void(const T&)> visit) const {
        inorder_recursive(root, visit);
    }
    
    void postorder_traversal(std::function<void(const T&)> visit) const {
        postorder_recursive(root, visit);
    }
    
    void level_order_traversal(std::function<void(const T&)> visit) const {
        level_order_iterative(visit);
    }
    
    // 遍历方法（迭代版本）
    void preorder_iterative(std::function<void(const T&)> visit) const {
        if (!root) return;
        
        std::stack<TreeNode<T>*> stack;
        stack.push(root);
        
        while (!stack.empty()) {
            TreeNode<T>* node = stack.top();
            stack.pop();
            
            visit(node->data);
            
            if (node->right) stack.push(node->right);
            if (node->left) stack.push(node->left);
        }
    }
    
    void inorder_iterative(std::function<void(const T&)> visit) const {
        if (!root) return;
        
        std::stack<TreeNode<T>*> stack;
        TreeNode<T>* current = root;
        
        while (current || !stack.empty()) {
            while (current) {
                stack.push(current);
                current = current->left;
            }
            
            current = stack.top();
            stack.pop();
            visit(current->data);
            
            current = current->right;
        }
    }
    
    void postorder_iterative(std::function<void(const T&)> visit) const {
        if (!root) return;
        
        std::stack<TreeNode<T>*> stack1, stack2;
        stack1.push(root);
        
        while (!stack1.empty()) {
            TreeNode<T>* node = stack1.top();
            stack1.pop();
            stack2.push(node);
            
            if (node->left) stack1.push(node->left);
            if (node->right) stack1.push(node->right);
        }
        
        while (!stack2.empty()) {
            visit(stack2.top()->data);
            stack2.pop();
        }
    }
    
    // 获取遍历结果
    std::vector<T> preorder() const {
        std::vector<T> result;
        preorder_traversal([&result](const T& value) {
            result.push_back(value);
        });
        return result;
    }
    
    std::vector<T> inorder() const {
        std::vector<T> result;
        inorder_traversal([&result](const T& value) {
            result.push_back(value);
        });
        return result;
    }
    
    std::vector<T> postorder() const {
        std::vector<T> result;
        postorder_traversal([&result](const T& value) {
            result.push_back(value);
        });
        return result;
    }
    
    std::vector<T> level_order() const {
        std::vector<T> result;
        level_order_traversal([&result](const T& value) {
            result.push_back(value);
        });
        return result;
    }
    
    // 层序遍历（分层）
    std::vector<std::vector<T>> level_order_by_levels() const {
        std::vector<std::vector<T>> result;
        if (!root) return result;
        
        std::queue<TreeNode<T>*> queue;
        queue.push(root);
        
        while (!queue.empty()) {
            int level_size = queue.size();
            std::vector<T> current_level;
            
            for (int i = 0; i < level_size; ++i) {
                TreeNode<T>* node = queue.front();
                queue.pop();
                
                current_level.push_back(node->data);
                
                if (node->left) queue.push(node->left);
                if (node->right) queue.push(node->right);
            }
            
            result.push_back(current_level);
        }
        
        return result;
    }
    
    // 路径相关
    std::vector<std::vector<T>> all_paths() const {
        std::vector<std::vector<T>> paths;
        std::vector<T> current_path;
        find_all_paths(root, current_path, paths);
        return paths;
    }
    
    std::vector<T> path_to_node(TreeNode<T>* target) const {
        std::vector<T> path;
        find_path_to_node(root, target, path);
        return path;
    }
    
    // 最近公共祖先
    TreeNode<T>* lowest_common_ancestor(TreeNode<T>* p, TreeNode<T>* q) const {
        return find_lca(root, p, q);
    }
    
    // 树的直径
    int diameter() const {
        int max_diameter = 0;
        calculate_diameter(root, max_diameter);
        return max_diameter;
    }
    
    // 输出函数
    void print() const {
        std::cout << "Binary Tree (size: " << size() << ", height: " << height() << "):" << std::endl;
        if (empty()) {
            std::cout << "(empty)" << std::endl;
            return;
        }
        
        print_tree(root, 0);
    }
    
    void print_level_order() const {
        std::cout << "Level order: ";
        auto levels = level_order_by_levels();
        for (size_t i = 0; i < levels.size(); ++i) {
            std::cout << "[";
            for (size_t j = 0; j < levels[i].size(); ++j) {
                std::cout << levels[i][j];
                if (j < levels[i].size() - 1) std::cout << ", ";
            }
            std::cout << "]";
            if (i < levels.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;
    }
    
    void print_traversals() const {
        auto pre = preorder();
        auto in = inorder();
        auto post = postorder();
        auto level = level_order();
        
        std::cout << "Preorder:  [";
        for (size_t i = 0; i < pre.size(); ++i) {
            std::cout << pre[i];
            if (i < pre.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "Inorder:   [";
        for (size_t i = 0; i < in.size(); ++i) {
            std::cout << in[i];
            if (i < in.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "Postorder: [";
        for (size_t i = 0; i < post.size(); ++i) {
            std::cout << post[i];
            if (i < post.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
        
        std::cout << "Level:     [";
        for (size_t i = 0; i < level.size(); ++i) {
            std::cout << level[i];
            if (i < level.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
protected:
    // 辅助函数
    void destroy_tree(TreeNode<T>* node) {
        if (node) {
            destroy_tree(node->left);
            destroy_tree(node->right);
            delete node;
        }
    }
    
    TreeNode<T>* copy_tree(TreeNode<T>* node) {
        if (!node) return nullptr;
        
        TreeNode<T>* new_node = new TreeNode<T>(node->data);
        new_node->left = copy_tree(node->left);
        new_node->right = copy_tree(node->right);
        
        return new_node;
    }
    
    int calculate_height(TreeNode<T>* node) const {
        if (!node) return -1;
        
        int left_height = calculate_height(node->left);
        int right_height = calculate_height(node->right);
        
        return std::max(left_height, right_height) + 1;
    }
    
    int calculate_depth(TreeNode<T>* current, TreeNode<T>* target, int depth) const {
        if (!current) return -1;
        if (current == target) return depth;
        
        int left_depth = calculate_depth(current->left, target, depth + 1);
        if (left_depth != -1) return left_depth;
        
        return calculate_depth(current->right, target, depth + 1);
    }
    
    std::pair<bool, int> check_balanced(TreeNode<T>* node) const {
        if (!node) return {true, -1};
        
        auto left_result = check_balanced(node->left);
        if (!left_result.first) return {false, 0};
        
        auto right_result = check_balanced(node->right);
        if (!right_result.first) return {false, 0};
        
        int height_diff = std::abs(left_result.second - right_result.second);
        bool balanced = height_diff <= 1;
        int height = std::max(left_result.second, right_result.second) + 1;
        
        return {balanced, height};
    }
    
    bool check_complete() const {
        if (!root) return true;
        
        std::queue<TreeNode<T>*> queue;
        queue.push(root);
        bool found_null = false;
        
        while (!queue.empty()) {
            TreeNode<T>* node = queue.front();
            queue.pop();
            
            if (!node) {
                found_null = true;
            } else {
                if (found_null) return false;
                queue.push(node->left);
                queue.push(node->right);
            }
        }
        
        return true;
    }
    
    bool check_full(TreeNode<T>* node) const {
        if (!node) return true;
        
        if ((!node->left && node->right) || (node->left && !node->right)) {
            return false;
        }
        
        return check_full(node->left) && check_full(node->right);
    }
    
    TreeNode<T>* find_node(TreeNode<T>* node, const T& value) const {
        if (!node) return nullptr;
        if (node->data == value) return node;
        
        TreeNode<T>* left_result = find_node(node->left, value);
        if (left_result) return left_result;
        
        return find_node(node->right, value);
    }
    
    void preorder_recursive(TreeNode<T>* node, std::function<void(const T&)> visit) const {
        if (node) {
            visit(node->data);
            preorder_recursive(node->left, visit);
            preorder_recursive(node->right, visit);
        }
    }
    
    void inorder_recursive(TreeNode<T>* node, std::function<void(const T&)> visit) const {
        if (node) {
            inorder_recursive(node->left, visit);
            visit(node->data);
            inorder_recursive(node->right, visit);
        }
    }
    
    void postorder_recursive(TreeNode<T>* node, std::function<void(const T&)> visit) const {
        if (node) {
            postorder_recursive(node->left, visit);
            postorder_recursive(node->right, visit);
            visit(node->data);
        }
    }
    
    void level_order_iterative(std::function<void(const T&)> visit) const {
        if (!root) return;
        
        std::queue<TreeNode<T>*> queue;
        queue.push(root);
        
        while (!queue.empty()) {
            TreeNode<T>* node = queue.front();
            queue.pop();
            
            visit(node->data);
            
            if (node->left) queue.push(node->left);
            if (node->right) queue.push(node->right);
        }
    }
    
    void find_all_paths(TreeNode<T>* node, std::vector<T>& current_path, 
                       std::vector<std::vector<T>>& paths) const {
        if (!node) return;
        
        current_path.push_back(node->data);
        
        if (!node->left && !node->right) {
            paths.push_back(current_path);
        } else {
            find_all_paths(node->left, current_path, paths);
            find_all_paths(node->right, current_path, paths);
        }
        
        current_path.pop_back();
    }
    
    bool find_path_to_node(TreeNode<T>* current, TreeNode<T>* target, 
                          std::vector<T>& path) const {
        if (!current) return false;
        
        path.push_back(current->data);
        
        if (current == target) return true;
        
        if (find_path_to_node(current->left, target, path) ||
            find_path_to_node(current->right, target, path)) {
            return true;
        }
        
        path.pop_back();
        return false;
    }
    
    TreeNode<T>* find_lca(TreeNode<T>* node, TreeNode<T>* p, TreeNode<T>* q) const {
        if (!node || node == p || node == q) return node;
        
        TreeNode<T>* left = find_lca(node->left, p, q);
        TreeNode<T>* right = find_lca(node->right, p, q);
        
        if (left && right) return node;
        return left ? left : right;
    }
    
    int calculate_diameter(TreeNode<T>* node, int& max_diameter) const {
        if (!node) return 0;
        
        int left_height = calculate_diameter(node->left, max_diameter);
        int right_height = calculate_diameter(node->right, max_diameter);
        
        max_diameter = std::max(max_diameter, left_height + right_height);
        
        return std::max(left_height, right_height) + 1;
    }
    
    void print_tree(TreeNode<T>* node, int depth) const {
        if (!node) return;
        
        print_tree(node->right, depth + 1);
        
        for (int i = 0; i < depth; ++i) {
            std::cout << "    ";
        }
        std::cout << node->data << std::endl;
        
        print_tree(node->left, depth + 1);
    }
    
    void build_from_level_order(const std::vector<T>& level_order, const T& null_value) {
        if (level_order.empty() || level_order[0] == null_value) return;
        
        root = new TreeNode<T>(level_order[0]);
        node_count = 1;
        
        std::queue<TreeNode<T>*> queue;
        queue.push(root);
        
        size_t i = 1;
        while (!queue.empty() && i < level_order.size()) {
            TreeNode<T>* current = queue.front();
            queue.pop();
            
            // 左子节点
            if (i < level_order.size() && level_order[i] != null_value) {
                current->left = new TreeNode<T>(level_order[i]);
                queue.push(current->left);
                ++node_count;
            }
            ++i;
            
            // 右子节点
            if (i < level_order.size() && level_order[i] != null_value) {
                current->right = new TreeNode<T>(level_order[i]);
                queue.push(current->right);
                ++node_count;
            }
            ++i;
        }
    }
};

// 从前序和中序构造二叉树
template<typename T>
TreeNode<T>* build_tree_from_preorder_inorder(const std::vector<T>& preorder, 
                                             const std::vector<T>& inorder) {
    if (preorder.empty() || inorder.empty() || preorder.size() != inorder.size()) {
        return nullptr;
    }
    
    std::function<TreeNode<T>*(int, int, int, int)> build = 
        [&](int pre_start, int pre_end, int in_start, int in_end) -> TreeNode<T>* {
        if (pre_start > pre_end) return nullptr;
        
        T root_val = preorder[pre_start];
        TreeNode<T>* root = new TreeNode<T>(root_val);
        
        // 在中序遍历中找到根节点位置
        int root_index = in_start;
        for (int i = in_start; i <= in_end; ++i) {
            if (inorder[i] == root_val) {
                root_index = i;
                break;
            }
        }
        
        int left_size = root_index - in_start;
        
        root->left = build(pre_start + 1, pre_start + left_size, 
                          in_start, root_index - 1);
        root->right = build(pre_start + left_size + 1, pre_end, 
                           root_index + 1, in_end);
        
        return root;
    };
    
    return build(0, preorder.size() - 1, 0, inorder.size() - 1);
}

// 从后序和中序构造二叉树
template<typename T>
TreeNode<T>* build_tree_from_postorder_inorder(const std::vector<T>& postorder, 
                                              const std::vector<T>& inorder) {
    if (postorder.empty() || inorder.empty() || postorder.size() != inorder.size()) {
        return nullptr;
    }
    
    std::function<TreeNode<T>*(int, int, int, int)> build = 
        [&](int post_start, int post_end, int in_start, int in_end) -> TreeNode<T>* {
        if (post_start > post_end) return nullptr;
        
        T root_val = postorder[post_end];
        TreeNode<T>* root = new TreeNode<T>(root_val);
        
        // 在中序遍历中找到根节点位置
        int root_index = in_start;
        for (int i = in_start; i <= in_end; ++i) {
            if (inorder[i] == root_val) {
                root_index = i;
                break;
            }
        }
        
        int left_size = root_index - in_start;
        
        root->left = build(post_start, post_start + left_size - 1, 
                          in_start, root_index - 1);
        root->right = build(post_start + left_size, post_end - 1, 
                           root_index + 1, in_end);
        
        return root;
    };
    
    return build(0, postorder.size() - 1, 0, inorder.size() - 1);
}

#endif // BINARY_TREE_H