//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINSTRUCTION_H
#define BRAINFCKINTERPRETER_BFINSTRUCTION_H


#include "../BFInstructionType.h"
#include "BFEnvironment.h"

class BFInstruction
{
public:
    BFInstruction(BFInstructionType type);
    
    BFInstructionType GetInstructionType();
    void Run(BFEnvironment environment);
private:
    BFInstructionType _instructionType;
    
    size_t _amount = 1; //by default, BF code only increments/decrements by 1, this behaviour can be changed for optimization purposes.
};


#endif //BRAINFCKINTERPRETER_BFINSTRUCTION_H
