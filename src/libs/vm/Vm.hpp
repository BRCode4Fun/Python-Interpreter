#pragma once

#include "./Opcodes.hpp"
#include "../value/primitives.hpp"
#include "./Instruction.hpp"
#include <stack>
#include <vector>

class BytecodeInstruction;

class VirtualMachine
{
public:

  VirtualMachine() {}

  void execute( std::vector<BytecodeInstruction*>& instructions);
  std::vector<int> registers;
};
