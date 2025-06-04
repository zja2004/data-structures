#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include <stdexcept>
#include <functional>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

// 基础堆类
template<typename T, typename Compare = less<T>>
class Heap {
private:
    vector<T> heap;
    Compare comp;
    
    // 获取父节点索引
    int parent(int i) {
        return (i - 1) / 2;
    }
    
    // 获取左子节点索引
    int left_child(int i) {
        return 2 * i + 1;
    }
    
    // 获取右子节点索引
    int right_child(int i) {
        return 2 * i + 2;
    }
    
    // 上浮操作
    void sift_up(int i) {
        while (i > 0 && comp(heap[parent(i)], heap[i])) {
            swap(heap[i], heap[parent(i)]);
            i = parent(i);
        }
    }
    
    // 下沉操作
    void sift_down(int i) {
        int size = heap.size();
        while (left_child(i) < size) {
            int largest = i;
            int left = left_child(i);
            int right = right_child(i);
            
            if (left < size && comp(heap[largest], heap[left])) {
                largest = left;
            }
            
            if (right < size && comp(heap[largest], heap[right])) {
                largest = right;
            }
            
            if (largest == i) {
                break;
            }
            
            swap(heap[i], heap[largest]);
            i = largest;
        }
    }
    
    // 建堆操作
    void build_heap() {
        int size = heap.size();
        for (int i = size / 2 - 1; i >= 0; --i) {
            sift_down(i);
        }
    }
    
public:
    // 构造函数
    explicit Heap(const Compare& cmp = Compare()) : comp(cmp) {}
    
    // 从数组构造
    explicit Heap(const vector<T>& arr, const Compare& cmp = Compare()) 
        : heap(arr), comp(cmp) {
        build_heap();
    }
    
    // 拷贝构造函数
    Heap(const Heap& other) : heap(other.heap), comp(other.comp) {}
    
    // 移动构造函数
    Heap(Heap&& other) noexcept 
        : heap(move(other.heap)), comp(move(other.comp)) {}
    
    // 拷贝赋值运算符
    Heap& operator=(const Heap& other) {
        if (this != &other) {
            heap = other.heap;
            comp = other.comp;
        }
        return *this;
    }
    
    // 移动赋值运算符
    Heap& operator=(Heap&& other) noexcept {
        if (this != &other) {
            heap = move(other.heap);
            comp = move(other.comp);
        }
        return *this;
    }
    
    // 插入元素
    void insert(const T& value) {
        heap.push_back(value);
        sift_up(heap.size() - 1);
    }
    
    void push(const T& value) {
        insert(value);
    }
    
    // 删除并返回堆顶元素
    T extract() {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        
        T result = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        
        if (!empty()) {
            sift_down(0);
        }
        
        return result;
    }
    
    T pop() {
        return extract();
    }
    
    // 查看堆顶元素
    const T& top() const {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        return heap[0];
    }
    
    const T& peek() const {
        return top();
    }
    
    // 检查是否为空
    bool empty() const {
        return heap.empty();
    }
    
    // 获取大小
    size_t size() const {
        return heap.size();
    }
    
    // 清空堆
    void clear() {
        heap.clear();
    }
    
    // 修改指定位置的值
    void change_key(int i, const T& new_value) {
        if (i < 0 || i >= heap.size()) {
            throw out_of_range("Index out of range");
        }
        
        T old_value = heap[i];
        heap[i] = new_value;
        
        if (comp(old_value, new_value)) {
            sift_up(i);
        } else if (comp(new_value, old_value)) {
            sift_down(i);
        }
    }
    
    // 删除指定位置的元素
    void erase(int i) {
        if (i < 0 || i >= heap.size()) {
            throw out_of_range("Index out of range");
        }
        
        heap[i] = heap.back();
        heap.pop_back();
        
        if (i < heap.size()) {
            // 可能需要上浮或下沉
            int parent_idx = parent(i);
            if (i > 0 && comp(heap[parent_idx], heap[i])) {
                sift_up(i);
            } else {
                sift_down(i);
            }
        }
    }
    
    // 获取堆的数组表示
    const vector<T>& get_array() const {
        return heap;
    }
    
