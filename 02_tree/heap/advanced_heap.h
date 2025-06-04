#ifndef ADVANCED_HEAP_H
#define ADVANCED_HEAP_H

#include "heap.h"
#include <memory>
#include <map>
#include <unordered_map>
#include <set>
#include <stack>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;

// d叉堆
template<typename T, int D = 4, typename Compare = less<T>>
class DaryHeap {
private:
    vector<T> heap;
    Compare comp;
    
    // 获取父节点索引
    int parent(int i) {
        return (i - 1) / D;
    }
    
    // 获取第k个子节点索引
    int child(int i, int k) {
        return D * i + k + 1;
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
        while (child(i, 0) < size) {
            int best = i;
            
            // 找到所有子节点中的最优节点
            for (int k = 0; k < D; ++k) {
                int c = child(i, k);
                if (c < size && comp(heap[best], heap[c])) {
                    best = c;
                }
            }
            
            if (best == i) break;
            
            swap(heap[i], heap[best]);
            i = best;
        }
    }
    
    // 建堆操作
    void build_heap() {
        int size = heap.size();
        for (int i = size / D; i >= 0; --i) {
            sift_down(i);
        }
    }
    
public:
    // 构造函数
    explicit DaryHeap(const Compare& cmp = Compare()) : comp(cmp) {}
    
    explicit DaryHeap(const vector<T>& arr, const Compare& cmp = Compare()) 
        : heap(arr), comp(cmp) {
        build_heap();
    }
    
