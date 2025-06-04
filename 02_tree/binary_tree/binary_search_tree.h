#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include "binary_tree.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>

// 二叉搜索树类
template<typename T>
class BinarySearchTree : public BinaryTree<T> {
public:
    // 构造函数
    BinarySearchTree() : BinaryTree<T>() {}
    
    // 从有序数组构造平衡BST
    BinarySearchTree(const std::vector<T>& sorted_array) : BinaryTree<T>() {
        this->root = build_balanced_bst(sorted_array, 0, sorted_array.size() - 1);
        this->node_count = sorted_array.size();
    }
    
    // 插入操作
    void insert(const T& value) {
        this->root = insert_recursive(this->root, value);
    }
    
    // 删除操作
    void remove(const T& value) {
        this->root = remove_recursive(this->root, value);
    }
    
    // 查找操作
    TreeNode<T>* search(const T& value) const {
        return search_recursive(this->root, value);
    }
    
    bool contains(const T& value) const {
        return search(value) != nullptr;
    }
    
    // 查找最小值
    TreeNode<T>* find_min() const {
        return find_min_node(this->root);
    }
    
    T min_value() const {
        TreeNode<T>* min_node = find_min();
        if (!min_node) {
            throw std::runtime_error("Tree is empty");
        }
        return min_node->data;
    }
    
    // 查找最大值
    TreeNode<T>* find_max() const {
        return find_max_node(this->root);
    }
    
    T max_value() const {
        TreeNode<T>* max_node = find_max();
        if (!max_node) {
            throw std::runtime_error("Tree is empty");
        }
        return max_node->data;
    }
    
    // 查找前驱
    TreeNode<T>* predecessor(const T& value) const {
        TreeNode<T>* node = search(value);
        if (!node) return nullptr;
        return find_predecessor(node);
    }
    
    // 查找后继
    TreeNode<T>* successor(const T& value) const {
        TreeNode<T>* node = search(value);
        if (!node) return nullptr;
        return find_successor(node);
    }
    
    // 范围查询
    std::vector<T> range_query(const T& min_val, const T& max_val) const {
        std::vector<T> result;
        range_search(this->root, min_val, max_val, result);
        return result;
    }
    
    // 查找第k小的元素（1-indexed）
    T kth_smallest(int k) const {
        int count = 0;
        TreeNode<T>* result = find_kth_smallest(this->root, k, count);
        if (!result) {
            throw std::out_of_range("k is out of range");
        }
        return result->data;
    }
    
    // 查找第k大的元素（1-indexed）
    T kth_largest(int k) const {
        return kth_smallest(this->size() - k + 1);
    }
    
    // 验证BST性质
    bool is_valid_bst() const {
        return validate_bst(this->root, std::numeric_limits<T>::lowest(), 
                           std::numeric_limits<T>::max());
    }
    
    // 获取有序序列
    std::vector<T> get_sorted_sequence() const {
        return this->inorder();
    }
    
    // 树的平衡操作
    void balance() {
        std::vector<T> sorted_values = get_sorted_sequence();
        this->clear();
        this->root = build_balanced_bst(sorted_values, 0, sorted_values.size() - 1);
        this->node_count = sorted_values.size();
    }
    
    // 统计信息
    int count_nodes_in_range(const T& min_val, const T& max_val) const {
        return count_in_range(this->root, min_val, max_val);
    }
    
    // 树的修改操作
    void insert_batch(const std::vector<T>& values) {
        for (const T& value : values) {
            insert(value);
        }
    }
    
    void remove_batch(const std::vector<T>& values) {
        for (const T& value : values) {
            remove(value);
        }
    }
    
