#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H

#include <vector>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <algorithm>

// 二叉堆基类
template<typename T, typename Compare = std::less<T>>
class BinaryHeap {
protected:
    std::vector<T> heap;
    Compare comp;
    
    // 获取父节点索引
    size_t parent(size_t i) const {
        return (i - 1) / 2;
    }
    
    // 获取左子节点索引
    size_t left_child(size_t i) const {
        return 2 * i + 1;
    }
    
    // 获取右子节点索引
    size_t right_child(size_t i) const {
        return 2 * i + 2;
    }
    
    // 向上调整（用于插入）
    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent_idx = parent(index);
            if (!comp(heap[parent_idx], heap[index])) {
                break;
            }
            std::swap(heap[index], heap[parent_idx]);
            index = parent_idx;
        }
    }
    
    // 向下调整（用于删除）
    void heapify_down(size_t index) {
        size_t size = heap.size();
        
        while (true) {
            size_t largest = index;
            size_t left = left_child(index);
            size_t right = right_child(index);
            
            // 找到最大的节点
            if (left < size && comp(heap[largest], heap[left])) {
                largest = left;
            }
            
            if (right < size && comp(heap[largest], heap[right])) {
                largest = right;
            }
            
            // 如果当前节点已经是最大的，停止
            if (largest == index) {
                break;
            }
            
            std::swap(heap[index], heap[largest]);
            index = largest;
        }
    }
    
