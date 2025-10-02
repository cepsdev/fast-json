# fast-json
C++ header-only library for processing of JSON documents, with focus on small memory footprint and performance.
## Build & run example
```$ ./build_and_run ```
## Example Usage


```c++
#include "fast-json.hpp"
#include <iostream>

using namespace std;

int main(int argc, char** argv){
    Arena<1> arena_allocator;        // fast-json comes with an arena allocator which supports reallocation,
                                    // the arena API is pretty simple which allows for a straight forward adoption
                                    // of user defined allocators.
    fast_json<Arena<1>> jsn;        
    string json = "[1,2,3,4,5,6]";
    auto r {jsn.read(json,&arena_allocator,0)};
    if(r){
        double sum{};
        // fast-json provides a traverse - function which takes a functor f.
        // The traversal occurs in document order, i.e. pre-order traversal of the binary representation
        // (root node, then children from left to right). If an inner node, i.e. array or object,
        // is visited the first time 'enter' is set to true, and false in the case the node is exited, i.e.
        // after all children have been visited.
        traverse_binary_representation(r->first, r->second.second, [&sum](msg_node*m, bool enter){
            if (m->what== msg_node::F64) sum += ((msg_node_f64*)m)->value; // all nodes derive from msg_node,
                    //dispatch according to the 'what' field, cast if necessary (see fast-json.hpp for details)
        });
        cout << " sum = " << sum << '\n';
    } else {
        cerr << "Failed to read JSON.\n";
        return 1;
    }
    return 0;
}
```
