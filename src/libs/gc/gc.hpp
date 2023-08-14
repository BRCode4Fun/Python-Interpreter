#pragma once

#include <unordered_map>
#include "../value/value.hpp"

class GarbageCollector { 

public:
    GarbageCollector() {};
    void Delete_objects(); 
    void Add_object(Value* value);
        
private:
    std::vector<Value*> objects; 
    unsigned int num_times_allocated = 0;
};
