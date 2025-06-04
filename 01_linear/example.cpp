#include "vector/vector.h"
#include "linked_list/singly_linked_list.h"
#include "linked_list/doubly_linked_list.h"
#include "stack/array_stack.h"
#include "stack/linked_stack.h"
#include "queue/array_queue.h"
#include "queue/linked_queue.h"
#include "queue/priority_queue.h"
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
#include <cassert>

using namespace std;
using namespace chrono;

// 性能测试工具类
class PerformanceTester {
public:
    template<typename Func>
    static double measure_time(Func func, const string& description) {
        auto start = high_resolution_clock::now();
        func();
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        cout << description << ": " << duration << " 微秒\n";
        return duration;
    }
    
    static void print_separator(const string& title) {
        cout << "\n" << string(50, '=') << "\n";
        cout << "  " << title << "\n";
        cout << string(50, '=') << "\n";
    }
};

// 动态数组测试
void test_vector() {
    PerformanceTester::print_separator("动态数组测试");
    
    cout << "\n1. 基本操作测试\n";
    Vector<int> vec;
    
    // 插入测试
    cout << "插入元素: ";
    for (int i = 0; i < 10; ++i) {
        vec.push_back(i);
        cout << i << " ";
    }
    cout << "\n";
    
    cout << "数组大小: " << vec.size() << ", 容量: " << vec.capacity() << "\n";
    
    // 访问测试
    cout << "随机访问测试: ";
    for (int i = 0; i < vec.size(); i += 2) {
        cout << "vec[" << i << "] = " << vec[i] << " ";
    }
    cout << "\n";
    
    // 插入和删除测试
    vec.insert(5, 999);
    cout << "在位置5插入999后: ";
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << "\n";
    
    vec.erase(5);
    cout << "删除位置5的元素后: ";
    for (int i = 0; i < vec.size(); ++i) {
        cout << vec[i] << " ";
    }
    cout << "\n";
    
    // 查找测试
    int target = 7;
    int index = vec.find(target);
    cout << "查找元素" << target << ": " << (index != -1 ? "找到，位置" + to_string(index) : "未找到") << "\n";
    
    cout << "\n2. 容量管理测试\n";
    Vector<string> str_vec;
    cout << "初始容量: " << str_vec.capacity() << "\n";
    
    for (int i = 0; i < 20; ++i) {
        str_vec.push_back("item" + to_string(i));
        if (i % 5 == 4) {
            cout << "插入" << (i+1) << "个元素后，容量: " << str_vec.capacity() << "\n";
        }
    }
    
    str_vec.reserve(50);
    cout << "预留50个位置后，容量: " << str_vec.capacity() << "\n";
    
    str_vec.shrink_to_fit();
    cout << "收缩到合适大小后，容量: " << str_vec.capacity() << "\n";
}

// 链表测试
void test_linked_list() {
    PerformanceTester::print_separator("链表测试");
    
    cout << "\n1. 单链表测试\n";
    SinglyLinkedList<int> slist;
    
    // 插入测试
    cout << "头部插入: ";
    for (int i = 0; i < 5; ++i) {
        slist.push_front(i);
        cout << i << " ";
    }
    cout << "\n";
    
    cout << "尾部插入: ";
    for (int i = 5; i < 10; ++i) {
        slist.push_back(i);
        cout << i << " ";
    }
    cout << "\n";
    
    cout << "链表内容: ";
    slist.print();
    cout << "链表大小: " << slist.size() << "\n";
    
    // 查找和删除测试
    int target = 7;
    if (slist.find(target)) {
        cout << "找到元素" << target << "\n";
        slist.remove(target);
        cout << "删除元素" << target << "后: ";
        slist.print();
    }
    
    cout << "\n2. 双链表测试\n";
    DoublyLinkedList<string> dlist;
    
    // 双向插入测试
    vector<string> words = {"apple", "banana", "cherry", "date", "elderberry"};
    
    cout << "交替从头尾插入: ";
    for (int i = 0; i < words.size(); ++i) {
        if (i % 2 == 0) {
            dlist.push_front(words[i]);
            cout << "[头]" << words[i] << " ";
        } else {
            dlist.push_back(words[i]);
            cout << "[尾]" << words[i] << " ";
        }
    }
    cout << "\n";
    
    cout << "正向遍历: ";
    dlist.print_forward();
    
    cout << "反向遍历: ";
    dlist.print_backward();
    
    // 中间插入测试
    dlist.insert(2, "coconut");
    cout << "在位置2插入coconut后: ";
    dlist.print_forward();
}

