#include "scope.hpp"

Scope::Scope() : parent(nullptr)
{}

Scope::Scope(Scope* parent)
{
 //   assert(parent != nullptr);
}

void Scope::Decrement_Reference(Value *Value){

    if(Value != nullptr)
        Value->Decrement_Reference_counting();
}

void Scope::Increment_Reference(Value *Value){

    if(Value != nullptr)
        Value->Increment_Reference_counting(); 
        
}

void Scope::define(const string& name,  Value * value)
{
    auto it = values.find(name);
   
    if (it != values.end()) {

     Decrement_Reference(it->second);

     it->second = value;

     Increment_Reference(it->second);

    }else 
    {

     values.emplace(name, value);
     Increment_Reference(value);
  
    }
}



Value * Scope::get(string name) 
{

    auto it = values.find(name);
   
    if (it != values.end()) {

        return it->second;
    }

    throw  runtime_error("Undeclared variable '" + name + "'");
}
