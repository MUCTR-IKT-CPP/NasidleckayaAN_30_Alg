#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <iterator>

template <typename T>
class StackArray {
private:
    T* data_;
    size_t size_;
    size_t capacity_;

    void resize(size_t new_capacity) {
        T* new_data = new T[new_capacity];
        for (size_t i = 0; i < size_; ++i) {
            new_data[i] = data_[i];
        }
        delete[] data_;
        data_ = new_data;
        capacity_ = new_capacity;
    }

public:
    StackArray() : data_(nullptr), size_(0), capacity_(0) {}
    ~StackArray() { delete[] data_; }

    void push(const T& value) {
        if (size_ == capacity_) {
            resize(capacity_ == 0 ? 4 : capacity_ * 2);
        }
        data_[size_++] = value;
    }

    void pop() {
        if (size_ > 0) {
            --size_;
        }
    }

    T& top() {
        if (empty()) throw std::runtime_error("Stack is empty!");
        return data_[size_ - 1];
    }
    [[nodiscard]] const T& top() const { return data_[size_ - 1]; }

    [[nodiscard]] bool empty() const { return size_ == 0; }
    [[nodiscard]] size_t size() const { return size_; }

    class Iterator {
        T* ptr_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(T* ptr) : ptr_(ptr) {}

        reference operator*() { return *ptr_; }
        pointer operator->() { return ptr_; }
        Iterator& operator++() {
            --ptr_;
            return *this;
        }
        bool operator!=(const Iterator& other) const { return ptr_ != other.ptr_; }
    };

    Iterator begin() { return Iterator(data_ + size_ - 1); }
    Iterator end() { return Iterator(data_ - 1); }
};

template <typename T>
class StackList {
private:
    struct Node {
        T data;
        Node* next;
    };
    Node* head_{ nullptr };
    size_t size_{ 0 };

public:
    StackList() = default;
    ~StackList() {
        while (head_) {
            pop();
        }
    }

    void push(const T& value) {
        head_ = new Node{ value, head_ };
        ++size_;
    }

    void pop() {
        if (head_) {
            Node* tmp = head_;
            head_ = head_->next;
            delete tmp;
            --size_;
        }
    }

    T& top() {
        if (empty()) throw std::runtime_error("Stack is empty!");
        return head_->data;
    }
    [[nodiscard]] const T& top() const { return head_->data; }

    [[nodiscard]] bool empty() const { return head_ == nullptr; }
    [[nodiscard]] size_t size() const { return size_; }

    // Iterator
    class Iterator {
        Node* current_;

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit Iterator(Node* node) : current_(node) {}

        reference operator*() { return current_->data; }
        pointer operator->() { return &current_->data; }
        Iterator& operator++() {
            current_ = current_->next;
            return *this;
        }
        bool operator!=(const Iterator& other) const {
            return current_ != other.current_;
        }
    };

    Iterator begin() { return Iterator(head_); }
    Iterator end() { return Iterator(nullptr); }
};


struct Person {
    std::string surname{};
    std::string name{};
    std::string patronymic{};
    int birth_year{};
};

std::mt19937 mt(std::random_device{}());
template <typename Stack>
void numeric_test(Stack& stack) {
    std::uniform_int_distribution<int> dist(-1000, 1000);
    int sum = 0;
    int min = 1000;
    int max = -1000;

    for (int i = 0; i < 1000; ++i) {
        int number = dist(mt);
        stack.push(number);
        sum += number;
        min = std::min(min, number);
        max = std::max(max, number);
    }

    std::cout << "Sum: " << sum
        << " Avg: " << sum / 1000.0
        << " Min: " << min
        << " Max: " << max << "\n";
}

template <typename StackType>
void string_test(StackType& stack) {
    std::vector<std::string> test_strings{ "one", "two", "three", "four", "five",
                                          "six", "seven", "eight", "nine", "ten" };

    for (const auto& str : test_strings) {
        stack.push(str);
    }

    while (!stack.empty()) {
        std::cout << stack.top() << ' ';
        stack.pop();
    }
    std::cout << "\n";
}

template <typename StackType>
void person_test(StackType& stack) {
    std::mt19937 rnd(std::random_device{}());
    std::vector<std::string> names{ "Иванов", "Иван", "Иванович" };
    std::vector<std::string> surnames{ "Петров", "Пётр", "Петрович" };
    std::vector<std::string> patronymics{ "Александров", "Александр", "Александрович" };
    std::uniform_int_distribution<> birth_dist(1980, 2020);

    for (int i = 0; i < 100; ++i) {
        Person p{
                surnames[rnd() % surnames.size()],
                names[rnd() % names.size()],
                patronymics[rnd() % names.size()],
                birth_dist(rnd)
        };
        stack.push(p);
    }

    StackList<Person> younger20;
    StackList<Person> older30;

    int current_year = 2025;
    for (auto& p : stack) {
        int age = current_year - p.birth_year;
        if (age < 20) younger20.push(p);
        else if (age > 30) older30.push(p);
    }
    std::cout << "People younger than 20: " << younger20.size()
        << ", older than 30: " << older30.size() << ", between 20 and 30 " << stack.size() - younger20.size() - older30.size() << "\n";
}

template <typename StackType>
void invert_test(StackType& stack) {
    for (int i = 0; i < 10; ++i)
        stack.push(i + 1);

    StackType reversed;
    while (!stack.empty()) {
        reversed.push(stack.top());
        stack.pop();
    }
    for (auto& el : reversed)
        std::cout << el << " ";
    std::cout << "\n";
}

template <typename StackT>
void benchmark(const std::string& test_name) {
    StackT stack;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < 10'000; ++i)
        stack.push(i);
    while (!stack.empty())
        stack.pop();
    auto end = std::chrono::steady_clock::now();
    std::cout << test_name << " elapsed: "
        << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
        << " ns\n";
}

int main() {
    std::cout << "--- Numeric Test with StackList ---\n";
    StackList<int> numeric_stack_list;
    numeric_test(numeric_stack_list);

    std::cout << "--- Numeric Test with StackArray ---\n";
    StackArray<int> numeric_stack_array;
    numeric_test(numeric_stack_array);

    std::cout << "--- String Test with StackList ---\n";
    StackList<std::string> string_stack_list;
    string_test(string_stack_list);

    std::cout << "--- String Test with StackArray ---\n";
    StackArray<std::string> string_stack_array;
    string_test(string_stack_array);

    std::cout << "--- Person Test with StackList ---\n";
    StackList<Person> person_stack_list;
    person_test(person_stack_list);

    std::cout << "--- Person Test with StackArray ---\n";
    StackArray<Person> person_stack_array;
    person_test(person_stack_array);

    std::cout << "--- Invert Test with StackList ---\n";
    StackList<int> invert_stack_list;
    invert_test(invert_stack_list);

    std::cout << "--- Invert Test with StackArray ---\n";
    StackArray<int> invert_stack_array;
    invert_test(invert_stack_array);

    std::cout << "--- Benchmark StackList ---\n";
    benchmark<StackList<int>>("StackList");

    std::cout << "--- Benchmark StackArray ---\n";
    benchmark<StackArray<int>>("StackArray");

    return 0;
}