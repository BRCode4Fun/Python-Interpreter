#include "gc.hpp"
#include <algorithm>

void GarbageCollector::freeUnused() {    

    for(PyObject* object : objects) {
                        
        if(object->getRefCount() == 0) {

            std::vector<PyObject*>::iterator position = std::find(objects.begin(), objects.end(), object);
                    
            if(position != objects.end()){
                objects.erase(position);
                delete object;
            }
        }
    }
}

void GarbageCollector::pushObject(PyObject* value) {
    if(value == nullptr) {
        return;
    }
    if(std::find(objects.begin(), objects.end(), value) != objects.end()) {
        return;
    }

    nAllocs++;
    objects.push_back(value); 

    return;
}
