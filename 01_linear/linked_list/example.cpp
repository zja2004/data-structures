#include <iostream>
#include "singly_linked_list.h"
#include "doubly_linked_list.h"

void test_singly_linked_list() {
    std::cout << "=== 单向链表测试 ===" << std::endl;
    
    SinglyLinkedList<int> list;
    
    // 测试基本操作
    std::cout << "\n1. 基本插入操作:" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        list.push_back(i * 10);
        std::cout << "插入 " << i * 10 << ": ";
        list.print();
    }
    
    // 测试头部插入
    std::cout << "\n2. 头部插入:" << std::endl;
    list.push_front(5);
    std::cout << "头部插入5: ";
    list.print();
    
    // 测试元素访问
    std::cout << "\n3. 元素访问:" << std::endl;
    std::cout << "第一个元素: " << list.front() << std::endl;
    std::cout << "最后一个元素: " << list.back() << std::endl;
    std::cout << "索引2的元素: " << list.at(2) << std::endl;
    
    // 测试查找
    std::cout << "\n4. 查找操作:" << std::endl;
    int target = 30;
    int index = list.find(target);
    if (index != -1) {
        std::cout << "找到 " << target << " 在索引 " << index << std::endl;
    }
    std::cout << "是否包含40: " << (list.contains(40) ? "是" : "否") << std::endl;
    
    // 测试插入和删除
    std::cout << "\n5. 插入和删除:" << std::endl;
    list.insert(2, 999);
    std::cout << "在索引2插入999: ";
    list.print();
    
    list.erase(1);
    std::cout << "删除索引1: ";
    list.print();
    
    // 测试反转
    std::cout << "\n6. 反转链表:" << std::endl;
    std::cout << "反转前: ";
    list.print();
    list.reverse();
    std::cout << "反转后: ";
    list.print();
    
    // 测试迭代器
    std::cout << "\n7. 迭代器遍历: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 测试环检测
    std::cout << "\n8. 环检测: " << (list.has_cycle() ? "有环" : "无环") << std::endl;
    
    // 测试中间节点
    std::cout << "\n9. 中间节点索引: " << list.find_middle() << std::endl;
    
    // 测试递归反向打印
    std::cout << "\n10. 递归反向打印:" << std::endl;
    list.print_reverse();
}

void test_doubly_linked_list() {
    std::cout << "\n\n=== 双向链表测试 ===" << std::endl;
    
    DoublyLinkedList<int> list;
    
    // 测试基本操作
    std::cout << "\n1. 基本插入操作:" << std::endl;
    for (int i = 1; i <= 5; ++i) {
        list.push_back(i * 10);
        std::cout << "插入 " << i * 10 << ": ";
        list.print();
    }
    
    // 测试头部和尾部插入
    std::cout << "\n2. 头部和尾部插入:" << std::endl;
    list.push_front(5);
    std::cout << "头部插入5: ";
    list.print();
    
    list.push_back(55);
    std::cout << "尾部插入55: ";
    list.print();
    
    // 测试双向访问
    std::cout << "\n3. 双向访问:" << std::endl;
    std::cout << "正向打印: ";
    list.print();
    std::cout << "反向打印: ";
    list.print_reverse();
    
    // 测试优化的索引访问
    std::cout << "\n4. 索引访问测试:" << std::endl;
    std::cout << "索引0的元素: " << list.at(0) << std::endl;
    std::cout << "索引3的元素: " << list.at(3) << std::endl;
    std::cout << "最后一个元素: " << list.at(list.size() - 1) << std::endl;
    
    // 测试双向查找
    std::cout << "\n5. 双向查找:" << std::endl;
    int target = 30;
    std::cout << "正向查找30: 索引 " << list.find(target) << std::endl;
    std::cout << "反向查找30: 索引 " << list.find_reverse(target) << std::endl;
    
    // 测试插入和删除
    std::cout << "\n6. 插入和删除:" << std::endl;
    list.insert(3, 999);
    std::cout << "在索引3插入999: ";
    list.print();
    
    list.erase(2);
    std::cout << "删除索引2: ";
    list.print();
    
    // 测试头尾删除
    std::cout << "\n7. 头尾删除:" << std::endl;
    list.pop_front();
    std::cout << "删除头部: ";
    list.print();
    
    list.pop_back();
    std::cout << "删除尾部: ";
    list.print();
    
    // 测试反转
    std::cout << "\n8. 反转链表:" << std::endl;
    std::cout << "反转前: ";
    list.print();
    list.reverse();
    std::cout << "反转后: ";
    list.print();
    
    // 测试双向迭代器
    std::cout << "\n9. 双向迭代器:" << std::endl;
    std::cout << "正向遍历: ";
    for (auto it = list.begin(); it != list.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    std::cout << "反向遍历: ";
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

void test_duplicate_removal() {
    std::cout << "\n\n=== 重复元素删除测试 ===" << std::endl;
    
    DoublyLinkedList<int> list;
    int values[] = {1, 2, 2, 3, 3, 3, 4, 5, 5};
    
    for (int i = 0; i < 9; ++i) {
        list.push_back(values[i]);
    }
    
    std::cout << "原始链表: ";
    list.print();
    
    list.remove_duplicates();
    std::cout << "删除重复元素后: ";
    list.print();
}

void test_copy_operations() {
    std::cout << "\n\n=== 拷贝操作测试 ===" << std::endl;
    
    SinglyLinkedList<int> list1;
    for (int i = 1; i <= 5; ++i) {
        list1.push_back(i * 10);
    }
    
    std::cout << "原始链表: ";
    list1.print();
    
    // 拷贝构造
    SinglyLinkedList<int> list2(list1);
    std::cout << "拷贝构造的链表: ";
    list2.print();
    
    // 修改原链表
    list1.push_front(999);
    std::cout << "\n修改原链表后:" << std::endl;
    std::cout << "原链表: ";
    list1.print();
    std::cout << "拷贝链表: ";
    list2.print();
    
    // 赋值操作
    SinglyLinkedList<int> list3;
    list3 = list1;
    std::cout << "\n赋值操作后的链表: ";
    list3.print();
}

void test_performance_comparison() {
    std::cout << "\n\n=== 性能对比测试 ===" << std::endl;
    
    const size_t test_size = 1000;
    
    // 单向链表性能测试
    SinglyLinkedList<int> slist;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < test_size; ++i) {
        slist.push_back(i);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "单向链表插入 " << test_size << " 个元素耗时: " << duration.count() << " 微秒" << std::endl;
    
    // 双向链表性能测试
    DoublyLinkedList<int> dlist;
    start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < test_size; ++i) {
        dlist.push_back(i);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "双向链表插入 " << test_size << " 个元素耗时: " << duration.count() << " 微秒" << std::endl;
    
    // 访问性能对比
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 100; ++i) {
        slist.at(test_size / 2);  // 访问中间元素
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "单向链表访问中间元素100次耗时: " << duration.count() << " 微秒" << std::endl;
    
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < 100; ++i) {
        dlist.at(test_size / 2);  // 访问中间元素
    }
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "双向链表访问中间元素100次耗时: " << duration.count() << " 微秒" << std::endl;
}

int main() {
    std::cout << "C++ 链表 (Linked List) 完整示例" << std::endl;
    std::cout << "=================================" << std::endl;
    
    test_singly_linked_list();
    test_doubly_linked_list();
    test_duplicate_removal();
    test_copy_operations();
    
    // 注意：性能测试需要包含 <chrono> 头文件
    // test_performance_comparison();
    
    std::cout << "\n所有测试完成！" << std::endl;
    
    return 0;
}