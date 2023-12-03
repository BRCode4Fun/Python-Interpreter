#pragma once

#include "./Opcodes.hpp"
#include "../value/PyObject.h"
#include <stack>

class VirtualMachine
{
public:

  VirtualMachine() {};

  void execute(const Bytecode& bytecode);
private:
    std::stack<Value> stack;
    std::unordered_map<std::string, PyObject> variables;
}
