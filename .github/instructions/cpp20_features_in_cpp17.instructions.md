# C++20 Features in C++17 with Libraries

This guide explains how to use C++20-like features in a C++17 project using third-party libraries, as practiced in Shorelark C++.

## Motivation

C++20 introduced many useful features, but not all compilers or environments support it. We use libraries to backport some features to C++17.

## Libraries Used
- **range-v3**: Brings C++20 ranges and views
- **tl-expected**: Provides `std::expected`-like error handling
- **span-lite**: Provides `std::span`-like non-owning array views

---

## range-v3: C++20 Ranges and Views

### Setup
- Provided via Conan: `range-v3/0.12.0`
- Find with CMake: `find_package(range-v3 REQUIRED)`
- Link: `target_link_libraries(target PRIVATE range-v3::range-v3)`

### Usage Example
```cpp
#include <range/v3/all.hpp>

std::vector<int> v = {1, 2, 3, 4, 5};
// Filter and transform using views
auto result = v | ranges::views::filter([](int x) { return x % 2 == 0; })
                 | ranges::views::transform([](int x) { return x * x; });
for (int x : result) {
    std::cout << x << " "; // 4 16
}
```

---

## tl-expected: std::expected for Error Handling

### Setup
- Provided via Conan: `tl-expected/1.1.0`
- Find with CMake: `find_package(tl-expected REQUIRED)`
- Link: `target_link_libraries(target PRIVATE tl-expected::tl-expected)`

### Usage Example
```cpp
#include <tl/expected.hpp>

template<typename T>
tll::expected<T, std::string> parse_number(const std::string& s) {
    try {
        return std::stoi(s);
    } catch (...) {
        return tl::unexpected{"Invalid number"};
    }
}

auto result = parse_number<int>("42");
if (result) {
    std::cout << "Parsed: " << *result << std::endl;
} else {
    std::cerr << "Error: " << result.error() << std::endl;
}
```

---

## span-lite: std::span for Array Views

### Setup
- Provided via Conan: `span-lite/0.10.3`
- Find with CMake: `find_package(span-lite REQUIRED)`
- Link: `target_link_libraries(target PRIVATE span-lite::span-lite)`

### Usage Example
```cpp
#include <span.hpp>

void print(span<int> arr) {
    for (int x : arr) std::cout << x << ' ';
}

std::vector<int> v = {1, 2, 3};
print(span<int>(v));
```

---

## Best Practices
- Prefer these libraries for new code needing C++20 features
- Use `range-v3` for range-based algorithms and pipelines
- Use `tl-expected` for error handling instead of exceptions or `std::optional`
- Use `span-lite` for non-owning array views
- Integrate via Conan and CMake as shown

## Summary Table
| Feature         | C++20 Standard | Library in C++17   |
|-----------------|----------------|--------------------|
| Ranges/Views    | std::ranges    | range-v3           |
| std::expected   | std::expected  | tl-expected        |
| std::span       | std::span      | span-lite          |

These libraries allow you to write modern, expressive C++ code even when limited to C++17.
