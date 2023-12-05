#include "./Instruction.hpp"
#include "./Vm.hpp"

void Int_Load::execute(VirtualMachine &VM)
{
   VM.registers[dest_reg] = value;
}

void ADD::execute(VirtualMachine &VM)
{
   VM.registers[dest] =  VM.registers[src1] + VM.registers[src2];

   std::cout << "Sum value: " <<  VM.registers[dest] << std::endl;
}
