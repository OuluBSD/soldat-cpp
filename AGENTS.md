# AGENTS

## Scope
- Applies to this STL C++ project and its subdirectories

## Overview
This is a standard C++ project using the STL (Standard Template Library).

## Build System
- **Type**: cmake
- **Build file**: CMakeLists.txt
- **Build commands**:
  ```bash
  mkdir -p build && cd build
  cmake ..
  make -j$(nproc)
  ```

## Code Conventions

### Header Files
- Use `#pragma once` or include guards:
  ```cpp
  #ifndef PROJECT_HEADER_H
  #define PROJECT_HEADER_H
  // content
  #endif
  ```

### Standard Library Usage
This project uses STL (Standard Template Library):
- `std::string` for strings
- `std::vector<T>`, `std::array<T>` for containers
- `std::map<K,V>`, `std::unordered_map<K,V>` for associative containers
- `std::unique_ptr<T>`, `std::shared_ptr<T>` for smart pointers
- `std::optional<T>` for optional values (C++17)

### Modern C++ Features
Use modern C++ features when appropriate:
- Range-based for loops: `for (const auto& item : container)`
- Auto type deduction: `auto value = someFunction();`
- Smart pointers instead of raw `new`/`delete`
- RAII (Resource Acquisition Is Initialization)
- `nullptr` instead of `NULL`
- `constexpr` for compile-time constants

### Memory Management
- Prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Avoid raw `new`/`delete` - use RAII containers
- Use `std::make_unique` and `std::make_shared`

### Naming Conventions
Follow project-specific naming conventions (common styles):
- **Classes/Structs**: `PascalCase` or `ClassName`
- **Functions/Methods**: `camelCase` or `snake_case`
- **Variables**: `camelCase` or `snake_case`
- **Constants**: `UPPER_SNAKE_CASE` or `kConstantName`
- **Namespaces**: `lowercase` or `snake_case`

### File Organization
- Header files: `.h` or `.hpp`
- Implementation files: `.cpp` or `.cc`
- Inline implementations: `.inl` (if used)

## Common Patterns

### Iterator Usage
```cpp
// Range-based (preferred)
for (const auto& item : container) {
    // use item
}

// Traditional iterator
for (auto it = container.begin(); it != container.end(); ++it) {
    // use *it
}
```

### String Operations
```cpp
std::string str = "hello world";
auto pos = str.find("world");
if (pos != std::string::npos) {
    // found
}
std::string sub = str.substr(0, 5);
```

### Container Operations
```cpp
std::vector<int> vec;
vec.push_back(42);
vec.emplace_back(43);  // construct in-place

std::map<std::string, int> map;
map["key"] = value;
auto it = map.find("key");
if (it != map.end()) {
    // found
}
```

## Building and Testing

### Debug Build
- Enable debug symbols: `-g`
- Disable optimization: `-O0`
- Enable sanitizers: `-fsanitize=address,undefined`

### Release Build
- Enable optimization: `-O2` or `-O3`
- Strip symbols: `-s`
- Link-time optimization: `-flto`

### Common Compiler Flags
- `-Wall -Wextra` - Enable warnings
- `-Werror` - Treat warnings as errors
- `-std=c++17` or `-std=c++20` - C++ standard version
- `-pthread` - Threading support

## Dependencies

Check the following files for dependencies:
- CMakeLists.txt - CMake dependencies
- Makefile - Make dependencies
- README.md - Manual dependency list
- requirements.txt or similar

## Documentation

- Code comments should explain "why", not "what"
- Use Doxygen-style comments for API documentation
- Keep README.md updated with build instructions

## Testing

If the project has tests:
- Unit tests are usually in `test/` or `tests/` directory
- Run tests after building to ensure correctness
- Add tests for new features

## Workflow and Project Organization

See `agents/common-workflow.md` for complete workflow patterns.

### Task Tracking
- Use `TASKS.md` for task tracking with sections: TODO, IN_PROGRESS, DONE
- Tasks belong to phases (Phase 1, Phase 2, etc.)
- Update TASKS.md after every completed task
- **CRITICAL**: When you discover new tasks during work, ADD them to TASKS.md immediately
  - Don't wait - document new tasks as soon as you realize they're needed
  - Examples: "Need password reset", "Found security issue", "Missing tests"
  - Add to appropriate phase in TODO section

### Git Workflow - ALWAYS commit after successful completion
- BEFORE committing: Run build and test scripts if they exist
- Commit format: `Task X.Y: Description`
- ALWAYS push after commit

### Documentation: Use docs/ with PlantUML (.puml → .png)

### Roadmap: Use roadmap/ directory (v1.0.0.md, etc.)

### Pseudocode: Use pseudocode/ directory before implementation

## References

- [C++ Reference](https://en.cppreference.com/)
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)
- Project-specific documentation in README.md

