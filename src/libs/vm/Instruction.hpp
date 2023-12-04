#pragma once

#include "./Opcodes.hpp"
#include <vector>


class BytecodeInstruction
{
public:
    BytecodeInstruction(OpCode op, std::vector<int> operands) : opCode(op), operands(operands) {}

    OpCode getOpCode() const { return opCode; }
    std::vector<int>  getOperands() const { return operands; }


    OpCode opCode;
    std::vector<int> operands;
};
