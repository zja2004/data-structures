#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <stdexcept>
#include <algorithm>

template<typename T>
class Vector {
private:
    T* data;           // 指向数据的指针
    size_t size_;      // 当前元素数量
    size_t capacity_;  // 当前容量
    
    // 扩容函数
    void resize();
    
    // 检查索引是否有效
    void check_bounds(size_t index) const;
    
public:
    // 构造函数
    Vector();
    explicit Vector(size_t initial_capacity);
    Vector(size_t count, const T& value);
    Vector(const Vector& other);  // 拷贝构造函数
    
    // 析构函数
    ~Vector();
    
    // 赋值操作符
    Vector& operator=(const Vector& other);
    
    // 容量相关
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }
    void reserve(size_t new_capacity);
    void shrink_to_fit();
    
    // 元素访问
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    T& at(size_t index);
    const T& at(size_t index) const;
    T& front();
    const T& front() const;
    T& back();
    const T& back() const;
    T* data_ptr() { return data; }
    const T* data_ptr() const { return data; }
    
    // 修改操作
    void push_back(const T& value);
    void pop_back();
    void insert(size_t index, const T& value);
    void erase(size_t index);
    void clear();
    
    // 查找操作
    int find(const T& value) const;
    bool contains(const T& value) const;
    
    // 实用函数
    void print() const;
    void reverse();
    void sort();
    
    // 迭代器支持（简化版）
    T* begin() { return data; }
    T* end() { return data + size_; }
    const T* begin() const { return data; }
    const T* end() const { return data + size_; }
};

// 模板类的实现需要在头文件中
#include "vector.cpp"

#endif // VECTOR_H