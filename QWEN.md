# QWEN - Simple Guide for C++ Project

**Project**: soldat-cpp
**Type**: Standard C++ with STL
**Build System**: cmake
**For**: Qwen AI - Simple, clear instructions

---

## STEP 1: Read AGENTS.md First!

**IMPORTANT**: Read the AGENTS.md file in this directory first!
It has all the detailed rules for this C++ project.

---

## STEP 2: Version Tracking

**CRITICAL**: This project was converted from a Pascal codebase. 

### Version Information
- The original Pascal code version is tracked in `VERSION.md`
- **IMPORTANT**: If you use a newer Pascal commit to update the C++ code, you MUST update `VERSION.md` with the new commit hash
- Always check `VERSION.md` to know which Pascal code version corresponds to this C++ conversion

### Old Pascal Conversion Reference
- The previous Pascal to C++ conversion can be found at: `~/Dev/CodexSandbox/soldat`
- **Note**: In the old conversion, some Pascal files were renamed to CapitalCase (e.g., `unit.pas` became `Unit.pas`)
- When looking for corresponding files from the old conversion, remember to check for both original and CapitalCase file names

---

## STEP 2: Understanding This Project

This is a **standard C++ project** using STL (Standard Template Library).

### What is STL?
- STL = Standard Template Library
- It's part of standard C++
- Provides containers, algorithms, and utilities

### Build System: cmake

This project uses **CMake** for building.

**How to build:**
```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
```

---

## STEP 3: Important C++ Rules

### Rule #1: Use STL Types

This project uses **standard C++ types**:

| Type | Example |
|------|---------|
| String | `std::string name = "hello";` |
| Vector | `std::vector<int> numbers;` |
| Map | `std::map<std::string, int> ages;` |
| Smart Pointer | `std::unique_ptr<MyClass> ptr;` |

### Rule #2: Include Headers Correctly

```cpp
// System headers first
#include <iostream>
#include <string>
#include <vector>

// Then project headers
#include "myheader.h"
```

### Rule #3: Use Smart Pointers

```cpp
// WRONG - Don't use raw pointers
MyClass* ptr = new MyClass();
// ... might forget to delete!

// RIGHT - Use smart pointers
auto ptr = std::make_unique<MyClass>();
// automatically cleaned up!
```

### Rule #4: Use Modern C++

```cpp
// OLD WAY (don't use):
for (int i = 0; i < vec.size(); i++) {
    std::cout << vec[i] << std::endl;
}

// NEW WAY (do use):
for (const auto& item : vec) {
    std::cout << item << std::endl;
}
```

---

## STEP 4: Common Operations

### Working with Strings
```cpp
std::string str = "hello world";

// Find substring
auto pos = str.find("world");
if (pos != std::string::npos) {
    // found!
}

// Get substring
std::string sub = str.substr(0, 5);  // "hello"

// Check if ends with
bool ends = (str.rfind("world") == str.length() - 5);
```

### Working with Vectors
```cpp
std::vector<int> numbers;

// Add items
numbers.push_back(1);
numbers.push_back(2);

// Iterate
for (const auto& num : numbers) {
    std::cout << num << std::endl;
}

// Check size
if (!numbers.empty()) {
    std::cout << "Size: " << numbers.size() << std::endl;
}
```

### Working with Maps
```cpp
std::map<std::string, int> ages;

// Add items
ages["Alice"] = 30;
ages["Bob"] = 25;

// Find item
auto it = ages.find("Alice");
if (it != ages.end()) {
    std::cout << "Alice is " << it->second << std::endl;
}

// Iterate
for (const auto& [name, age] : ages) {
    std::cout << name << " is " << age << std::endl;
}
```

---

## STEP 5: Building and Testing

### Debug Build
```bash
# Use -g for debugging symbols
# Use -O0 to disable optimization
g++ -g -O0 -Wall -Wextra myfile.cpp -o myprogram
```

### Release Build
```bash
# Use -O2 or -O3 for optimization
g++ -O2 -Wall -Wextra myfile.cpp -o myprogram
```

### Running Tests
```bash
# If project has tests
cd build
make test
# or
ctest
```

---

## STEP 6: Common Mistakes

### Mistake 1: Forgetting to Check Find Results
```cpp
// WRONG
auto pos = str.find("text");
char c = str[pos];  // CRASH if not found!

// RIGHT
auto pos = str.find("text");
if (pos != std::string::npos) {
    char c = str[pos];  // Safe!
}
```

### Mistake 2: Using Raw Pointers
```cpp
// WRONG
MyClass* obj = new MyClass();
// ... do stuff ...
// Might forget: delete obj;

// RIGHT
auto obj = std::make_unique<MyClass>();
// Automatically deleted!
```

### Mistake 3: Not Using const References
```cpp
// WRONG - copies the string!
void printString(std::string str) {
    std::cout << str << std::endl;
}

// RIGHT - no copy!
void printString(const std::string& str) {
    std::cout << str << std::endl;
}
```

---

## STEP 7: Quick Reference

### Include What You Use
```cpp
#include <iostream>   // std::cout, std::cin
#include <string>     // std::string
#include <vector>     // std::vector
#include <map>        // std::map
#include <memory>     // std::unique_ptr, std::shared_ptr
#include <algorithm>  // std::sort, std::find
```

### Modern C++ Features
- Use `auto` for type deduction
- Use `nullptr` instead of `NULL`
- Use range-based for loops
- Use smart pointers
- Use `constexpr` for compile-time constants

---

## Quick Checklist

Before you write code:
- [ ] Did I read AGENTS.md?
- [ ] Am I using std:: types (std::string, std::vector)?
- [ ] Am I using smart pointers instead of raw new/delete?
- [ ] Am I using modern C++ features (auto, range-for)?
- [ ] Did I check if find() returned a valid result?

**If NO to any: STOP and fix it!**

---

## STEP 8: Where to Get Help

1. **Read AGENTS.md** in this directory
2. **Read project README.md** for specific instructions
3. **Check C++ Reference**: https://en.cppreference.com/
4. **Look at existing code** in the project for examples

---

## Remember

**Three most important things:**

1. **Use STL types** (std::string, std::vector, std::map)
2. **Use smart pointers** (std::unique_ptr, std::shared_ptr)
3. **Use modern C++** (auto, range-for, nullptr)

---

**Read AGENTS.md for complete details!**
