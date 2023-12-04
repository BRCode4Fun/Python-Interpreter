#include "Vm.hpp"




void VirtualMachine::execute( std::vector<BytecodeInstruction*>& instructions)
{

  std::cout << "ins number " <<  instructions.size() << std::endl;

  this->registers.resize(20);

  for (BytecodeInstruction* instruction : instructions)
  {

    switch (instruction->opCode)
    {

      case OpCode::LOAD_INT:
      {

        int registerIndex = instruction->operands[0];
        registers[registerIndex] =  instruction->operands[1];
        break;
      }


      case OpCode::ADD:
      {


        int reg1 = instruction->operands[0];
        int reg2 = instruction->operands[1];
        registers[registers[2]] = registers[reg1] + registers[reg2];

        std::cout << "Sum value : " <<  registers[registers[2]] << std::endl;
        break;
    }


    }

  }

}
