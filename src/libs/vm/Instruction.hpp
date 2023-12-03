#pragma once

#include "./Opcodes.hpp"
#include "../value/PyObject.h"
#include <vector>


class BytecodeInstruction
{
public:
    BytecodeInstruction(OpCode op, std::vector<PyObject> operands) : opCode(op), operand(opd) {}

    OpCode getOpCode() const { return opCode; }
    PyObject getOperands() const { return operands; }

private:
    OpCode opCode;
    std::vector<PyObject> operands;
};
