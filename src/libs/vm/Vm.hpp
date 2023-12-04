#pragma once

#include "./Opcodes.hpp"
#include "../value/primitives.hpp"
#include "./Instruction.hpp"
#include <stack>

class VirtualMachine
{
public:

  VirtualMachine() {}
  void execute( std::vector<BytecodeInstruction*>& instructions);

private:

    std::vector<int> registers;
};
