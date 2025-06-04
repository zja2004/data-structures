#include "array_queue.h"
#include "linked_queue.h"
#include "priority_queue.h"
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

void test_array_queue() {
    std::cout << "=== 测试基于数组的队列 ===" << std::endl;
    
    ArrayQueue<int> queue;
    
    // 基本操作
    std::cout << "\n1. 基本操作测试:" << std::endl;
    queue.print();
    
    for (int i = 1; i <= 5; ++i) {
        queue.enqueue(i * 10);
        std::cout << "入队 " << i * 10 << ": ";
        queue.print();
    }
    
    std::cout << "队首元素: " << queue.front() << std::endl;
    std::cout << "队尾元素: " << queue.back() << std::endl;
    
    while (!queue.empty()) {
        std::cout << "出队 " << queue.front() << ": ";
        queue.dequeue();
        queue.print();
    }
    
    // 测试扩容
    std::cout << "\n2. 扩容测试:" << std::endl;
    for (int i = 1; i <= 10; ++i) {
        queue.enqueue(i);
    }
    queue.print();
    std::cout << "容量: " << queue.capacity() << std::endl;
    
    // 测试压缩
    for (int i = 0; i < 6; ++i) {
        queue.dequeue();
    }
    queue.print();
    std::cout << "容量: " << queue.capacity() << std::endl;
    
    queue.shrink_to_fit();
    queue.print();
    std::cout << "压缩后容量: " << queue.capacity() << std::endl;
}

void test_circular_queue() {
    std::cout << "\n=== 测试循环队列 ===" << std::endl;
    
    CircularArrayQueue<char> queue(5);
    
    std::cout << "\n1. 基本操作测试:" << std::endl;
    queue.print();
    queue.print_internal();
    
    // 填满队列
    for (char c = 'A'; c <= 'E'; ++c) {
        queue.enqueue(c);
        std::cout << "入队 " << c << ": ";
        queue.print();
        queue.print_internal();
    }
    
    // 测试满队列
    try {
        queue.enqueue('F');
    } catch (const std::exception& e) {
        std::cout << "队列已满: " << e.what() << std::endl;
    }
    
    // 出队几个元素
    for (int i = 0; i < 2; ++i) {
        std::cout << "出队 " << queue.front() << ": ";
        queue.dequeue();
        queue.print();
        queue.print_internal();
    }
    
    // 再入队几个元素（测试循环）
    for (char c = 'F'; c <= 'G'; ++c) {
        queue.enqueue(c);
        std::cout << "入队 " << c << ": ";
        queue.print();
        queue.print_internal();
    }
}

void test_dynamic_circular_queue() {
    std::cout << "\n=== 测试动态循环队列 ===" << std::endl;
    
    DynamicCircularQueue<int> queue(4);
    
    std::cout << "\n1. 扩容测试:" << std::endl;
    for (int i = 1; i <= 8; ++i) {
        queue.enqueue(i * 10);
        queue.print();
    }
    
    std::cout << "\n2. 缩容测试:" << std::endl;
    while (queue.size() > 1) {
        queue.dequeue();
        queue.print();
    }
}

void test_linked_queue() {
    std::cout << "\n=== 测试基于链表的队列 ===" << std::endl;
    
    LinkedQueue<std::string> queue;
    
    // 基本操作
    std::cout << "\n1. 基本操作测试:" << std::endl;
    queue.print();
    
    std::vector<std::string> words = {"Hello", "World", "Queue", "Test"};
    for (const auto& word : words) {
        queue.enqueue(word);
        std::cout << "入队 " << word << ": ";
        queue.print();
    }
    
    std::cout << "队首: " << queue.front() << std::endl;
    std::cout << "队尾: " << queue.back() << std::endl;
    
    // 拷贝测试
    std::cout << "\n2. 拷贝测试:" << std::endl;
    LinkedQueue<std::string> queue_copy = queue;
    std::cout << "原队列: ";
    queue.print();
    std::cout << "拷贝队列: ";
    queue_copy.print();
    
    queue.dequeue();
    std::cout << "原队列出队后: ";
    queue.print();
    std::cout << "拷贝队列: ";
    queue_copy.print();
    
    // 反向打印测试
    std::cout << "\n3. 反向打印测试:" << std::endl;
    queue_copy.print_reverse();
}