    // 验证堆性质
    bool is_valid_heap() const {
        for (int i = 0; i < heap.size(); ++i) {
            int left = left_child(i);
            int right = right_child(i);
            
            if (left < heap.size() && comp(heap[i], heap[left])) {
                return false;
            }
            
            if (right < heap.size() && comp(heap[i], heap[right])) {
                return false;
            }
        }
        return true;
    }
    
    // 打印堆结构
    void print_structure() const {
        if (empty()) {
            cout << "Empty heap" << endl;
            return;
        }
        
        cout << "Heap structure:" << endl;
        print_level(0, 0);
        cout << endl;
    }
    
    // 打印数组表示
    void print_array() const {
        cout << "Heap array: [";
        for (int i = 0; i < heap.size(); ++i) {
            if (i > 0) cout << ", ";
            cout << heap[i];
        }
        cout << "]" << endl;
    }
    
private:
    // 递归打印层级结构
    void print_level(int index, int level) const {
        if (index >= heap.size()) return;
        
        // 打印右子树
        print_level(right_child(index), level + 1);
        
        // 打印当前节点
        for (int i = 0; i < level; ++i) {
            cout << "    ";
        }
        cout << heap[index] << endl;
        
        // 打印左子树
        print_level(left_child(index), level + 1);
    }
};

// 最大堆类型别名
template<typename T>
using MaxHeap = Heap<T, less<T>>;

// 最小堆类型别名
template<typename T>
using MinHeap = Heap<T, greater<T>>;

// 常用类型的堆
using IntMaxHeap = MaxHeap<int>;
using IntMinHeap = MinHeap<int>;
using LongMaxHeap = MaxHeap<long long>;
using LongMinHeap = MinHeap<long long>;
using DoubleMaxHeap = MaxHeap<double>;
using DoubleMinHeap = MinHeap<double>;
using StringMaxHeap = MaxHeap<string>;
using StringMinHeap = MinHeap<string>;

// 堆排序函数
template<typename T, typename Compare = less<T>>
void heap_sort(vector<T>& arr, const Compare& comp = Compare()) {
    if (arr.size() <= 1) return;
    
    // 建堆
    Heap<T, Compare> heap(arr, comp);
    
    // 依次取出最值
    arr.clear();
    while (!heap.empty()) {
        arr.push_back(heap.extract());
    }
    
    // 如果是最大堆，结果是升序；如果是最小堆，结果是降序
    // 对于升序排序，我们需要使用最大堆然后反转
    if (is_same_v<Compare, less<T>>) {
        reverse(arr.begin(), arr.end());
    }
}

// 原地堆排序
template<typename T>
void heap_sort_inplace(vector<T>& arr) {
    if (arr.size() <= 1) return;
    
    // 建最大堆
    auto build_max_heap = [&arr](int size) {
        for (int i = size / 2 - 1; i >= 0; --i) {
            int parent = i;
            while (2 * parent + 1 < size) {
                int left = 2 * parent + 1;
                int right = 2 * parent + 2;
                int largest = parent;
                
                if (left < size && arr[left] > arr[largest]) {
                    largest = left;
                }
                
                if (right < size && arr[right] > arr[largest]) {
                    largest = right;
                }
                
                if (largest == parent) break;
                
                swap(arr[parent], arr[largest]);
                parent = largest;
            }
        }
    };
    
    // 建堆
    build_max_heap(arr.size());
    
    // 排序
    for (int i = arr.size() - 1; i > 0; --i) {
        swap(arr[0], arr[i]);
        build_max_heap(i);
    }
}

// 找第K大元素
template<typename T>
T find_kth_largest(vector<T> arr, int k) {
    if (k <= 0 || k > arr.size()) {
        throw invalid_argument("Invalid k");
    }
    
    MinHeap<T> heap;
    
    for (const T& val : arr) {
        if (heap.size() < k) {
            heap.insert(val);
        } else if (val > heap.top()) {
            heap.pop();
            heap.insert(val);
        }
    }
    
    return heap.top();
}