// 栈测试
void test_stack() {
    PerformanceTester::print_separator("栈测试");
    
    cout << "\n1. 数组栈测试\n";
    ArrayStack<int> array_stack;
    
    cout << "入栈操作: ";
    for (int i = 1; i <= 10; ++i) {
        array_stack.push(i * i);
        cout << i * i << " ";
    }
    cout << "\n";
    
    cout << "栈大小: " << array_stack.size() << "\n";
    cout << "栈顶元素: " << array_stack.top() << "\n";
    
    cout << "出栈操作: ";
    while (!array_stack.empty()) {
        cout << array_stack.top() << " ";
        array_stack.pop();
    }
    cout << "\n";
    
    cout << "\n2. 链式栈测试\n";
    LinkedStack<string> linked_stack;
    
    vector<string> expressions = {"(", "a", "+", "b", ")", "*", "c"};
    
    cout << "处理表达式: ";
    for (const string& token : expressions) {
        cout << token << " ";
        if (token == "(" || token == "+" || token == "*") {
            linked_stack.push(token);
        } else if (token == ")") {
            if (!linked_stack.empty()) {
                cout << "[匹配" << linked_stack.top() << "] ";
                linked_stack.pop();
            }
        }
    }
    cout << "\n";
    
    cout << "\n3. 栈的应用：括号匹配\n";
    auto check_brackets = [](const string& expr) {
        ArrayStack<char> stack;
        for (char c : expr) {
            if (c == '(' || c == '[' || c == '{') {
                stack.push(c);
            } else if (c == ')' || c == ']' || c == '}') {
                if (stack.empty()) return false;
                char top = stack.top();
                stack.pop();
                if ((c == ')' && top != '(') ||
                    (c == ']' && top != '[') ||
                    (c == '}' && top != '{')) {
                    return false;
                }
            }
        }
        return stack.empty();
    };
    
    vector<string> test_cases = {
        "(a+b)*c",
        "[a+(b*c)]",
        "{a+[b*(c+d)]}",
        "(a+b])*c",
        "((a+b)"
    };
    
    for (const string& expr : test_cases) {
        cout << "表达式 \"" << expr << "\": " 
             << (check_brackets(expr) ? "括号匹配" : "括号不匹配") << "\n";
    }
}

// 队列测试
void test_queue() {
    PerformanceTester::print_separator("队列测试");
    
    cout << "\n1. 数组队列测试\n";
    ArrayQueue<int> array_queue(8);
    
    cout << "入队操作: ";
    for (int i = 1; i <= 6; ++i) {
        array_queue.enqueue(i * 10);
        cout << i * 10 << " ";
    }
    cout << "\n";
    
    cout << "队列大小: " << array_queue.size() << "\n";
    cout << "队头元素: " << array_queue.front() << "\n";
    cout << "队尾元素: " << array_queue.back() << "\n";
    
    cout << "出队操作: ";
    for (int i = 0; i < 3; ++i) {
        cout << array_queue.front() << " ";
        array_queue.dequeue();
    }
    cout << "\n";
    
    cout << "继续入队: ";
    for (int i = 7; i <= 9; ++i) {
        array_queue.enqueue(i * 10);
        cout << i * 10 << " ";
    }
    cout << "\n";
    
    cout << "\n2. 链式队列测试\n";
    LinkedQueue<string> linked_queue;
    
    vector<string> tasks = {"任务A", "任务B", "任务C", "任务D", "任务E"};
    
    cout << "添加任务: ";
    for (const string& task : tasks) {
        linked_queue.enqueue(task);
        cout << task << " ";
    }
    cout << "\n";
    
    cout << "处理任务: ";
    while (!linked_queue.empty()) {
        cout << "正在处理" << linked_queue.front() << " ";
        linked_queue.dequeue();
    }
    cout << "\n";
    
    cout << "\n3. 优先队列测试\n";
    PriorityQueue<int> pq;
    
    vector<int> priorities = {3, 1, 4, 1, 5, 9, 2, 6};
    
    cout << "插入优先级: ";
    for (int p : priorities) {
        pq.push(p);
        cout << p << " ";
    }
    cout << "\n";
    
    cout << "按优先级处理: ";
    while (!pq.empty()) {
        cout << pq.top() << " ";
        pq.pop();
    }
    cout << "\n";
}

