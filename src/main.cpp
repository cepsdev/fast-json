#include "fast-json.hpp"
#include <iostream>

using namespace std;

int main(int argc, char** argv){
    Arena<1> arena_allocator;
    fast_json<Arena<1>> jsn;
    string json = "[1,2,3,4,5,6]";
    auto r {jsn.read(json,&arena_allocator,0)};
    if(r){
        double sum{};
        traverse_binary_representation(r->first, r->second.second, [&sum](msg_node*m, bool enter){
            if (m->what== msg_node::F64) sum += ((msg_node_f64*)m)->value;
        });
        cout << " sum = " << sum << '\n';
    } else {
        cerr << "Failed to read JSON.\n";
        return 1;
    }
    return 0;
}