void test_linked_deque() {
    std::cout << "\n=== 测试双端队列 ===" << std::endl;
    
    LinkedDeque<int> deque;
    
    std::cout << "\n1. 双端操作测试:" << std::endl;
    deque.print();
    
    // 前端插入
    for (int i = 1; i <= 3; ++i) {
        deque.push_front(i);
        std::cout << "前端插入 " << i << ": ";
        deque.print();
    }
    
    // 后端插入
    for (int i = 10; i <= 12; ++i) {
        deque.push_back(i);
        std::cout << "后端插入 " << i << ": ";
        deque.print();
    }
    
    std::cout << "\n正向打印: ";
    deque.print();
    std::cout << "反向打印: ";
    deque.print_reverse();
    
    // 双端删除
    std::cout << "\n2. 双端删除测试:" << std::endl;
    deque.pop_front();
    std::cout << "前端删除后: ";
    deque.print();
    
    deque.pop_back();
    std::cout << "后端删除后: ";
    deque.print();
    
    // 作为栈使用
    std::cout << "\n3. 作为栈使用:" << std::endl;
    deque.push(100);
    deque.push(200);
    std::cout << "栈顶: " << deque.top() << std::endl;
    deque.print();
    
    deque.pop();
    std::cout << "弹栈后: ";
    deque.print();
}

void test_queue_using_two_stacks() {
    std::cout << "\n=== 测试用两个栈实现的队列 ===" << std::endl;
    
    QueueUsingTwoStacks<int> queue;
    
    std::cout << "\n1. 基本操作测试:" << std::endl;
    queue.print();
    queue.print_stacks();
    
    // 入队操作
    for (int i = 1; i <= 5; ++i) {
        queue.enqueue(i * 10);
        std::cout << "\n入队 " << i * 10 << ":" << std::endl;
        queue.print();
        queue.print_stacks();
    }
    
    // 出队操作
    std::cout << "\n2. 出队操作:" << std::endl;
    for (int i = 0; i < 2; ++i) {
        std::cout << "\n出队 " << queue.front() << ":" << std::endl;
        queue.dequeue();
        queue.print();
        queue.print_stacks();
    }
    
    // 混合操作
    std::cout << "\n3. 混合操作:" << std::endl;
    queue.enqueue(60);
    std::cout << "\n入队 60:" << std::endl;
    queue.print();
    queue.print_stacks();
    
    std::cout << "\n出队 " << queue.front() << ":" << std::endl;
    queue.dequeue();
    queue.print();
    queue.print_stacks();
}

void test_priority_queue() {
    std::cout << "\n=== 测试优先队列 ===" << std::endl;
    
    // 最大堆
    std::cout << "\n1. 最大堆测试:" << std::endl;
    MaxPriorityQueue<int> max_heap;
    
    std::vector<int> values = {3, 1, 4, 1, 5, 9, 2, 6};
    for (int val : values) {
        max_heap.push(val);
        std::cout << "插入 " << val << ": ";
        max_heap.print();
    }
    
    std::cout << "\n树形结构:" << std::endl;
    max_heap.print_tree();
    
    std::cout << "\n堆验证: " << (max_heap.is_valid_heap() ? "有效" : "无效") << std::endl;
    
    std::cout << "\n删除操作:" << std::endl;
    while (!max_heap.empty()) {
        std::cout << "删除最大值 " << max_heap.top() << ": ";
        max_heap.pop();
        max_heap.print();
    }
    
    // 最小堆
    std::cout << "\n2. 最小堆测试:" << std::endl;
    MinPriorityQueue<int> min_heap({5, 2, 8, 1, 9, 3});
    
    std::cout << "初始状态: ";
    min_heap.print();
    min_heap.print_tree();
    
    while (!min_heap.empty()) {
        std::cout << "删除最小值 " << min_heap.top() << ": ";
        min_heap.pop();
        min_heap.print();
    }
}