// 性能对比测试
void performance_comparison() {
    PerformanceTester::print_separator("性能对比测试");
    
    const int n = 100000;
    
    cout << "\n测试规模: " << n << " 个元素\n";
    
    // 随机访问性能对比
    cout << "\n1. 随机访问性能对比\n";
    
    Vector<int> vec;
    for (int i = 0; i < n; ++i) {
        vec.push_back(i);
    }
    
    SinglyLinkedList<int> list;
    for (int i = 0; i < min(n, 10000); ++i) {  // 链表测试规模较小
        list.push_back(i);
    }
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, n - 1);
    
    // 动态数组随机访问
    PerformanceTester::measure_time([&]() {
        int sum = 0;
        for (int i = 0; i < 10000; ++i) {
            int index = dis(gen);
            sum += vec[index];
        }
    }, "动态数组随机访问10000次");
    
    // 链表顺序访问（模拟随机访问的代价）
    uniform_int_distribution<int> small_dis(0, min(n, 10000) - 1);
    PerformanceTester::measure_time([&]() {
        int sum = 0;
        for (int i = 0; i < 1000; ++i) {  // 减少测试次数
            int index = small_dis(gen);
            int value = list.get(index);
            sum += value;
        }
    }, "链表模拟随机访问1000次");
    
    // 插入性能对比
    cout << "\n2. 插入性能对比\n";
    
    Vector<int> vec2;
    PerformanceTester::measure_time([&]() {
        for (int i = 0; i < n; ++i) {
            vec2.push_back(i);
        }
    }, "动态数组尾部插入");
    
    SinglyLinkedList<int> list2;
    PerformanceTester::measure_time([&]() {
        for (int i = 0; i < n; ++i) {
            list2.push_front(i);
        }
    }, "链表头部插入");
    
    // 栈操作性能对比
    cout << "\n3. 栈操作性能对比\n";
    
    ArrayStack<int> array_stack;
    PerformanceTester::measure_time([&]() {
        for (int i = 0; i < n; ++i) {
            array_stack.push(i);
        }
        while (!array_stack.empty()) {
            array_stack.pop();
        }
    }, "数组栈push/pop操作");
    
    LinkedStack<int> linked_stack;
    PerformanceTester::measure_time([&]() {
        for (int i = 0; i < n; ++i) {
            linked_stack.push(i);
        }
        while (!linked_stack.empty()) {
            linked_stack.pop();
        }
    }, "链式栈push/pop操作");
    
    // 队列操作性能对比
    cout << "\n4. 队列操作性能对比\n";
    
    ArrayQueue<int> array_queue(n + 1000);
    PerformanceTester::measure_time([&]() {
        for (int i = 0; i < n; ++i) {
            array_queue.enqueue(i);
        }
        while (!array_queue.empty()) {
            array_queue.dequeue();
        }
    }, "数组队列enqueue/dequeue操作");
    
    LinkedQueue<int> linked_queue;
    PerformanceTester::measure_time([&]() {
        for (int i = 0; i < n; ++i) {
            linked_queue.enqueue(i);
        }
        while (!linked_queue.empty()) {
            linked_queue.dequeue();
        }
    }, "链式队列enqueue/dequeue操作");
}

