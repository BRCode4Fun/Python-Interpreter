#pragma once

#include "./Opcodes.hpp"


class VirtualMachine;


class BytecodeInstruction
{
public:
    BytecodeInstruction(OpCode op) : opCode(op) { }

    OpCode getOpCode() const { return opCode; }
    virtual void execute(VirtualMachine &) = 0;

    OpCode opCode;
};



class Int_Load : public BytecodeInstruction
{
public:
    Int_Load(OpCode op , int value , int dest_reg) : BytecodeInstruction(op), value(value)
    , dest_reg(dest_reg) {}

    void execute(VirtualMachine &) override;

    OpCode getOpCode() const { return opCode; }


    int dest_reg;
    int value;
};


class ADD : public BytecodeInstruction
{
public:

    ADD(OpCode op , int dest , int src1 , int src2) : BytecodeInstruction(op)
    , dest(dest) , src1(src1) , src2(src2) {}

    void execute(VirtualMachine &) override;

    OpCode getOpCode() const { return opCode; }


    int dest;
    int src1;
    int src2;

    int value;
};
