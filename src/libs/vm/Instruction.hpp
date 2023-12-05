#pragma once

#include "./Opcodes.hpp"


class VirtualMachine;


class BytecodeInstruction
{
public:

    BytecodeInstruction() { }
    virtual void execute(VirtualMachine &) = 0;


};

class Int_Load : public BytecodeInstruction
{
public:
    Int_Load(int value , int dest_reg) : value(value)
    , dest_reg(dest_reg) {}

    void execute(VirtualMachine &) override;


    int dest_reg;
    int value;
};


class ADD : public BytecodeInstruction
{
public:

    ADD(int dest , int src1 , int src2) :
     dest(dest) , src1(src1) , src2(src2) {}

    void execute(VirtualMachine &) override;

    int dest;
    int src1;
    int src2;

    int value;
};

class SUB : public BytecodeInstruction
{
public:

    SUB(int dest , int src1 , int src2) :
     dest(dest) , src1(src1) , src2(src2) {}

    void execute(VirtualMachine &) override;

    int dest;
    
    int src1;
    int src2;

    int value;
};