void test_updatable_priority_queue() {
    std::cout << "\n=== 测试可更新优先队列 ===" << std::endl;
    
    UpdatablePriorityQueue<int> upq;
    
    std::cout << "\n1. 插入和更新测试:" << std::endl;
    
    // 插入元素
    std::vector<size_t> ids;
    std::vector<int> values = {10, 5, 20, 3, 15};
    
    for (int val : values) {
        size_t id = upq.push(val);
        ids.push_back(id);
        std::cout << "插入 " << val << " (id: " << id << "): ";
        upq.print();
    }
    
    // 更新元素
    std::cout << "\n2. 更新操作:" << std::endl;
    upq.update(ids[1], 25);  // 将5更新为25
    std::cout << "更新id " << ids[1] << " 为 25: ";
    upq.print();
    
    upq.update(ids[2], 1);   // 将20更新为1
    std::cout << "更新id " << ids[2] << " 为 1: ";
    upq.print();
    
    // 删除元素
    std::cout << "\n3. 删除操作:" << std::endl;
    upq.erase(ids[0]);  // 删除原来的10
    std::cout << "删除id " << ids[0] << ": ";
    upq.print();
    
    // 检查元素
    std::cout << "\n4. 元素查询:" << std::endl;
    for (size_t id : ids) {
        if (upq.contains(id)) {
            std::cout << "id " << id << " 的值: " << upq.get(id) << std::endl;
        } else {
            std::cout << "id " << id << " 不存在" << std::endl;
        }
    }
}

// 经典算法：滑动窗口最大值
void sliding_window_maximum() {
    std::cout << "\n=== 滑动窗口最大值算法 ===" << std::endl;
    
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;
    
    std::cout << "数组: [";
    for (size_t i = 0; i < nums.size(); ++i) {
        std::cout << nums[i];
        if (i < nums.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "窗口大小: " << k << std::endl;
    
    // 使用双端队列解决
    LinkedDeque<int> deque;  // 存储数组索引
    std::vector<int> result;
    
    for (size_t i = 0; i < nums.size(); ++i) {
        // 移除超出窗口的元素
        while (!deque.empty() && deque.front() <= (int)i - k) {
            deque.pop_front();
        }
        
        // 移除所有小于当前元素的元素
        while (!deque.empty() && nums[deque.back()] <= nums[i]) {
            deque.pop_back();
        }
        
        deque.push_back(i);
        
        // 如果窗口大小达到k，记录最大值
        if (i >= k - 1) {
            result.push_back(nums[deque.front()]);
        }
    }
    
    std::cout << "滑动窗口最大值: [";
    for (size_t i = 0; i < result.size(); ++i) {
        std::cout << result[i];
        if (i < result.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

// 经典算法：任务调度
void task_scheduling() {
    std::cout << "\n=== 任务调度算法 ===" << std::endl;
    
    struct Task {
        int id;
        int priority;
        int duration;
        
        Task(int i, int p, int d) : id(i), priority(p), duration(d) {}
        
        bool operator<(const Task& other) const {
            return priority < other.priority;  // 优先级高的先执行
        }
    };
    
    MaxPriorityQueue<Task> task_queue;
    
    // 添加任务
    std::vector<Task> tasks = {
        {1, 3, 5}, {2, 1, 3}, {3, 4, 2}, {4, 2, 4}, {5, 5, 1}
    };
    
    std::cout << "添加任务:" << std::endl;
    for (const auto& task : tasks) {
        task_queue.push(task);
        std::cout << "任务 " << task.id << " (优先级: " << task.priority 
                  << ", 时长: " << task.duration << ")" << std::endl;
    }
    
    std::cout << "\n执行任务:" << std::endl;
    int current_time = 0;
    while (!task_queue.empty()) {
        Task current_task = task_queue.top();
        task_queue.pop();
        
        std::cout << "时间 " << current_time << ": 执行任务 " << current_task.id
                  << " (优先级: " << current_task.priority << ")" << std::endl;
        
        current_time += current_task.duration;
    }
    
    std::cout << "所有任务完成，总时间: " << current_time << std::endl;
}

int main() {
    try {
        test_array_queue();
        test_circular_queue();
        test_dynamic_circular_queue();
        test_linked_queue();
        test_linked_deque();
        test_queue_using_two_stacks();
        test_priority_queue();
        test_updatable_priority_queue();
        
        std::cout << "\n" << std::string(50, '=') << std::endl;
        std::cout << "经典算法演示" << std::endl;
        std::cout << std::string(50, '=') << std::endl;
        
        sliding_window_maximum();
        task_scheduling();
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}