#include "gc.hpp"

void GarbageCollector::Delete_objects()
{         
            for(auto object :  objects) {
                        
                if(object->Get_Reference_counting() == 0) {

                    std::vector<Value *>::iterator position = std::find(objects.begin(), objects.end(), object);
                    
                    if(position != objects.end()){
                           objects.erase(position);
                           delete object;
                         
                    }

                }
            }
}


void GarbageCollector::Add_object(Value * value)
{ 
             if(num_times_allocated >= 50){
                Delete_objects(); 
                num_times_allocated = 0; 
             }

             num_times_allocated++;
             objects.push_back(value); 

             return;
}