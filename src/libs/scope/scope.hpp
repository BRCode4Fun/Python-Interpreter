#pragma once

#include <unordered_map>
#include "../value/value.hpp"

class Scope {

public:
    Scope();
    Scope(Scope* parent);

    void define(const std::string& name, Value* value);
    void Decrement_Reference(Value* value);
    void Increment_Reference(Value* value);

    Value* get(const std::string name);

private:
    Scope* parent = nullptr;
    std::unordered_map<std::string, Value*> values;
};
