# Logging with spdlog

This guide outlines best practices for logging using spdlog in the Shorelark C++ project.

## Why spdlog?
- Header-only, fast, and modern C++ logging library
- Supports formatting, sinks, log levels, and more
- Used for both application and library logging

## Basic Usage

```cpp
#include <spdlog/spdlog.h>

int main() {
    spdlog::info("Welcome to Shorelark!");
    spdlog::warn("This is a warning!");
    spdlog::error("This is an error: {}", 42);
    return 0;
}
```

## Logger Initialization

- Use the default logger for simple cases
- For libraries, create named loggers

```cpp
// Create a named logger
auto logger = spdlog::stdout_color_mt("shorelark");
logger->info("Library initialized");
```

## Log Levels

Set log levels globally or per logger:

```cpp
spdlog::set_level(spdlog::level::debug); // Set global log level
logger->set_level(spdlog::level::info);  // Set per-logger level
```

## Formatting

Use fmt-style formatting:

```cpp
spdlog::info("Simulation step {}: value = {:.3f}", step, value);
```

## Sinks

Log to files, console, or both:

```cpp
auto file_logger = spdlog::basic_logger_mt("file_logger", "logs/output.log");
file_logger->info("Logging to a file");

// Multiple sinks
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

std::vector<spdlog::sink_ptr> sinks;
sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/shorelark.log", true));
auto combined_logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
spdlog::register_logger(combined_logger);
combined_logger->info("This message goes to both console and file");
```

## Thread Safety

spdlog is thread-safe by default for all loggers.

## Compile-Time Log Level

For performance, set the compile-time log level:

```cpp
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#include <spdlog/spdlog.h>
```

## Logging in Libraries

- Do not use `spdlog::set_default_logger` in libraries
- Create and register a named logger for each library
- Document logger names for users

## Logging Best Practices

1. Use appropriate log levels (`trace`, `debug`, `info`, `warn`, `error`, `critical`)
2. Avoid logging in tight loops unless at `trace` or `debug` level
3. Use structured messages for easier parsing
4. Prefer named loggers for libraries and subsystems
5. Do not log sensitive information
6. Use log rotation for file sinks in long-running applications

## Example: Library Logger

```cpp
namespace shorelark::simulation {
    std::shared_ptr<spdlog::logger> get_logger() {
        static auto logger = spdlog::stdout_color_mt("simulation");
        return logger;
    }
}

// Usage
shorelark::simulation::get_logger()->info("Simulation started");
```

## Integration with CMake and Conan

- spdlog is provided via Conan and found with `find_package(spdlog REQUIRED)`
- Link with `spdlog::spdlog`

```cmake
target_link_libraries(myapp PRIVATE spdlog::spdlog)
```

## Summary
- Use spdlog for all logging
- Prefer named loggers for libraries
- Set log levels appropriately
- Use sinks for flexible output
- Integrate with CMake and Conan as shown
