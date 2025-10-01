# Testing with Catch2

This guide outlines best practices for testing with Catch2 in the Shorelark C++ project.

## Setting Up Catch2

Catch2 is included in our project as a dependency via Conan. The CMake integration is already set up as seen in the main CMakeLists.txt:

```cmake
find_package(Catch2 REQUIRED)
include(Catch)
```

## Test File Structure

Each library or application component should have a corresponding test directory with test files following the naming convention `*_test.cc`.

Example structure:
```
libs/
  neural_network/
    src/
      neuron.cc
      layer.cc
    test/
      neuron_test.cc
      layer_test.cc
```

## Writing Tests with Catch2

### Basic Test Structure

```cpp
#include <catch2/catch_all.hpp>

TEST_CASE("My test case description", "[tag1][tag2]") {
    // Setup
    int result = 0;
    
    // Exercise
    result = functionToTest();
    
    // Verify
    REQUIRE(result == 42);
    
    // Teardown (automatic in Catch2)
}
```

### Use BDD Style When Appropriate

```cpp
SCENARIO("Neural network forward propagation", "[neural_network][propagation]") {
    GIVEN("A neural network with one hidden layer") {
        NeuralNetwork network(2, 3, 1);
        
        WHEN("Inputs are provided") {
            std::vector<double> inputs = {0.5, 0.8};
            auto outputs = network.propagate(inputs);
            
            THEN("Output values are within expected range") {
                REQUIRE(outputs.size() == 1);
                REQUIRE(outputs[0] >= 0.0);
                REQUIRE(outputs[0] <= 1.0);
            }
        }
    }
}
```

### Using Test Fixtures

```cpp
class NetworkFixture {
public:
    NetworkFixture() : network(2, 3, 1) {
        // Common setup
        inputs = {0.5, 0.8};
    }
    
    NeuralNetwork network;
    std::vector<double> inputs;
};

TEST_CASE_METHOD(NetworkFixture, "Neural network propagation with fixture", "[neural_network]") {
    auto outputs = network.propagate(inputs);
    
    REQUIRE(outputs.size() == 1);
    REQUIRE(outputs[0] >= 0.0);
    REQUIRE(outputs[0] <= 1.0);
}
```

### Testing Exceptions

```cpp
TEST_CASE("Test exception handling", "[exceptions]") {
    REQUIRE_THROWS_AS(divideByZero(), std::invalid_argument);
    REQUIRE_THROWS_WITH(divideByZero(), "Division by zero is not allowed");
}
```

### Parameterized Tests

```cpp
TEMPLATE_TEST_CASE("Vector operations with different numeric types", "[vector][template]", 
                    int, float, double) {
    std::vector<TestType> v = {
        static_cast<TestType>(1), 
        static_cast<TestType>(2), 
        static_cast<TestType>(3)
    };
    
    REQUIRE(sum(v) == static_cast<TestType>(6));
}

TEST_CASE("Test with dynamic values", "[parametrized]") {
    auto value = GENERATE(1, 2, 3, 4, 5);
    
    REQUIRE(isPositive(value));
}
```

### Using Sections

```cpp
TEST_CASE("Multiple test sections", "[sections]") {
    std::vector<int> v = {1, 2, 3};
    
    SECTION("Adding an element") {
        v.push_back(4);
        REQUIRE(v.size() == 4);
        REQUIRE(v.back() == 4);
    }
    
    SECTION("Removing an element") {
        v.pop_back();
        REQUIRE(v.size() == 2);
        REQUIRE(v.back() == 2);
    }
}
```

## Mocking with FakeIt

FakeIt is included as a dependency for mocking:

```cpp
#include <fakeit.hpp>
using namespace fakeit;

TEST_CASE("Test with mocks", "[mocking]") {
    // Create a mock
    Mock<IDatabase> mockDatabase;
    
    // Setup expectations
    When(Method(mockDatabase, getData)).Return(std::vector<int>{1, 2, 3});
    When(Method(mockDatabase, isConnected)).Return(true);
    
    // Get the instance
    IDatabase& db = mockDatabase.get();
    
    // Test code that uses the database
    DataProcessor processor(db);
    auto result = processor.process();
    
    // Verify results
    REQUIRE(result == 6);
    
    // Verify expectations
    Verify(Method(mockDatabase, getData)).Once();
}
```

## Test Organization

### Test Tags

Use tags to categorize tests:
- `[unit]` - Unit tests
- `[integration]` - Integration tests
- `[performance]` - Performance tests
- `[neural_network]`, `[genetic_algorithm]`, etc. - Component tags

### Running Specific Tests

```bash
# Run all tests
./test_executable

# Run tests with specific tags
./test_executable [neural_network]

# Run a specific test case
./test_executable "Neural network forward propagation"

# Run tests with tag expressions
./test_executable [neural_network]~[slow]  # Neural network tests that aren't slow
```

## Test-Driven Development (TDD)

Follow the TDD workflow:
1. Write a failing test first
2. Implement the minimal code to pass the test
3. Refactor while keeping tests green

## Integration with CMake

Our project uses Catch2's CMake integration to automatically discover and run tests:

```cmake
# In library CMakeLists.txt
add_executable(my_library_tests
    test/feature1_test.cc
    test/feature2_test.cc
)

target_link_libraries(my_library_tests
    PRIVATE
    my_library
    Catch2::Catch2WithMain
)

# Register with CTest
catch_discover_tests(my_library_tests)
```

## Code Coverage

Use code coverage tools to ensure tests cover critical code paths:

```bash
# Generate coverage
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..
make
make test
lcov --capture --directory . --output-file lcov.info
genhtml lcov.info --output-directory coverage
```

## Best Practices

1. **Keep Tests Fast**: Tests should execute quickly to encourage frequent runs
2. **Independent Tests**: Tests should not rely on each other
3. **Test Behavior, Not Implementation**: Focus on testing behavior, not implementation details
4. **Clear Test Names**: Use descriptive test names that indicate what's being tested
5. **One Assertion Per Test**: Prefer one logical assertion per test
6. **Avoid Test Logic**: Minimize control flow in tests
7. **Setup Common Test Data**: Use fixtures for common setup
8. **Test Edge Cases**: Test boundary conditions and error cases
9. **Keep Tests Simple**: Tests should be straightforward and easy to understand
