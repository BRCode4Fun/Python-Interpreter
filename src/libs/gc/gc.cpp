#include "gc.hpp"
#include <algorithm>

void GarbageCollector::freeUnused() {    

    for(auto object : objects) {
                        
        if(object->getRc() == 0) {

            std::vector<PyObject*>::iterator position = std::find(objects.begin(), objects.end(), object);
                    
            if(position != objects.end()){
                objects.erase(position);
                delete object;
            }
        }
    }
}

void GarbageCollector::pushObject(PyObject* value) {

    if(nAllocs >= 50){
        freeUnused();
        nAllocs = 0;
    }
    nAllocs++;
    objects.push_back(value); 

    return;
}
