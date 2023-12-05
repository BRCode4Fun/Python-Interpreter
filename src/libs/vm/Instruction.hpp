#pragma once

#include "./Opcodes.hpp"


class VirtualMachine;


class BytecodeInstruction
{
public:
    BytecodeInstruction(OpCode op) : opCode(op) {}

    OpCode getOpCode() const { return opCode; }
    virtual void execute(VirtualMachine &) = 0;

    OpCode opCode;
};



class Int_Load : public BytecodeInstruction
{
public:
    Int_Load(OpCode op , int value) : BytecodeInstruction(op), value(value) {}

    void execute(VirtualMachine &) override;

    OpCode getOpCode() const { return opCode; }


    int dest_reg;
    int value;
};
