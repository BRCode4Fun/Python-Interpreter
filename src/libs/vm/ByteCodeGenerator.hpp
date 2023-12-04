#pragma once

#include <limits>

#include "./Instruction.hpp"

class BytecodeGenerator
{

public:

  BytecodeGenerator()
  {
      this->registers = 0;
  }

  int allocateRegister()
  {
    //  std::cout << "regis " << this->registers << std::endl;
      if (registers <  std::numeric_limits<int>::max() )
      {

        return registers++;

      } else {

          std::cout << "MAX REGISTER NUMBER REACHED!" << std::endl;
          std::exit(-1);
      }
  }

  void append_instruction(BytecodeInstruction* instruction)
  {
    instructions.push_back(instruction);
    return;
  }



   int registers;
   std::vector<BytecodeInstruction*>  instructions;


};
