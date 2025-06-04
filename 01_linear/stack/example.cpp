#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include "array_stack.h"
#include "linked_stack.h"

// 括号匹配检查
bool is_valid_parentheses(const std::string& s) {
    ArrayStack<char> stack;
    
    for (char c : s) {
        if (c == '(' || c == '[' || c == '{') {
            stack.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (stack.empty()) {
                return false;
            }
            
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
}

// 逆波兰表达式求值
int evaluate_rpn(const std::vector<std::string>& tokens) {
    ArrayStack<int> stack;
    
    for (const std::string& token : tokens) {
        if (token == "+" || token == "-" || token == "*" || token == "/") {
            if (stack.size() < 2) {
                throw std::runtime_error("Invalid expression");
            }
            
            int b = stack.top(); stack.pop();
            int a = stack.top(); stack.pop();
            
            int result;
            if (token == "+") result = a + b;
            else if (token == "-") result = a - b;
            else if (token == "*") result = a * b;
            else if (token == "/") result = a / b;
            
            stack.push(result);
        } else {
            stack.push(std::stoi(token));
        }
    }
    
    if (stack.size() != 1) {
        throw std::runtime_error("Invalid expression");
    }
    
    return stack.top();
}

// 中缀表达式转后缀表达式
std::string infix_to_postfix(const std::string& infix) {
    ArrayStack<char> stack;
    std::string postfix;
    
    auto precedence = [](char op) {
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;
    };
    
    for (char c : infix) {
        if (std::isspace(c)) continue;
        
        if (std::isalnum(c)) {
            postfix += c;
            postfix += ' ';
        } else if (c == '(') {
            stack.push(c);
        } else if (c == ')') {
            while (!stack.empty() && stack.top() != '(') {
                postfix += stack.top();
                postfix += ' ';
                stack.pop();
            }
            if (!stack.empty()) stack.pop(); // 弹出 '('
        } else if (c == '+' || c == '-' || c == '*' || c == '/') {
            while (!stack.empty() && stack.top() != '(' &&
                   precedence(stack.top()) >= precedence(c)) {
                postfix += stack.top();
                postfix += ' ';
                stack.pop();
            }
            stack.push(c);
        }
    }
    
    while (!stack.empty()) {
        postfix += stack.top();
        postfix += ' ';
        stack.pop();
    }
    
    return postfix;
}

// 下一个更大元素
std::vector<int> next_greater_element(const std::vector<int>& nums) {
    std::vector<int> result(nums.size(), -1);
    ArrayStack<int> stack; // 存储索引
    
    for (int i = 0; i < nums.size(); ++i) {
        while (!stack.empty() && nums[stack.top()] < nums[i]) {
            result[stack.top()] = nums[i];
            stack.pop();
        }
        stack.push(i);
    }
    
    return result;
}

// 柱状图中最大的矩形
int largest_rectangle_area(const std::vector<int>& heights) {
    ArrayStack<int> stack; // 存储索引
    int max_area = 0;
    
    for (int i = 0; i <= heights.size(); ++i) {
        int h = (i == heights.size()) ? 0 : heights[i];
        
        while (!stack.empty() && heights[stack.top()] > h) {
            int height = heights[stack.top()];
            stack.pop();
            
            int width = stack.empty() ? i : i - stack.top() - 1;
            max_area = std::max(max_area, height * width);
        }
        
        stack.push(i);
    }
    
    return max_area;
}

void test_basic_operations() {
    std::cout << "=== 基本操作测试 ===" << std::endl;
    
    // 测试数组栈
    std::cout << "\n1. 数组栈测试:" << std::endl;
    ArrayStack<int> array_stack;
    
    for (int i = 1; i <= 5; ++i) {
        array_stack.push(i * 10);
        std::cout << "Push " << i * 10 << ": ";
        array_stack.print();
    }
    
    std::cout << "栈顶元素: " << array_stack.top() << std::endl;
    std::cout << "栈大小: " << array_stack.size() << std::endl;
    
    while (!array_stack.empty()) {
        std::cout << "Pop " << array_stack.top() << ": ";
        array_stack.pop();
        array_stack.print();
    }
    
    // 测试链表栈
    std::cout << "\n2. 链表栈测试:" << std::endl;
    LinkedStack<int> linked_stack;
    
    for (int i = 1; i <= 5; ++i) {
        linked_stack.push(i * 10);
        std::cout << "Push " << i * 10 << ": ";
        linked_stack.print();
    }
    
    while (!linked_stack.empty()) {
        std::cout << "Pop " << linked_stack.top() << ": ";
        linked_stack.pop();
        linked_stack.print();
    }
}

void test_min_stack() {
    std::cout << "\n=== 最小栈测试 ===" << std::endl;
    
    ArrayStackWithMin<int> min_stack;
    
    std::vector<int> values = {3, 5, 2, 1, 4};
    
    for (int val : values) {
        min_stack.push(val);
        std::cout << "Push " << val << ", 当前最小值: " << min_stack.get_min() << std::endl;
    }
    
    while (!min_stack.empty()) {
        std::cout << "Pop " << min_stack.top() << ", 当前最小值: ";
        min_stack.pop();
        if (!min_stack.empty()) {
            std::cout << min_stack.get_min();
        } else {
            std::cout << "(栈为空)";
        }
        std::cout << std::endl;
    }
}

void test_dual_stack() {
    std::cout << "\n=== 双栈测试 ===" << std::endl;
    
    DualArrayStack<int> dual_stack(10);
    
    // 向两个栈中添加元素
    for (int i = 1; i <= 3; ++i) {
        dual_stack.push1(i);
        dual_stack.push2(i * 10);
        std::cout << "添加元素后:" << std::endl;
        dual_stack.print();
    }
    
    // 从两个栈中弹出元素
    std::cout << "\n弹出元素:" << std::endl;
    while (!dual_stack.empty1() || !dual_stack.empty2()) {
        if (!dual_stack.empty1()) {
            std::cout << "从栈1弹出: " << dual_stack.top_1() << std::endl;
            dual_stack.pop1();
        }
        if (!dual_stack.empty2()) {
            std::cout << "从栈2弹出: " << dual_stack.top_2() << std::endl;
            dual_stack.pop2();
        }
        dual_stack.print();
    }
}

void test_queue_using_stacks() {
    std::cout << "\n=== 用栈实现队列测试 ===" << std::endl;
    
    QueueUsingTwoStacks<int> queue;
    
    // 入队操作
    for (int i = 1; i <= 5; ++i) {
        queue.enqueue(i * 10);
        std::cout << "入队 " << i * 10 << ":" << std::endl;
        queue.print();
    }
    
    // 出队操作
    while (!queue.empty()) {
        std::cout << "出队 " << queue.front() << ":" << std::endl;
        queue.dequeue();
        queue.print();
    }
}

void test_parentheses_matching() {
    std::cout << "\n=== 括号匹配测试 ===" << std::endl;
    
    std::vector<std::string> test_cases = {
        "()",
        "()[]{}",
        "(])",
        "([{}])",
        "(((",
        ")(",
        "{[()]}"
    };
    
    for (const std::string& test : test_cases) {
        bool valid = is_valid_parentheses(test);
        std::cout << "\"" << test << "\" -> " << (valid ? "有效" : "无效") << std::endl;
    }
}

void test_rpn_evaluation() {
    std::cout << "\n=== 逆波兰表达式求值测试 ===" << std::endl;
    
    std::vector<std::vector<std::string>> test_cases = {
        {"2", "1", "+", "3", "*"},
        {"4", "13", "5", "/", "+"},
        {"10", "6", "9", "3", "+", "-11", "*", "/", "*", "17", "+", "5", "+"}
    };
    
    for (const auto& tokens : test_cases) {
        std::cout << "表达式: ";
        for (const std::string& token : tokens) {
            std::cout << token << " ";
        }
        
        try {
            int result = evaluate_rpn(tokens);
            std::cout << "= " << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "错误: " << e.what() << std::endl;
        }
    }
}

void test_infix_to_postfix() {
    std::cout << "\n=== 中缀转后缀表达式测试 ===" << std::endl;
    
    std::vector<std::string> test_cases = {
        "a+b*c",
        "(a+b)*c",
        "a+b*c-d",
        "a*(b+c)/d"
    };
    
    for (const std::string& infix : test_cases) {
        std::string postfix = infix_to_postfix(infix);
        std::cout << "中缀: " << infix << " -> 后缀: " << postfix << std::endl;
    }
}

void test_next_greater_element() {
    std::cout << "\n=== 下一个更大元素测试 ===" << std::endl;
    
    std::vector<int> nums = {2, 1, 2, 4, 3, 1};
    std::vector<int> result = next_greater_element(nums);
    
    std::cout << "数组: ";
    for (int num : nums) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    std::cout << "下一个更大元素: ";
    for (int res : result) {
        std::cout << res << " ";
    }
    std::cout << std::endl;
}

void test_largest_rectangle() {
    std::cout << "\n=== 最大矩形面积测试 ===" << std::endl;
    
    std::vector<std::vector<int>> test_cases = {
        {2, 1, 5, 6, 2, 3},
        {2, 4},
        {1, 1, 1, 1},
        {6, 2, 5, 4, 5, 1, 6}
    };
    
    for (const auto& heights : test_cases) {
        std::cout << "柱状图高度: ";
        for (int h : heights) {
            std::cout << h << " ";
        }
        
        int max_area = largest_rectangle_area(heights);
        std::cout << "-> 最大矩形面积: " << max_area << std::endl;
    }
}

int main() {
    std::cout << "C++ 栈 (Stack) 完整示例" << std::endl;
    std::cout << "========================" << std::endl;
    
    test_basic_operations();
    test_min_stack();
    test_dual_stack();
    test_queue_using_stacks();
    test_parentheses_matching();
    test_rpn_evaluation();
    test_infix_to_postfix();
    test_next_greater_element();
    test_largest_rectangle();
    
    std::cout << "\n所有测试完成！" << std::endl;
    
    return 0;
}