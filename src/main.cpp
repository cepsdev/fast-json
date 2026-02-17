#include <iostream>
#include "fast-json.hpp"


int main(int argc, char** argv){
    Arena<1> arena_allocator;
    { // Full traversal of a json tree
     fast_json<Arena<1>> jsn;
     std::string json = "[1,2,3,4,5,6]";
     auto r {jsn.read(json,&arena_allocator,0)};
     if(r.has_value()){
        double sum{};
        traverse_binary_representation(r->first, r->second.second, [&sum](msg_node*m, bool enter){
            if (m->what== msg_node::F64) sum += ((msg_node_f64*)m)->value;
        });
        std::cout << " sum = " << sum << '\n';
     } else {
        std::cerr << "Failed to read JSON.\n";
        return 1;
     }
    }
    {
     // Access via the index operator []
     std::string json = R"({
      "pi":3.141592655359,
      "jsonrpc":"2.0",
      
      "id":0,"method":"initialize",
      "params":
       {
        "protocolVersion":"2025-11-25",
        "capabilities":{
          "sampling":{},
          "elicitation":{},
          "roots":{"listChanged":true},
          "tasks":{
            "list":{},
            "cancel":{},
            "requests":{
            "sampling":{"createMessage":{}},
            "elicitation":{"create":{}}
           }
          }
        },
       "clientInfo":{
         "name":"inspector-client",
         "version":"0.20.0"
        }
       }
      })";
     fast_json<Arena<1>> jsn{json, arena_allocator, 0}; 
     if (!jsn.valid()){
        std::cerr << "Failed to read >>"+json+"<<.\n";
        return 2;
     }
     std::string jsonrpc = jsn["jsonrpc"];
     std::string method = jsn["method"];
     double pi = jsn["pi"];
     std::string params_protocolVersion = jsn["params"]["protocolVersion"];
     std::cerr << "jsonrpc=" << jsonrpc <<" "<<"method="<< method << " pi=" << pi << " params.protocolVersion=" << params_protocolVersion << "\n"; 
    }
    return 0;
}