    // 输出函数
    void print_sorted() const {
        std::cout << "BST sorted sequence: [";
        auto sorted = get_sorted_sequence();
        for (size_t i = 0; i < sorted.size(); ++i) {
            std::cout << sorted[i];
            if (i < sorted.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    void print_structure() const {
        std::cout << "BST structure (size: " << this->size() 
                  << ", height: " << this->height() 
                  << ", balanced: " << (this->is_balanced() ? "yes" : "no")
                  << ", valid: " << (is_valid_bst() ? "yes" : "no") << "):" << std::endl;
        if (this->empty()) {
            std::cout << "(empty)" << std::endl;
        } else {
            this->print_tree(this->root, 0);
        }
    }
    
private:
    // 递归插入
    TreeNode<T>* insert_recursive(TreeNode<T>* node, const T& value) {
        if (!node) {
            ++this->node_count;
            return new TreeNode<T>(value);
        }
        
        if (value < node->data) {
            node->left = insert_recursive(node->left, value);
        } else if (value > node->data) {
            node->right = insert_recursive(node->right, value);
        }
        // 如果值相等，不插入重复值
        
        return node;
    }
    
    // 递归删除
    TreeNode<T>* remove_recursive(TreeNode<T>* node, const T& value) {
        if (!node) return nullptr;
        
        if (value < node->data) {
            node->left = remove_recursive(node->left, value);
        } else if (value > node->data) {
            node->right = remove_recursive(node->right, value);
        } else {
            // 找到要删除的节点
            --this->node_count;
            
            // 情况1：叶子节点
            if (!node->left && !node->right) {
                delete node;
                return nullptr;
            }
            // 情况2：只有一个子节点
            else if (!node->left) {
                TreeNode<T>* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                TreeNode<T>* temp = node->left;
                delete node;
                return temp;
            }
            // 情况3：有两个子节点
            else {
                // 找到右子树的最小节点（中序后继）
                TreeNode<T>* successor = find_min_node(node->right);
                
                // 用后继节点的值替换当前节点的值
                node->data = successor->data;
                
                // 删除后继节点
                ++this->node_count; // 先加回来，因为下面的递归会减
                node->right = remove_recursive(node->right, successor->data);
            }
        }
        
        return node;
    }
    
    // 递归查找
    TreeNode<T>* search_recursive(TreeNode<T>* node, const T& value) const {
        if (!node || node->data == value) {
            return node;
        }
        
        if (value < node->data) {
            return search_recursive(node->left, value);
        } else {
            return search_recursive(node->right, value);
        }
    }
    
    // 查找最小节点
    TreeNode<T>* find_min_node(TreeNode<T>* node) const {
        if (!node) return nullptr;
        
        while (node->left) {
            node = node->left;
        }
        return node;
    }
    
    // 查找最大节点
    TreeNode<T>* find_max_node(TreeNode<T>* node) const {
        if (!node) return nullptr;
        
        while (node->right) {
            node = node->right;
        }
        return node;
    }
    
    // 查找前驱
    TreeNode<T>* find_predecessor(TreeNode<T>* node) const {
        if (!node) return nullptr;
        
        // 如果有左子树，前驱是左子树的最大值
        if (node->left) {
            return find_max_node(node->left);
        }
        
        // 否则需要向上查找
        // 注意：这里需要父指针，简化实现中暂不支持
        return nullptr;
    }
    
    // 查找后继
    TreeNode<T>* find_successor(TreeNode<T>* node) const {
        if (!node) return nullptr;
        
        // 如果有右子树，后继是右子树的最小值
        if (node->right) {
            return find_min_node(node->right);
        }
        
        // 否则需要向上查找
        // 注意：这里需要父指针，简化实现中暂不支持
        return nullptr;
    }
    
    // 范围搜索
    void range_search(TreeNode<T>* node, const T& min_val, const T& max_val, 
                     std::vector<T>& result) const {
        if (!node) return;
        
        // 如果当前节点值在范围内
        if (node->data >= min_val && node->data <= max_val) {
            result.push_back(node->data);
        }
        
        // 递归搜索左子树
        if (node->data > min_val) {
            range_search(node->left, min_val, max_val, result);
        }
        
        // 递归搜索右子树
        if (node->data < max_val) {
            range_search(node->right, min_val, max_val, result);
        }
    }
    
    // 查找第k小元素
    TreeNode<T>* find_kth_smallest(TreeNode<T>* node, int k, int& count) const {
        if (!node) return nullptr;
        
        // 先搜索左子树
        TreeNode<T>* left_result = find_kth_smallest(node->left, k, count);
        if (left_result) return left_result;
        
        // 访问当前节点
        ++count;
        if (count == k) return node;
        
        // 搜索右子树
        return find_kth_smallest(node->right, k, count);
    }
    
    // 验证BST性质
    bool validate_bst(TreeNode<T>* node, T min_val, T max_val) const {
        if (!node) return true;
        
        if (node->data <= min_val || node->data >= max_val) {
            return false;
        }
        
        return validate_bst(node->left, min_val, node->data) &&
               validate_bst(node->right, node->data, max_val);
    }
    
    // 从有序数组构建平衡BST
    TreeNode<T>* build_balanced_bst(const std::vector<T>& arr, int start, int end) {
        if (start > end) return nullptr;
        
        int mid = start + (end - start) / 2;
        TreeNode<T>* node = new TreeNode<T>(arr[mid]);
        
        node->left = build_balanced_bst(arr, start, mid - 1);
        node->right = build_balanced_bst(arr, mid + 1, end);
        
        return node;
    }
    
    // 统计范围内节点数
    int count_in_range(TreeNode<T>* node, const T& min_val, const T& max_val) const {
        if (!node) return 0;
        
        int count = 0;
        
        // 如果当前节点在范围内
        if (node->data >= min_val && node->data <= max_val) {
            count = 1;
        }
        
        // 递归计算左右子树
        if (node->data > min_val) {
            count += count_in_range(node->left, min_val, max_val);
        }
        
        if (node->data < max_val) {
            count += count_in_range(node->right, min_val, max_val);
        }
        
        return count;
    }
};

// BST迭代器类
template<typename T>
class BSTIterator {
private:
    std::stack<TreeNode<T>*> stack;
    
    void push_left(TreeNode<T>* node) {
        while (node) {
            stack.push(node);
            node = node->left;
        }
    }
    
public:
    BSTIterator(TreeNode<T>* root) {
        push_left(root);
    }
    
    bool has_next() const {
        return !stack.empty();
    }
    
    T next() {
        if (!has_next()) {
            throw std::runtime_error("No more elements");
        }
        
        TreeNode<T>* node = stack.top();
        stack.pop();
        
        push_left(node->right);
        
        return node->data;
    }
};

// 一些实用函数

// 判断两个BST是否相同
template<typename T>
bool are_identical_bst(const BinarySearchTree<T>& bst1, const BinarySearchTree<T>& bst2) {
    std::function<bool(TreeNode<T>*, TreeNode<T>*)> compare = 
        [&](TreeNode<T>* node1, TreeNode<T>* node2) -> bool {
        if (!node1 && !node2) return true;
        if (!node1 || !node2) return false;
        
        return node1->data == node2->data &&
               compare(node1->left, node2->left) &&
               compare(node1->right, node2->right);
    };
    
    return compare(bst1.get_root(), bst2.get_root());
}

// 合并两个BST
template<typename T>
BinarySearchTree<T> merge_bst(const BinarySearchTree<T>& bst1, const BinarySearchTree<T>& bst2) {
    std::vector<T> seq1 = bst1.get_sorted_sequence();
    std::vector<T> seq2 = bst2.get_sorted_sequence();
    
    std::vector<T> merged;
    std::merge(seq1.begin(), seq1.end(), seq2.begin(), seq2.end(), 
               std::back_inserter(merged));
    
    // 去重
    merged.erase(std::unique(merged.begin(), merged.end()), merged.end());
    
    return BinarySearchTree<T>(merged);
}

// 从BST中提取子树
template<typename T>
BinarySearchTree<T> extract_subtree(const BinarySearchTree<T>& bst, const T& min_val, const T& max_val) {
    std::vector<T> range_values = bst.range_query(min_val, max_val);
    BinarySearchTree<T> subtree;
    subtree.insert_batch(range_values);
    return subtree;
}

#endif // BINARY_SEARCH_TREE_H