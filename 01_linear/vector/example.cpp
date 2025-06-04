#include <iostream>
#include "vector.h"

void test_basic_operations() {
    std::cout << "=== 基本操作测试 ===" << std::endl;
    
    // 创建空向量
    Vector<int> vec;
    std::cout << "创建空向量，大小: " << vec.size() << ", 容量: " << vec.capacity() << std::endl;
    
    // 添加元素
    for (int i = 1; i <= 5; ++i) {
        vec.push_back(i * 10);
        std::cout << "添加 " << i * 10 << " 后: ";
        vec.print();
    }
    
    // 访问元素
    std::cout << "\n元素访问:" << std::endl;
    std::cout << "第一个元素: " << vec.front() << std::endl;
    std::cout << "最后一个元素: " << vec.back() << std::endl;
    std::cout << "索引2的元素: " << vec[2] << std::endl;
    
    // 修改元素
    vec[2] = 999;
    std::cout << "修改索引2为999后: ";
    vec.print();
}

void test_insert_erase() {
    std::cout << "\n=== 插入和删除测试 ===" << std::endl;
    
    Vector<int> vec;
    for (int i = 1; i <= 5; ++i) {
        vec.push_back(i);
    }
    
    std::cout << "初始向量: ";
    vec.print();
    
    // 在中间插入元素
    vec.insert(2, 100);
    std::cout << "在索引2插入100: ";
    vec.print();
    
    // 删除元素
    vec.erase(1);
    std::cout << "删除索引1的元素: ";
    vec.print();
    
    // 删除末尾元素
    vec.pop_back();
    std::cout << "删除末尾元素: ";
    vec.print();
}

void test_search_operations() {
    std::cout << "\n=== 查找操作测试 ===" << std::endl;
    
    Vector<int> vec;
    int values[] = {10, 20, 30, 20, 40};
    for (int i = 0; i < 5; ++i) {
        vec.push_back(values[i]);
    }
    
    std::cout << "测试向量: ";
    vec.print();
    
    // 查找元素
    int target = 20;
    int index = vec.find(target);
    if (index != -1) {
        std::cout << "找到 " << target << " 在索引 " << index << std::endl;
    } else {
        std::cout << "未找到 " << target << std::endl;
    }
    
    // 检查是否包含
    std::cout << "是否包含30: " << (vec.contains(30) ? "是" : "否") << std::endl;
    std::cout << "是否包含50: " << (vec.contains(50) ? "是" : "否") << std::endl;
}

void test_capacity_management() {
    std::cout << "\n=== 容量管理测试 ===" << std::endl;
    
    Vector<int> vec;
    
    std::cout << "添加元素过程中的容量变化:" << std::endl;
    for (int i = 1; i <= 10; ++i) {
        vec.push_back(i);
        std::cout << "添加 " << i << " 后 - 大小: " << vec.size() 
                  << ", 容量: " << vec.capacity() << std::endl;
    }
    
    // 预留容量
    vec.reserve(20);
    std::cout << "\n预留容量20后 - 大小: " << vec.size() 
              << ", 容量: " << vec.capacity() << std::endl;
    
    // 缩减容量
    vec.shrink_to_fit();
    std::cout << "缩减容量后 - 大小: " << vec.size() 
              << ", 容量: " << vec.capacity() << std::endl;
}

void test_copy_operations() {
    std::cout << "\n=== 拷贝操作测试 ===" << std::endl;
    
    Vector<int> vec1;
    for (int i = 1; i <= 5; ++i) {
        vec1.push_back(i * 10);
    }
    
    std::cout << "原始向量: ";
    vec1.print();
    
    // 拷贝构造
    Vector<int> vec2(vec1);
    std::cout << "拷贝构造的向量: ";
    vec2.print();
    
    // 修改原向量
    vec1[0] = 999;
    std::cout << "\n修改原向量后:" << std::endl;
    std::cout << "原向量: ";
    vec1.print();
    std::cout << "拷贝向量: ";
    vec2.print();
    
    // 赋值操作
    Vector<int> vec3;
    vec3 = vec1;
    std::cout << "\n赋值操作后的向量: ";
    vec3.print();
}

void test_utility_functions() {
    std::cout << "\n=== 实用函数测试 ===" << std::endl;
    
    Vector<int> vec;
    int values[] = {5, 2, 8, 1, 9, 3};
    for (int i = 0; i < 6; ++i) {
        vec.push_back(values[i]);
    }
    
    std::cout << "原始向量: ";
    vec.print();
    
    // 反转
    vec.reverse();
    std::cout << "反转后: ";
    vec.print();
    
    // 排序
    vec.sort();
    std::cout << "排序后: ";
    vec.print();
}

void test_iterator_support() {
    std::cout << "\n=== 迭代器支持测试 ===" << std::endl;
    
    Vector<int> vec;
    for (int i = 1; i <= 5; ++i) {
        vec.push_back(i * 10);
    }
    
    std::cout << "使用迭代器遍历: ";
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // 使用范围for循环
    std::cout << "使用范围for循环: ";
    for (const auto& value : vec) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}

void test_exception_handling() {
    std::cout << "\n=== 异常处理测试 ===" << std::endl;
    
    Vector<int> vec;
    
    try {
        // 尝试访问空向量的元素
        std::cout << "尝试访问空向量的front(): ";
        std::cout << vec.front() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }
    
    vec.push_back(10);
    vec.push_back(20);
    
    try {
        // 尝试访问越界索引
        std::cout << "尝试访问索引10: ";
        std::cout << vec.at(10) << std::endl;
    } catch (const std::exception& e) {
        std::cout << "捕获异常: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "C++ 动态数组 (Vector) 完整示例" << std::endl;
    std::cout << "================================" << std::endl;
    
    test_basic_operations();
    test_insert_erase();
    test_search_operations();
    test_capacity_management();
    test_copy_operations();
    test_utility_functions();
    test_iterator_support();
    test_exception_handling();
    
    std::cout << "\n所有测试完成！" << std::endl;
    
    return 0;
}