# fast-json
JSON Lib with focus on small memory footprint and performance
## Build & run example
```$ ./build_and_run ```
# Github Copilot's take:

## What fast-json Does

- **Parses JSON strings** into a binary format optimized for traversal and low memory usage.
- **Serializes JSON data** using a structure inspired by protocol buffers.
- **Allocates memory** using a user-supplied arena allocator, reducing fragmentation and improving speed.
- **Provides traversal utilities** for iterating over the binary JSON representation.

---

## Example Usage

Below is a conceptual example based on the code in `include/fast-json.hpp`:

```c++
// Suppose you have an arena allocator called MyArenaAllocator.

#include "fast-json.hpp"

using fast_json_t = fast_json<MyArenaAllocator>;

// Your JSON string
std::string json = R"({"name": "Alice", "age": 30, "is_student": false})";

// Prepare the arena allocator
MyArenaAllocator arena;

// Parse the JSON
auto result = fast_json_t::read(json, &arena, /*arena_id=*/0);

if (result) {
    char* binary = result->first; // pointer to binary representation
    size_t total_size = result->second.first;
    size_t used_size = result->second.second;

    // Traverse the binary representation
    fast_json_t::traverse_binary_representation(binary, used_size, [](auto* node, bool enter) {
        // Process each node (object, array, string, number, etc.)
        // Example: print node type or content here
    });
} else {
    // Handle parse error
}
```

---

## Traversal Example

You can traverse the binary JSON representation and process each element:

```c++
fast_json_t::traverse_binary_representation(binary, used_size, [](auto* node, bool enter) {
    if (node->what == fast_json_t::msg_node::NODE && enter) {
        std::cout << "Start object" << std::endl;
    }
    if (node->what == fast_json_t::msg_node::SZ) {
        // Print string value (you'd extract it from the node)
    }
    // Handle other types: INT32, F64, BOOLEAN, ARRAY, etc.
});
```

---

## Key Features

- **Header-only**: Easy to include in any project.
- **Low overhead**: Designed for performance and minimal memory usage.
- **Custom binary format**: Enables fast access and traversal.

---

## Typical Use Cases

- High-performance JSON parsing in resource-constrained environments.
- Scenarios where repeated parsing and traversal of large JSON data is needed.
- Systems requiring efficient memory management (with custom allocators).

---

**Note:**  
As of now, the library is in an early stage, so you may need to adapt the code and provide your own arena allocator. The API is subject to change as the project matures.

If you want a more concrete example or demo program, let me know!
