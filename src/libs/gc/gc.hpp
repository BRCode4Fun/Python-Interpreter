#pragma once

#include "../value/pyObject.hpp"

class GarbageCollector { 

public:
    GarbageCollector() {};
    void freeUnused(); 
    void pushObject(PyObject* value);
        
private:
    std::vector<PyObject*> objects; 
    unsigned int nAllocs = 0;
};
