# KISS and DRY Principles for C++

This guide outlines how to apply KISS (Keep It Simple, Stupid) and DRY (Don't Repeat Yourself) principles in the Shorelark C++ project.

## KISS Principle - Keep It Simple, Stupid

The KISS principle advocates for simplicity over unnecessary complexity. Simpler solutions are generally more maintainable, have fewer bugs, and are easier to understand.

### Guidelines

1. **Write Clear, Self-Documenting Code**

```cpp
// Bad: Overly complex
int process(int x) {
    return ((x & 0x01) == 0) ? (x / 2) : (3 * x + 1);
}

// Good: Clear and simple
bool isEven(int x) {
    return x % 2 == 0;
}

int collatzNextValue(int x) {
    if (isEven(x)) {
        return x / 2;
    } else {
        return 3 * x + 1;
    }
}
```

2. **Favor Standard Library Over Custom Solutions**

```cpp
// Bad: Custom implementation
template<typename T>
class MyList {
    // Complex custom list implementation
};

// Good: Use the standard library
std::vector<int> numbers = {1, 2, 3, 4, 5};
std::list<std::string> names = {"Alice", "Bob", "Charlie"};
```

3. **Avoid Premature Optimization**

```cpp
// Bad: Premature optimization
template<size_t N>
void processBatch(const std::array<int, N>& data) {
    // Overly complex bit manipulation and SIMD optimizations
    // before understanding if this is a bottleneck
}

// Good: Start with clear, maintainable code
void processBatch(const std::vector<int>& data) {
    for (const auto& value : data) {
        processValue(value);
    }
}
```

4. **Use Descriptive Names**

```cpp
// Bad: Cryptic names
int calc(int a, int b) {
    return a > b ? a : b;
}

// Good: Self-documenting names
int maximum(int first, int second) {
    return first > second ? first : second;
}
```

5. **Avoid Deep Nesting**

```cpp
// Bad: Deep nesting
void process(const std::vector<int>& values) {
    if (!values.empty()) {
        for (const auto& value : values) {
            if (value > 0) {
                if (value % 2 == 0) {
                    // Process even positive values
                } else {
                    // Process odd positive values
                }
            }
        }
    }
}

// Good: Early returns and function extraction
void processEvenValue(int value) {
    // Process even positive value
}

void processOddValue(int value) {
    // Process odd positive value
}

void process(const std::vector<int>& values) {
    if (values.empty()) {
        return;
    }
    
    for (const auto& value : values) {
        if (value <= 0) {
            continue;
        }
        
        if (value % 2 == 0) {
            processEvenValue(value);
        } else {
            processOddValue(value);
        }
    }
}
```

## DRY Principle - Don't Repeat Yourself

The DRY principle aims to reduce repetition of patterns and code duplication.

### Guidelines

1. **Extract Common Code Into Functions**

```cpp
// Bad: Duplicated code
void processStudent(const Student& student) {
    std::cout << "Processing " << student.firstName << " " << student.lastName << std::endl;
    // Process student
}

void processTeacher(const Teacher& teacher) {
    std::cout << "Processing " << teacher.firstName << " " << teacher.lastName << std::endl;
    // Process teacher
}

// Good: Extract common code
template<typename Person>
void logProcessing(const Person& person) {
    std::cout << "Processing " << person.firstName << " " << person.lastName << std::endl;
}

void processStudent(const Student& student) {
    logProcessing(student);
    // Process student
}

void processTeacher(const Teacher& teacher) {
    logProcessing(teacher);
    // Process teacher
}
```

2. **Use Templates for Type-Generic Code**

```cpp
// Bad: Repeated functions for different types
int findMaxInt(const std::vector<int>& values);
double findMaxDouble(const std::vector<double>& values);
std::string findMaxString(const std::vector<std::string>& values);

// Good: Generic template
template<typename T>
T findMax(const std::vector<T>& values) {
    if (values.empty()) {
        throw std::invalid_argument("Cannot find max in empty collection");
    }
    
    return *std::max_element(values.begin(), values.end());
}
```

3. **Create Base Classes for Common Functionality**

```cpp
// Bad: Duplicate code in derived classes
class Circle {
public:
    void render() {
        prepareCanvas();
        // Render circle
        cleanupCanvas();
    }
private:
    void prepareCanvas() { /* ... */ }
    void cleanupCanvas() { /* ... */ }
};

class Rectangle {
public:
    void render() {
        prepareCanvas();
        // Render rectangle
        cleanupCanvas();
    }
private:
    void prepareCanvas() { /* ... */ }
    void cleanupCanvas() { /* ... */ }
};

// Good: Base class with common functionality
class Shape {
public:
    void render() {
        prepareCanvas();
        renderShape();
        cleanupCanvas();
    }
    virtual ~Shape() = default;
protected:
    virtual void renderShape() = 0;
private:
    void prepareCanvas() { /* ... */ }
    void cleanupCanvas() { /* ... */ }
};

class Circle : public Shape {
protected:
    void renderShape() override {
        // Render circle
    }
};

class Rectangle : public Shape {
protected:
    void renderShape() override {
        // Render rectangle
    }
};
```

4. **Use Constants and Configuration**

```cpp
// Bad: Magic numbers and repeated values
void process() {
    connectToDatabase("localhost", 3306, "username", "password");
    // Later in code...
    auto connection = DBConnection("localhost", 3306, "username", "password");
}

// Good: Define constants or configuration
struct DatabaseConfig {
    std::string host = "localhost";
    int port = 3306;
    std::string username = "username";
    std::string password = "password";
};

const DatabaseConfig DB_CONFIG;

void process() {
    connectToDatabase(DB_CONFIG.host, DB_CONFIG.port, 
                      DB_CONFIG.username, DB_CONFIG.password);
    // Later in code...
    auto connection = DBConnection(DB_CONFIG.host, DB_CONFIG.port, 
                                  DB_CONFIG.username, DB_CONFIG.password);
}
```

5. **Use Libraries and Components**

Don't reinvent the wheel. Use established libraries for common tasks:
- Use standard algorithms from the STL
- Use spdlog for logging
- Use Catch2 for testing
- Use range-v3 for range operations
- Use tl-expected for error handling

## Combining KISS and DRY in Shorelark C++

- Start with simple, clear implementations
- Extract common patterns only when they appear multiple times
- Don't over-abstract; wait until a pattern emerges
- Use existing libraries for common tasks
- Refactor to improve code clarity and remove duplication
