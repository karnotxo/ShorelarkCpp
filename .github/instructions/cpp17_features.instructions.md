# Modern C++17 Features

This guide outlines recommended C++17 features to use in the Shorelark C++ project.

## Structured Bindings

Use structured bindings to improve code readability when accessing tuple-like objects.

```cpp
// Instead of:
auto pair = std::make_pair(1, "one");
auto first = pair.first;
auto second = pair.second;

// Prefer:
auto [number, text] = std::make_pair(1, "one");
```

## if constexpr

Use `if constexpr` for compile-time conditional code without SFINAE.

```cpp
template <typename T>
void process(const T& value) {
    if constexpr (std::is_integral_v<T>) {
        // Only compiled when T is an integral type
        std::cout << "Integer: " << value << std::endl;
    } else if constexpr (std::is_floating_point_v<T>) {
        // Only compiled when T is a floating-point type
        std::cout << "Float: " << std::fixed << value << std::endl;
    } else {
        // Only compiled for other types
        std::cout << "Other: " << value << std::endl;
    }
}
```

## Class Template Argument Deduction (CTAD)

Let the compiler deduce template arguments from constructors.

```cpp
// Instead of:
std::pair<int, double> p(1, 2.5);
std::vector<int> v = {1, 2, 3};

// Prefer:
std::pair p(1, 2.5);  // Deduces std::pair<int, double>
std::vector v = {1, 2, 3};  // Deduces std::vector<int>
```

## std::optional

Use `std::optional` for values that might or might not be present.

```cpp
std::optional<double> divide(double numerator, double denominator) {
    if (denominator == 0.0) {
        return std::nullopt;  // No value
    }
    return numerator / denominator;
}

// Usage
auto result = divide(10.0, 2.0);
if (result) {
    std::cout << "Result: " << *result << std::endl;
}
```

## std::variant

Use `std::variant` for type-safe unions.

```cpp
std::variant<int, double, std::string> v;
v = 42;  // v contains int
v = 3.14;  // v contains double
v = "hello";  // v contains string

// Visitor pattern
std::visit([](const auto& value) {
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, int>) {
        std::cout << "Integer: " << value << std::endl;
    } else if constexpr (std::is_same_v<T, double>) {
        std::cout << "Double: " << value << std::endl;
    } else if constexpr (std::is_same_v<T, std::string>) {
        std::cout << "String: " << value << std::endl;
    }
}, v);
```

## std::string_view

Use `std::string_view` for non-owning views of strings.

```cpp
// Instead of taking const std::string& for read-only string parameters:
void analyze(std::string_view text) {
    // Can work with string literals or std::string without copying
}
```

## constexpr if

Use `constexpr if` to conditionally compile code based on compile-time conditions.

```cpp
template <typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer_v<T>) {
        // Only compiled when T is a pointer type
        return *t;
    } else {
        // Only compiled when T is not a pointer type
        return t;
    }
}
```

## Inline Variables

Use inline variables for definitions in headers.

```cpp
// header.h
inline const int MAX_CONNECTIONS = 100;
inline constexpr double PI = 3.14159265358979323846;
```

## fold expressions

Use fold expressions for parameter pack operations.

```cpp
template<typename... Args>
auto sum(Args... args) {
    return (... + args);  // Unary left fold
}

// Usage
int total = sum(1, 2, 3, 4, 5);  // 15
```

## Nested Namespaces

Use nested namespace syntax.

```cpp
// Instead of:
namespace shorelark {
    namespace simulation {
        // Code
    }
}

// Prefer:
namespace shorelark::simulation {
    // Code
}
```

## Guide Summary

- Use structured bindings for multiple return values
- Use `if constexpr` for compile-time conditionals
- Take advantage of class template argument deduction (CTAD)
- Use `std::optional` for nullable values
- Use `std::variant` for type-safe unions
- Use `std::string_view` for string parameters
- Use inline variables for header-defined constants
- Use fold expressions for variadic templates
- Use nested namespace syntax
