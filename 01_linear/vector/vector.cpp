#ifndef VECTOR_CPP
#define VECTOR_CPP

#include "vector.h"

// 默认构造函数
template<typename T>
Vector<T>::Vector() : data(nullptr), size_(0), capacity_(0) {}

// 带初始容量的构造函数
template<typename T>
Vector<T>::Vector(size_t initial_capacity) : size_(0), capacity_(initial_capacity) {
    data = new T[capacity_];
}

// 带初始值的构造函数
template<typename T>
Vector<T>::Vector(size_t count, const T& value) : size_(count), capacity_(count) {
    data = new T[capacity_];
    for (size_t i = 0; i < size_; ++i) {
        data[i] = value;
    }
}

// 拷贝构造函数
template<typename T>
Vector<T>::Vector(const Vector& other) : size_(other.size_), capacity_(other.capacity_) {
    data = new T[capacity_];
    for (size_t i = 0; i < size_; ++i) {
        data[i] = other.data[i];
    }
}

// 析构函数
template<typename T>
Vector<T>::~Vector() {
    delete[] data;
}

// 赋值操作符
template<typename T>
Vector<T>& Vector<T>::operator=(const Vector& other) {
    if (this != &other) {
        delete[] data;
        size_ = other.size_;
        capacity_ = other.capacity_;
        data = new T[capacity_];
        for (size_t i = 0; i < size_; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

// 扩容函数
template<typename T>
void Vector<T>::resize() {
    size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
    T* new_data = new T[new_capacity];
    
    // 复制原有数据
    for (size_t i = 0; i < size_; ++i) {
        new_data[i] = data[i];
    }
    
    delete[] data;
    data = new_data;
    capacity_ = new_capacity;
}

// 检查索引边界
template<typename T>
void Vector<T>::check_bounds(size_t index) const {
    if (index >= size_) {
        throw std::out_of_range("Index out of range");
    }
}

// 预留容量
template<typename T>
void Vector<T>::reserve(size_t new_capacity) {
    if (new_capacity > capacity_) {
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        capacity_ = new_capacity;
    }
}

// 缩减到合适大小
template<typename T>
void Vector<T>::shrink_to_fit() {
    if (size_ < capacity_) {
        T* new_data = new T[size_];
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        capacity_ = size_;
    }
}

// 索引访问操作符
template<typename T>
T& Vector<T>::operator[](size_t index) {
    return data[index];
}

template<typename T>
const T& Vector<T>::operator[](size_t index) const {
    return data[index];
}

// 安全的索引访问
template<typename T>
T& Vector<T>::at(size_t index) {
    check_bounds(index);
    return data[index];
}

template<typename T>
const T& Vector<T>::at(size_t index) const {
    check_bounds(index);
    return data[index];
}

// 访问第一个元素
template<typename T>
T& Vector<T>::front() {
    if (empty()) {
        throw std::runtime_error("Vector is empty");
    }
    return data[0];
}

template<typename T>
const T& Vector<T>::front() const {
    if (empty()) {
        throw std::runtime_error("Vector is empty");
    }
    return data[0];
}

// 访问最后一个元素
template<typename T>
T& Vector<T>::back() {
    if (empty()) {
        throw std::runtime_error("Vector is empty");
    }
    return data[size_ - 1];
}

template<typename T>
const T& Vector<T>::back() const {
    if (empty()) {
        throw std::runtime_error("Vector is empty");
    }
    return data[size_ - 1];
}

// 在末尾添加元素
template<typename T>
void Vector<T>::push_back(const T& value) {
    if (size_ >= capacity_) {
        resize();
    }
    data[size_++] = value;
}

// 删除末尾元素
template<typename T>
void Vector<T>::pop_back() {
    if (empty()) {
        throw std::runtime_error("Vector is empty");
    }
    --size_;
}

// 在指定位置插入元素
template<typename T>
void Vector<T>::insert(size_t index, const T& value) {
    if (index > size_) {
        throw std::out_of_range("Index out of range");
    }
    
    if (size_ >= capacity_) {
        resize();
    }
    
    // 向后移动元素
    for (size_t i = size_; i > index; --i) {
        data[i] = data[i - 1];
    }
    
    data[index] = value;
    ++size_;
}

// 删除指定位置的元素
template<typename T>
void Vector<T>::erase(size_t index) {
    check_bounds(index);
    
    // 向前移动元素
    for (size_t i = index; i < size_ - 1; ++i) {
        data[i] = data[i + 1];
    }
    
    --size_;
}

// 清空所有元素
template<typename T>
void Vector<T>::clear() {
    size_ = 0;
}

// 查找元素
template<typename T>
int Vector<T>::find(const T& value) const {
    for (size_t i = 0; i < size_; ++i) {
        if (data[i] == value) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// 检查是否包含元素
template<typename T>
bool Vector<T>::contains(const T& value) const {
    return find(value) != -1;
}

// 打印所有元素
template<typename T>
void Vector<T>::print() const {
    std::cout << "Vector[" << size_ << "/" << capacity_ << "]: [";
    for (size_t i = 0; i < size_; ++i) {
        std::cout << data[i];
        if (i < size_ - 1) {
            std::cout << ", ";
        }
    }
    std::cout << "]" << std::endl;
}

// 反转数组
template<typename T>
void Vector<T>::reverse() {
    for (size_t i = 0; i < size_ / 2; ++i) {
        std::swap(data[i], data[size_ - 1 - i]);
    }
}

// 排序数组
template<typename T>
void Vector<T>::sort() {
    std::sort(data, data + size_);
}

#endif // VECTOR_CPP