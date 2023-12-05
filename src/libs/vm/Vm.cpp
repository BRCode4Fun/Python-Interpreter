#include "Vm.hpp"





void VirtualMachine::execute( std::vector<BytecodeInstruction*>& instructions)
{

  std::cout << "ins number " <<  instructions.size() << std::endl;

  this->registers.resize(20);

  for (BytecodeInstruction* instruction : instructions)
  {
             instruction->execute(*this);
  }

}