public:
    // 构造函数
    BinaryHeap() = default;
    
    explicit BinaryHeap(const Compare& compare) : comp(compare) {}
    
    // 从数组构造堆
    explicit BinaryHeap(const std::vector<T>& data, const Compare& compare = Compare()) 
        : heap(data), comp(compare) {
        build_heap();
    }
    
    // 拷贝构造函数
    BinaryHeap(const BinaryHeap& other) : heap(other.heap), comp(other.comp) {}
    
    // 赋值操作符
    BinaryHeap& operator=(const BinaryHeap& other) {
        if (this != &other) {
            heap = other.heap;
            comp = other.comp;
        }
        return *this;
    }
    
    // 移动构造函数
    BinaryHeap(BinaryHeap&& other) noexcept 
        : heap(std::move(other.heap)), comp(std::move(other.comp)) {}
    
    // 移动赋值操作符
    BinaryHeap& operator=(BinaryHeap&& other) noexcept {
        if (this != &other) {
            heap = std::move(other.heap);
            comp = std::move(other.comp);
        }
        return *this;
    }
    
    // 基本操作
    void insert(const T& value) {
        heap.push_back(value);
        heapify_up(heap.size() - 1);
    }
    
    void insert(T&& value) {
        heap.push_back(std::move(value));
        heapify_up(heap.size() - 1);
    }
    
    T extract_top() {
        if (empty()) {
            throw std::runtime_error("Heap is empty");
        }
        
        T top_value = std::move(heap[0]);
        heap[0] = std::move(heap.back());
        heap.pop_back();
        
        if (!empty()) {
            heapify_down(0);
        }
        
        return top_value;
    }
    
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Heap is empty");
        }
        return heap[0];
    }
    
    // 状态查询
    bool empty() const {
        return heap.empty();
    }
    
    size_t size() const {
        return heap.size();
    }
    
    void clear() {
        heap.clear();
    }
    
    // 容量管理
    void reserve(size_t capacity) {
        heap.reserve(capacity);
    }
    
    void shrink_to_fit() {
        heap.shrink_to_fit();
    }
    
    // 堆操作
    void build_heap() {
        if (heap.size() <= 1) return;
        
        // 从最后一个非叶子节点开始向下调整
        for (int i = static_cast<int>(parent(heap.size() - 1)); i >= 0; --i) {
            heapify_down(static_cast<size_t>(i));
        }
    }
    
    // 修改指定位置的值
    void update(size_t index, const T& new_value) {
        if (index >= heap.size()) {
            throw std::out_of_range("Index out of range");
        }
        
        T old_value = heap[index];
        heap[index] = new_value;
        
        // 根据新值与旧值的比较决定调整方向
        if (comp(old_value, new_value)) {
            heapify_up(index);
        } else {
            heapify_down(index);
        }
    }
    
    // 删除指定位置的元素
    void erase(size_t index) {
        if (index >= heap.size()) {
            throw std::out_of_range("Index out of range");
        }
        
        if (index == heap.size() - 1) {
            heap.pop_back();
            return;
        }
        
        T last_value = std::move(heap.back());
        heap.pop_back();
        
        T old_value = heap[index];
        heap[index] = std::move(last_value);
        
        // 根据新值与旧值的比较决定调整方向
        if (comp(old_value, heap[index])) {
            heapify_up(index);
        } else {
            heapify_down(index);
        }
    }
    
    // 查找元素（线性搜索）
    size_t find(const T& value) const {
        for (size_t i = 0; i < heap.size(); ++i) {
            if (heap[i] == value) {
                return i;
            }
        }
        return heap.size(); // 未找到
    }
    
    bool contains(const T& value) const {
        return find(value) != heap.size();
    }
    
    // 验证堆性质
    bool is_valid_heap() const {
        for (size_t i = 0; i < heap.size(); ++i) {
            size_t left = left_child(i);
            size_t right = right_child(i);
            
            if (left < heap.size() && comp(heap[i], heap[left])) {
                return false;
            }
            
            if (right < heap.size() && comp(heap[i], heap[right])) {
                return false;
            }
        }
        return true;
    }
    
    // 获取堆的数组表示（用于调试）
    const std::vector<T>& get_array() const {
        return heap;
    }
    
    // 打印堆结构
    void print_heap() const {
        if (empty()) {
            std::cout << "(empty heap)" << std::endl;
            return;
        }
        
        std::cout << "Heap array: [";
        for (size_t i = 0; i < heap.size(); ++i) {
            std::cout << heap[i];
            if (i < heap.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }
    
    // 打印树形结构
    void print_tree() const {
        if (empty()) {
            std::cout << "(empty heap)" << std::endl;
            return;
        }
        print_tree_recursive(0, 0);
    }
    
private:
    void print_tree_recursive(size_t index, int depth) const {
        if (index >= heap.size()) return;
        
        // 先打印右子树
        print_tree_recursive(right_child(index), depth + 1);
        
        // 打印当前节点
        for (int i = 0; i < depth; ++i) {
            std::cout << "    ";
        }
        std::cout << heap[index] << std::endl;
        
        // 打印左子树
        print_tree_recursive(left_child(index), depth + 1);
    }
};

// 最大堆（默认）
template<typename T>
using MaxHeap = BinaryHeap<T, std::less<T>>;

// 最小堆
template<typename T>
using MinHeap = BinaryHeap<T, std::greater<T>>;

// 堆排序函数
template<typename T>
void heap_sort(std::vector<T>& arr, bool ascending = true) {
    if (arr.size() <= 1) return;
    
    if (ascending) {
        // 升序：使用最大堆
        MaxHeap<T> heap(arr);
        for (int i = static_cast<int>(arr.size()) - 1; i >= 0; --i) {
            arr[i] = heap.extract_top();
        }
    } else {
        // 降序：使用最小堆
        MinHeap<T> heap(arr);
        for (int i = static_cast<int>(arr.size()) - 1; i >= 0; --i) {
            arr[i] = heap.extract_top();
        }
    }
}

// 原地堆排序（更高效）
template<typename T>
void heap_sort_inplace(std::vector<T>& arr, bool ascending = true) {
    if (arr.size() <= 1) return;
    
    auto heapify_down = [&](size_t start, size_t end, bool max_heap) {
        size_t parent = start;
        while (true) {
            size_t left = 2 * parent + 1;
            size_t right = 2 * parent + 2;
            size_t target = parent;
            
            if (left <= end) {
                if (max_heap ? arr[left] > arr[target] : arr[left] < arr[target]) {
                    target = left;
                }
            }
            
            if (right <= end) {
                if (max_heap ? arr[right] > arr[target] : arr[right] < arr[target]) {
                    target = right;
                }
            }
            
            if (target == parent) break;
            
            std::swap(arr[parent], arr[target]);
            parent = target;
        }
    };
    
    bool use_max_heap = ascending;
    
    // 构建堆
    for (int i = static_cast<int>(arr.size() / 2) - 1; i >= 0; --i) {
        heapify_down(static_cast<size_t>(i), arr.size() - 1, use_max_heap);
    }
    
    // 排序
    for (size_t i = arr.size() - 1; i > 0; --i) {
        std::swap(arr[0], arr[i]);
        heapify_down(0, i - 1, use_max_heap);
    }
}

// 找第K大元素
template<typename T>
T find_kth_largest(std::vector<T> arr, int k) {
    if (k <= 0 || k > static_cast<int>(arr.size())) {
        throw std::out_of_range("k is out of range");
    }
    
    MinHeap<T> heap;
    
    for (const T& val : arr) {
        if (heap.size() < static_cast<size_t>(k)) {
            heap.insert(val);
        } else if (val > heap.top()) {
            heap.extract_top();
            heap.insert(val);
        }
    }
    
    return heap.top();
}

// 找第K小元素
template<typename T>
T find_kth_smallest(std::vector<T> arr, int k) {
    if (k <= 0 || k > static_cast<int>(arr.size())) {
        throw std::out_of_range("k is out of range");
    }
    
    MaxHeap<T> heap;
    
    for (const T& val : arr) {
        if (heap.size() < static_cast<size_t>(k)) {
            heap.insert(val);
        } else if (val < heap.top()) {
            heap.extract_top();
            heap.insert(val);
        }
    }
    
    return heap.top();
}

// 合并K个有序数组
template<typename T>
std::vector<T> merge_k_sorted_arrays(const std::vector<std::vector<T>>& arrays) {
    struct Element {
        T value;
        size_t array_idx;
        size_t element_idx;
        
        bool operator>(const Element& other) const {
            return value > other.value;
        }
    };
    
    MinHeap<Element> heap;
    std::vector<T> result;
    
    // 将每个数组的第一个元素加入堆
    for (size_t i = 0; i < arrays.size(); ++i) {
        if (!arrays[i].empty()) {
            heap.insert({arrays[i][0], i, 0});
        }
    }
    
    while (!heap.empty()) {
        Element min_elem = heap.extract_top();
        result.push_back(min_elem.value);
        
        // 如果该数组还有下一个元素，加入堆
        if (min_elem.element_idx + 1 < arrays[min_elem.array_idx].size()) {
            heap.insert({
                arrays[min_elem.array_idx][min_elem.element_idx + 1],
                min_elem.array_idx,
                min_elem.element_idx + 1
            });
        }
    }
    
    return result;
}

// 数据流中位数类
template<typename T>
class MedianFinder {
private:
    MaxHeap<T> left_heap;  // 存储较小的一半
    MinHeap<T> right_heap; // 存储较大的一半
    
public:
    void add_number(const T& num) {
        if (left_heap.empty() || num <= left_heap.top()) {
            left_heap.insert(num);
        } else {
            right_heap.insert(num);
        }
        
        // 平衡两个堆的大小
        if (left_heap.size() > right_heap.size() + 1) {
            right_heap.insert(left_heap.extract_top());
        } else if (right_heap.size() > left_heap.size() + 1) {
            left_heap.insert(right_heap.extract_top());
        }
    }
    
    double find_median() const {
        if (left_heap.empty() && right_heap.empty()) {
            throw std::runtime_error("No numbers added");
        }
        
        if (left_heap.size() == right_heap.size()) {
            return (static_cast<double>(left_heap.top()) + 
                   static_cast<double>(right_heap.top())) / 2.0;
        } else if (left_heap.size() > right_heap.size()) {
            return static_cast<double>(left_heap.top());
        } else {
            return static_cast<double>(right_heap.top());
        }
    }
    
    size_t size() const {
        return left_heap.size() + right_heap.size();
    }
};

// 滑动窗口最大值
template<typename T>
std::vector<T> sliding_window_maximum(const std::vector<T>& nums, int k) {
    if (nums.empty() || k <= 0) return {};
    
    std::vector<T> result;
    MaxHeap<std::pair<T, int>> heap; // (value, index)
    
    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        heap.insert({nums[i], i});
        
        // 移除窗口外的元素
        while (!heap.empty() && heap.top().second <= i - k) {
            heap.extract_top();
        }
        
        // 当窗口大小达到k时，记录最大值
        if (i >= k - 1) {
            result.push_back(heap.top().first);
        }
    }
    
    return result;
}

#endif // BINARY_HEAP_H