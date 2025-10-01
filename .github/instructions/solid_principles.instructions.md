# SOLID Principles for C++

This guide outlines how to apply SOLID design principles in the Shorelark C++ project.

## S - Single Responsibility Principle

Each class should have a single responsibility, a single reason to change.

```cpp
// Bad: Class has multiple responsibilities
class Animal {
public:
    void feed();
    void clean();
    void save(); // Persistence logic mixed with domain logic
};

// Good: Separate responsibilities
class Animal {
public:
    void feed();
    void clean();
};

class AnimalRepository {
public:
    void save(const Animal& animal);
    std::optional<Animal> findById(int id);
};
```

## O - Open/Closed Principle

Software entities should be open for extension but closed for modification.

```cpp
// Bad: Need to modify the class for new shapes
class AreaCalculator {
public:
    double calculateArea(const Shape* shape) {
        if (dynamic_cast<const Rectangle*>(shape)) {
            auto rect = dynamic_cast<const Rectangle*>(shape);
            return rect->width * rect->height;
        } else if (dynamic_cast<const Circle*>(shape)) {
            auto circle = dynamic_cast<const Circle*>(shape);
            return 3.14 * circle->radius * circle->radius;
        }
        return 0;
    }
};

// Good: Use polymorphism to extend without modifying
class Shape {
public:
    virtual double area() const = 0;
    virtual ~Shape() = default;
};

class Rectangle : public Shape {
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() const override { return width * height; }
private:
    double width, height;
};

class Circle : public Shape {
public:
    explicit Circle(double r) : radius(r) {}
    double area() const override { return 3.14 * radius * radius; }
private:
    double radius;
};

// Main code only depends on the Shape interface
void printArea(const Shape& shape) {
    std::cout << "Area: " << shape.area() << std::endl;
}
```

## L - Liskov Substitution Principle

Objects of a superclass should be replaceable with objects of its subclasses without breaking the application.

```cpp
class Bird {
public:
    virtual void fly() = 0;
    virtual ~Bird() = default;
};

// Bad: Violates LSP because Penguin can't fly
class Penguin : public Bird {
public:
    void fly() override {
        throw std::runtime_error("Penguins can't fly!");
    }
};

// Good: Restructure the hierarchy
class Animal {
public:
    virtual void move() = 0;
    virtual ~Animal() = default;
};

class FlyingBird : public Animal {
public:
    void move() override { fly(); }
    virtual void fly() = 0;
};

class SwimmingBird : public Animal {
public:
    void move() override { swim(); }
    virtual void swim() = 0;
};

class Sparrow : public FlyingBird {
public:
    void fly() override { std::cout << "Sparrow flies" << std::endl; }
};

class Penguin : public SwimmingBird {
public:
    void swim() override { std::cout << "Penguin swims" << std::endl; }
};
```

## I - Interface Segregation Principle

Clients should not be forced to depend on interfaces they do not use.

```cpp
// Bad: Clients must implement unnecessary methods
class Worker {
public:
    virtual void work() = 0;
    virtual void eat() = 0;
    virtual ~Worker() = default;
};

// Good: Segregate interfaces
class Workable {
public:
    virtual void work() = 0;
    virtual ~Workable() = default;
};

class Eatable {
public:
    virtual void eat() = 0;
    virtual ~Eatable() = default;
};

class Worker : public Workable, public Eatable {
public:
    void work() override { std::cout << "Working" << std::endl; }
    void eat() override { std::cout << "Eating during break" << std::endl; }
};

class Robot : public Workable {
public:
    void work() override { std::cout << "Robot working" << std::endl; }
    // No need to implement eat()
};
```

## D - Dependency Inversion Principle

High-level modules should not depend on low-level modules. Both should depend on abstractions.

```cpp
// Bad: High-level module depends on low-level module
class MySQLDatabase {
public:
    void insert(const std::string& data);
};

class UserService {
private:
    MySQLDatabase database;  // Direct dependency on concrete implementation

public:
    void addUser(const std::string& user) {
        database.insert(user);
    }
};

// Good: Depend on abstractions
class Database {
public:
    virtual void insert(const std::string& data) = 0;
    virtual ~Database() = default;
};

class MySQLDatabase : public Database {
public:
    void insert(const std::string& data) override;
};

class PostgreSQLDatabase : public Database {
public:
    void insert(const std::string& data) override;
};

class UserService {
private:
    std::unique_ptr<Database> database;  // Dependency on abstraction

public:
    UserService(std::unique_ptr<Database> db)
        : database(std::move(db)) {}

    void addUser(const std::string& user) {
        database->insert(user);
    }
};

// Usage
auto service = UserService(std::make_unique<MySQLDatabase>());
```

## Applying SOLID to Shorelark C++ Project

- Keep classes focused on a single responsibility
- Use interfaces and polymorphism to allow extension without modification
- Ensure subtypes can be used in place of their parent types
- Create smaller, focused interfaces rather than large, general ones
- Use dependency injection to depend on abstractions, not concrete types
- Consider using an IoC container for complex dependency graphs