// 实际应用场景演示
void application_scenarios() {
    PerformanceTester::print_separator("实际应用场景演示");
    
    cout << "\n1. 文本编辑器的撤销功能（栈）\n";
    
    class TextEditor {
    private:
        string content;
        ArrayStack<string> undo_stack;
        ArrayStack<string> redo_stack;
        
    public:
        void type(const string& text) {
            undo_stack.push(content);
            content += text;
            // 清空redo栈
            while (!redo_stack.empty()) {
                redo_stack.pop();
            }
            cout << "输入: \"" << text << "\", 当前内容: \"" << content << "\"\n";
        }
        
        void undo() {
            if (!undo_stack.empty()) {
                redo_stack.push(content);
                content = undo_stack.top();
                undo_stack.pop();
                cout << "撤销操作, 当前内容: \"" << content << "\"\n";
            } else {
                cout << "无法撤销\n";
            }
        }
        
        void redo() {
            if (!redo_stack.empty()) {
                undo_stack.push(content);
                content = redo_stack.top();
                redo_stack.pop();
                cout << "重做操作, 当前内容: \"" << content << "\"\n";
            } else {
                cout << "无法重做\n";
            }
        }
    };
    
    TextEditor editor;
    editor.type("Hello");
    editor.type(" World");
    editor.type("!");
    editor.undo();
    editor.undo();
    editor.redo();
    editor.type(" C++");
    
    cout << "\n2. 任务调度系统（队列）\n";
    
    class TaskScheduler {
    private:
        LinkedQueue<string> normal_tasks;
        PriorityQueue<pair<int, string>> priority_tasks;  // (priority, task)
        
    public:
        void add_normal_task(const string& task) {
            normal_tasks.enqueue(task);
            cout << "添加普通任务: " << task << "\n";
        }
        
        void add_priority_task(const string& task, int priority) {
            priority_tasks.push({priority, task});
            cout << "添加优先任务: " << task << " (优先级: " << priority << ")\n";
        }
        
        void process_tasks() {
            cout << "\n开始处理任务:\n";
            
            // 先处理优先任务
            while (!priority_tasks.empty()) {
                auto task = priority_tasks.top();
                priority_tasks.pop();
                cout << "处理优先任务: " << task.second 
                     << " (优先级: " << task.first << ")\n";
            }
            
            // 再处理普通任务
            while (!normal_tasks.empty()) {
                cout << "处理普通任务: " << normal_tasks.front() << "\n";
                normal_tasks.dequeue();
            }
        }
    };
    
    TaskScheduler scheduler;
    scheduler.add_normal_task("发送邮件");
    scheduler.add_priority_task("处理紧急bug", 9);
    scheduler.add_normal_task("更新文档");
    scheduler.add_priority_task("备份数据库", 7);
    scheduler.add_priority_task("系统维护", 8);
    scheduler.process_tasks();
    
    cout << "\n3. 浏览器历史记录（动态数组）\n";
    
    class BrowserHistory {
    private:
        Vector<string> history;
        int current_index;
        
    public:
        BrowserHistory() : current_index(-1) {}
        
        void visit(const string& url) {
            // 删除当前位置之后的历史记录
            while (history.size() > current_index + 1) {
                history.pop_back();
            }
            
            history.push_back(url);
            current_index++;
            cout << "访问: " << url << "\n";
        }
        
        void back() {
            if (current_index > 0) {
                current_index--;
                cout << "后退到: " << history[current_index] << "\n";
            } else {
                cout << "无法后退\n";
            }
        }
        
        void forward() {
            if (current_index < history.size() - 1) {
                current_index++;
                cout << "前进到: " << history[current_index] << "\n";
            } else {
                cout << "无法前进\n";
            }
        }
        
        void show_history() {
            cout << "浏览历史:\n";
            for (int i = 0; i < history.size(); ++i) {
                cout << (i == current_index ? "-> " : "   ") 
                     << history[i] << "\n";
            }
        }
    };
    
    BrowserHistory browser;
    browser.visit("https://google.com");
    browser.visit("https://github.com");
    browser.visit("https://stackoverflow.com");
    browser.back();
    browser.back();
    browser.visit("https://cppreference.com");
    browser.forward();
    browser.show_history();
}

// 正确性测试
void correctness_tests() {
    PerformanceTester::print_separator("正确性测试");
    
    cout << "\n运行所有数据结构的正确性测试...\n";
    
    // 动态数组测试
    {
        Vector<int> vec;
        assert(vec.empty());
        assert(vec.size() == 0);
        
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);
        assert(vec.size() == 3);
        assert(vec[0] == 1 && vec[1] == 2 && vec[2] == 3);
        
        vec.insert(1, 10);
        assert(vec.size() == 4);
        assert(vec[1] == 10);
        
        vec.erase(1);
        assert(vec.size() == 3);
        assert(vec[1] == 2);
        
        cout << "✓ 动态数组测试通过\n";
    }
    
    // 栈测试
    {
        ArrayStack<int> stack;
        assert(stack.empty());
        
        stack.push(1);
        stack.push(2);
        stack.push(3);
        assert(stack.size() == 3);
        assert(stack.top() == 3);
        
        stack.pop();
        assert(stack.top() == 2);
        assert(stack.size() == 2);
        
        cout << "✓ 栈测试通过\n";
    }
    
    // 队列测试
    {
        LinkedQueue<int> queue;
        assert(queue.empty());
        
        queue.enqueue(1);
        queue.enqueue(2);
        queue.enqueue(3);
        assert(queue.size() == 3);
        assert(queue.front() == 1);
        assert(queue.back() == 3);
        
        queue.dequeue();
        assert(queue.front() == 2);
        assert(queue.size() == 2);
        
        cout << "✓ 队列测试通过\n";
    }
    
    cout << "\n所有正确性测试通过！\n";
}

int main() {
    cout << "C++线性数据结构教程示例\n";
    cout << "========================\n";
    
    try {
        // 基础功能测试
        test_vector();
        test_linked_list();
        test_stack();
        test_queue();
        
        // 性能对比测试
        performance_comparison();
        
        // 实际应用场景
        application_scenarios();
        
        // 正确性测试
        correctness_tests();
        
        PerformanceTester::print_separator("测试完成");
        cout << "\n所有测试成功完成！\n";
        cout << "\n学习建议:\n";
        cout << "1. 理解每种数据结构的特点和适用场景\n";
        cout << "2. 关注时间复杂度和空间复杂度的差异\n";
        cout << "3. 在实际项目中选择合适的数据结构\n";
        cout << "4. 深入学习STL中对应的容器实现\n";
        
    } catch (const exception& e) {
        cerr << "测试过程中发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}