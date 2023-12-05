#include "./Instruction.hpp"
#include "./Vm.hpp"

void Int_Load::execute(VirtualMachine &VM)
{
   VM.registers[dest_reg] = value;
}