// 找第K小元素
template<typename T>
T find_kth_smallest(vector<T> arr, int k) {
    if (k <= 0 || k > arr.size()) {
        throw invalid_argument("Invalid k");
    }
    
    MaxHeap<T> heap;
    
    for (const T& val : arr) {
        if (heap.size() < k) {
            heap.insert(val);
        } else if (val < heap.top()) {
            heap.pop();
            heap.insert(val);
        }
    }
    
    return heap.top();
}

// 合并K个有序数组
template<typename T>
vector<T> merge_k_sorted_arrays(const vector<vector<T>>& arrays) {
    struct Element {
        T value;
        int array_idx;
        int element_idx;
        
        bool operator>(const Element& other) const {
            return value > other.value;
        }
    };
    
    MinHeap<Element> heap;
    vector<T> result;
    
    // 将每个数组的第一个元素加入堆
    for (int i = 0; i < arrays.size(); ++i) {
        if (!arrays[i].empty()) {
            heap.insert({arrays[i][0], i, 0});
        }
    }
    
    // 依次取出最小元素
    while (!heap.empty()) {
        Element min_elem = heap.extract();
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

// 中位数维护器
template<typename T>
class MedianFinder {
private:
    MaxHeap<T> left_heap;   // 存储较小的一半
    MinHeap<T> right_heap;  // 存储较大的一半
    
public:
    // 添加数字
    void add_number(T num) {
        if (left_heap.empty() || num <= left_heap.top()) {
            left_heap.insert(num);
        } else {
            right_heap.insert(num);
        }
        
        // 平衡两个堆的大小
        if (left_heap.size() > right_heap.size() + 1) {
            right_heap.insert(left_heap.extract());
        } else if (right_heap.size() > left_heap.size() + 1) {
            left_heap.insert(right_heap.extract());
        }
    }
    
    // 获取中位数
    double get_median() {
        if (left_heap.empty() && right_heap.empty()) {
            throw runtime_error("No numbers added");
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
    
    // 获取当前数字数量
    size_t size() const {
        return left_heap.size() + right_heap.size();
    }
    
    // 检查是否为空
    bool empty() const {
        return left_heap.empty() && right_heap.empty();
    }
};

// 堆性能统计
struct HeapPerformance {
    int insert_count = 0;
    int extract_count = 0;
    double total_insert_time = 0.0;
    double total_extract_time = 0.0;
    
    double avg_insert_time() const {
        return insert_count > 0 ? total_insert_time / insert_count : 0.0;
    }
    
    double avg_extract_time() const {
        return extract_count > 0 ? total_extract_time / extract_count : 0.0;
    }
    
    void print_stats() const {
        cout << "Heap Performance Statistics:" << endl;
        cout << "Insert operations: " << insert_count << endl;
        cout << "Extract operations: " << extract_count << endl;
        cout << "Average insert time: " << fixed << setprecision(3) 
             << avg_insert_time() << "ms" << endl;
        cout << "Average extract time: " << fixed << setprecision(3) 
             << avg_extract_time() << "ms" << endl;
        cout << "Total time: " << fixed << setprecision(3) 
             << (total_insert_time + total_extract_time) << "ms" << endl;
    }
};

// 实用函数

// 构建最大堆
template<typename T>
MaxHeap<T> build_max_heap(const vector<T>& arr) {
    return MaxHeap<T>(arr);
}

// 构建最小堆
template<typename T>
MinHeap<T> build_min_heap(const vector<T>& arr) {
    return MinHeap<T>(arr);
}

// 检查数组是否满足堆性质
template<typename T, typename Compare = less<T>>
bool is_heap(const vector<T>& arr, const Compare& comp = Compare()) {
    for (int i = 0; i < arr.size(); ++i) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        
        if (left < arr.size() && comp(arr[i], arr[left])) {
            return false;
        }
        
        if (right < arr.size() && comp(arr[i], arr[right])) {
            return false;
        }
    }
    return true;
}

// 将数组转换为堆
template<typename T, typename Compare = less<T>>
void make_heap(vector<T>& arr, const Compare& comp = Compare()) {
    Heap<T, Compare> heap(arr, comp);
    arr = heap.get_array();
}

#endif // HEAP_H