    // 插入元素
    void insert(const T& value) {
        heap.push_back(value);
        sift_up(heap.size() - 1);
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
    
    // 查看堆顶元素
    const T& top() const {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        return heap[0];
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
    
    // 获取度数
    static constexpr int degree() {
        return D;
    }
};

// 索引堆
template<typename T, typename Compare = less<T>>
class IndexedHeap {
private:
    vector<T> values;           // 存储值
    vector<int> heap;           // 堆数组，存储索引
    vector<int> index_map;      // 索引映射，index_map[i]表示索引i在堆中的位置
    Compare comp;
    int next_id;
    
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
    
    // 比较两个索引对应的值
    bool compare(int i, int j) {
        return comp(values[i], values[j]);
    }
    
    // 交换堆中两个位置的元素
    void swap_heap(int i, int j) {
        int idx1 = heap[i];
        int idx2 = heap[j];
        
        swap(heap[i], heap[j]);
        index_map[idx1] = j;
        index_map[idx2] = i;
    }
    
    // 上浮操作
    void sift_up(int pos) {
        while (pos > 0 && compare(heap[parent(pos)], heap[pos])) {
            swap_heap(pos, parent(pos));
            pos = parent(pos);
        }
    }
    
    // 下沉操作
    void sift_down(int pos) {
        int size = heap.size();
        while (left_child(pos) < size) {
            int best = pos;
            int left = left_child(pos);
            int right = right_child(pos);
            
            if (left < size && compare(heap[best], heap[left])) {
                best = left;
            }
            
            if (right < size && compare(heap[best], heap[right])) {
                best = right;
            }
            
            if (best == pos) break;
            
            swap_heap(pos, best);
            pos = best;
        }
    }
    
public:
    // 构造函数
    explicit IndexedHeap(const Compare& cmp = Compare()) 
        : comp(cmp), next_id(0) {}
    
    // 插入元素，返回索引
    int insert(const T& value) {
        int id = next_id++;
        
        if (id >= values.size()) {
            values.resize(id + 1);
            index_map.resize(id + 1, -1);
        }
        
        values[id] = value;
        heap.push_back(id);
        index_map[id] = heap.size() - 1;
        
        sift_up(heap.size() - 1);
        
        return id;
    }
    
    // 删除并返回堆顶元素
    pair<int, T> extract() {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        
        int top_id = heap[0];
        T top_value = values[top_id];
        
        // 将最后一个元素移到堆顶
        heap[0] = heap.back();
        index_map[heap[0]] = 0;
        heap.pop_back();
        
        // 标记删除的元素
        index_map[top_id] = -1;
        
        if (!empty()) {
            sift_down(0);
        }
        
        return {top_id, top_value};
    }
    
    // 查看堆顶元素
    pair<int, T> top() const {
        if (empty()) {
            throw runtime_error("Heap is empty");
        }
        
        int top_id = heap[0];
        return {top_id, values[top_id]};
    }
    
    // 修改指定索引的值
    void change_key(int id, const T& new_value) {
        if (id < 0 || id >= values.size() || index_map[id] == -1) {
            throw invalid_argument("Invalid index");
        }
        
        T old_value = values[id];
        values[id] = new_value;
        
        int pos = index_map[id];
        
        if (comp(old_value, new_value)) {
            sift_up(pos);
        } else if (comp(new_value, old_value)) {
            sift_down(pos);
        }
    }
    
    // 增加键值
    void increase_key(int id, const T& delta) {
        if (id < 0 || id >= values.size() || index_map[id] == -1) {
            throw invalid_argument("Invalid index");
        }
        
        change_key(id, values[id] + delta);
    }
    
    // 减少键值
    void decrease_key(int id, const T& delta) {
        if (id < 0 || id >= values.size() || index_map[id] == -1) {
            throw invalid_argument("Invalid index");
        }
        
        change_key(id, values[id] - delta);
    }
    
    // 删除指定索引的元素
    void erase(int id) {
        if (id < 0 || id >= values.size() || index_map[id] == -1) {
            throw invalid_argument("Invalid index");
        }
        
        int pos = index_map[id];
        
        // 将要删除的元素移到最后
        swap_heap(pos, heap.size() - 1);
        heap.pop_back();
        index_map[id] = -1;
        
        // 重新调整堆
        if (pos < heap.size()) {
            int parent_pos = parent(pos);
            if (pos > 0 && compare(heap[parent_pos], heap[pos])) {
                sift_up(pos);
            } else {
                sift_down(pos);
            }
        }
    }
    
    // 检查索引是否存在
    bool contains(int id) const {
        return id >= 0 && id < values.size() && index_map[id] != -1;
    }
    
    // 获取指定索引的值
    T get_value(int id) const {
        if (!contains(id)) {
            throw invalid_argument("Invalid index");
        }
        return values[id];
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
        values.clear();
        index_map.clear();
        next_id = 0;
    }
};

// 双端堆（同时维护最大值和最小值）
template<typename T>
class DoubleEndedHeap {
private:
    MaxHeap<T> max_heap;
    MinHeap<T> min_heap;
    unordered_map<T, int> count;  // 元素计数
    
    // 清理无效元素
    void clean_max_heap() {
        while (!max_heap.empty() && count[max_heap.top()] == 0) {
            max_heap.pop();
        }
    }
    
    void clean_min_heap() {
        while (!min_heap.empty() && count[min_heap.top()] == 0) {
            min_heap.pop();
        }
    }
    
public:
    // 插入元素
    void insert(const T& value) {
        max_heap.insert(value);
        min_heap.insert(value);
        count[value]++;
    }
    
    // 删除并返回最大值
    T extract_max() {
        clean_max_heap();
        if (max_heap.empty()) {
            throw runtime_error("Heap is empty");
        }
        
        T max_val = max_heap.extract();
        count[max_val]--;
        
        if (count[max_val] == 0) {
            count.erase(max_val);
        }
        
        return max_val;
    }
    
    // 删除并返回最小值
    T extract_min() {
        clean_min_heap();
        if (min_heap.empty()) {
            throw runtime_error("Heap is empty");
        }
        
        T min_val = min_heap.extract();
        count[min_val]--;
        
        if (count[min_val] == 0) {
            count.erase(min_val);
        }
        
        return min_val;
    }
    
    // 查看最大值
    T max() {
        clean_max_heap();
        if (max_heap.empty()) {
            throw runtime_error("Heap is empty");
        }
        return max_heap.top();
    }
    
    // 查看最小值
    T min() {
        clean_min_heap();
        if (min_heap.empty()) {
            throw runtime_error("Heap is empty");
        }
        return min_heap.top();
    }
    
    // 删除指定值
    bool erase(const T& value) {
        if (count.find(value) == count.end() || count[value] == 0) {
            return false;
        }
        
        count[value]--;
        if (count[value] == 0) {
            count.erase(value);
        }
        
        return true;
    }
    
    // 检查是否为空
    bool empty() {
        clean_max_heap();
        clean_min_heap();
        return max_heap.empty() && min_heap.empty();
    }
    
    // 获取大小
    size_t size() const {
        size_t total = 0;
        for (const auto& pair : count) {
            total += pair.second;
        }
        return total;
    }
    
    // 清空堆
    void clear() {
        max_heap.clear();
        min_heap.clear();
        count.clear();
    }
};

// 线程安全的堆
template<typename T, typename Compare = less<T>>
class ThreadSafeHeap {
private:
    Heap<T, Compare> heap;
    mutable mutex heap_mutex;
    
public:
    // 构造函数
    explicit ThreadSafeHeap(const Compare& cmp = Compare()) : heap(cmp) {}
    
    explicit ThreadSafeHeap(const vector<T>& arr, const Compare& cmp = Compare()) 
        : heap(arr, cmp) {}
    
    // 线程安全的插入
    void insert(const T& value) {
        lock_guard<mutex> lock(heap_mutex);
        heap.insert(value);
    }
    
    // 线程安全的删除
    T extract() {
        lock_guard<mutex> lock(heap_mutex);
        return heap.extract();
    }
    
    // 线程安全的查看堆顶
    T top() const {
        lock_guard<mutex> lock(heap_mutex);
        return heap.top();
    }
    
    // 线程安全的检查是否为空
    bool empty() const {
        lock_guard<mutex> lock(heap_mutex);
        return heap.empty();
    }
    
    // 线程安全的获取大小
    size_t size() const {
        lock_guard<mutex> lock(heap_mutex);
        return heap.size();
    }
    
    // 批量操作（减少锁的开销）
    void batch_insert(const vector<T>& values) {
        lock_guard<mutex> lock(heap_mutex);
        for (const T& value : values) {
            heap.insert(value);
        }
    }
    
    vector<T> batch_extract(int count) {
        lock_guard<mutex> lock(heap_mutex);
        vector<T> result;
        result.reserve(count);
        
        for (int i = 0; i < count && !heap.empty(); ++i) {
            result.push_back(heap.extract());
        }
        
        return result;
    }
    
    // 清空堆
    void clear() {
        lock_guard<mutex> lock(heap_mutex);
        heap.clear();
    }
};

// 可撤销的堆
template<typename T, typename Compare = less<T>>
class UndoableHeap {
private:
    struct Operation {
        enum Type { INSERT, EXTRACT, CHANGE_KEY } type;
        T value;
        int index;  // 用于change_key操作
        T old_value;  // 用于change_key操作
        
        Operation(Type t, const T& v, int idx = -1, const T& old_v = T{}) 
            : type(t), value(v), index(idx), old_value(old_v) {}
    };
    
    Heap<T, Compare> heap;
    stack<Operation> history;
    vector<T> elements;  // 保存所有元素用于撤销
    
public:
    // 构造函数
    explicit UndoableHeap(const Compare& cmp = Compare()) : heap(cmp) {}
    
    // 插入元素
    void insert(const T& value) {
        heap.insert(value);
        elements.push_back(value);
        history.push(Operation(Operation::INSERT, value));
    }
    
    // 删除堆顶元素
    T extract() {
        if (heap.empty()) {
            throw runtime_error("Heap is empty");
        }
        
        T value = heap.extract();
        history.push(Operation(Operation::EXTRACT, value));
        return value;
    }
    
    // 查看堆顶元素
    const T& top() const {
        return heap.top();
    }
    
    // 撤销最后一次操作
    bool undo() {
        if (history.empty()) {
            return false;
        }
        
        Operation op = history.top();
        history.pop();
        
        switch (op.type) {
            case Operation::INSERT:
                // 撤销插入：需要重建堆
                elements.pop_back();
                rebuild_heap();
                break;
                
            case Operation::EXTRACT:
                // 撤销删除：重新插入
                heap.insert(op.value);
                break;
                
            case Operation::CHANGE_KEY:
                // 撤销修改：恢复原值
                heap.change_key(op.index, op.old_value);
                break;
        }
        
        return true;
    }
    
    // 撤销多次操作
    int undo(int count) {
        int undone = 0;
        for (int i = 0; i < count && !history.empty(); ++i) {
            if (undo()) {
                undone++;
            }
        }
        return undone;
    }
    
    // 清空历史记录
    void clear_history() {
        while (!history.empty()) {
            history.pop();
        }
    }
    
    // 获取历史记录大小
    size_t history_size() const {
        return history.size();
    }
    
    // 检查是否为空
    bool empty() const {
        return heap.empty();
    }
    
    // 获取大小
    size_t size() const {
        return heap.size();
    }
    
private:
    // 重建堆
    void rebuild_heap() {
        heap = Heap<T, Compare>(elements);
    }
};

// 滑动窗口最大值/最小值
template<typename T, typename Compare = less<T>>
class SlidingWindowHeap {
private:
    struct Element {
        T value;
        int index;
        
        bool operator<(const Element& other) const {
            Compare comp;
            return comp(value, other.value);
        }
        
        bool operator>(const Element& other) const {
            Compare comp;
            return comp(other.value, value);
        }
    };
    
    priority_queue<Element, vector<Element>, greater<Element>> heap;
    int window_size;
    int current_index;
    
public:
    // 构造函数
    explicit SlidingWindowHeap(int size) : window_size(size), current_index(0) {
        if (size <= 0) {
            throw invalid_argument("Window size must be positive");
        }
    }
    
    // 添加元素
    void add(const T& value) {
        heap.push({value, current_index});
        current_index++;
        
        // 清理过期元素
        while (!heap.empty() && heap.top().index <= current_index - window_size - 1) {
            heap.pop();
        }
    }
    
    // 获取窗口内的最值
    T get_extreme() {
        // 清理过期元素
        while (!heap.empty() && heap.top().index <= current_index - window_size - 1) {
            heap.pop();
        }
        
        if (heap.empty()) {
            throw runtime_error("No elements in window");
        }
        
        return heap.top().value;
    }
    
    // 检查是否有足够的元素
    bool has_full_window() const {
        return current_index >= window_size;
    }
    
    // 获取当前窗口大小
    int current_window_size() const {
        return min(current_index, window_size);
    }
};

// 类型别名
template<typename T>
using QuadHeap = DaryHeap<T, 4>;

template<typename T>
using OctalHeap = DaryHeap<T, 8>;

template<typename T>
using IndexedMaxHeap = IndexedHeap<T, less<T>>;

template<typename T>
using IndexedMinHeap = IndexedHeap<T, greater<T>>;

template<typename T>
using ThreadSafeMaxHeap = ThreadSafeHeap<T, less<T>>;

template<typename T>
using ThreadSafeMinHeap = ThreadSafeHeap<T, greater<T>>;

template<typename T>
using UndoableMaxHeap = UndoableHeap<T, less<T>>;

template<typename T>
using UndoableMinHeap = UndoableHeap<T, greater<T>>;

template<typename T>
using SlidingWindowMaxHeap = SlidingWindowHeap<T, less<T>>;

template<typename T>
using SlidingWindowMinHeap = SlidingWindowHeap<T, greater<T>>;

// 实用函数

// 构建d叉堆
template<typename T, int D = 4>
DaryHeap<T, D> build_dary_heap(const vector<T>& arr) {
    return DaryHeap<T, D>(arr);
}

// 构建索引堆
template<typename T>
IndexedHeap<T> build_indexed_heap() {
    return IndexedHeap<T>();
}

// 构建双端堆
template<typename T>
DoubleEndedHeap<T> build_double_ended_heap() {
    return DoubleEndedHeap<T>();
}

// 构建线程安全堆
template<typename T>
ThreadSafeHeap<T> build_thread_safe_heap(const vector<T>& arr) {
    return ThreadSafeHeap<T>(arr);
}

#endif // ADVANCED_HEAP_H