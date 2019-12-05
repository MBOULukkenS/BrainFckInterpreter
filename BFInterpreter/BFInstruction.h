//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINSTRUCTION_H
#define BRAINFCKINTERPRETER_BFINSTRUCTION_H


#include "../BFInstructionType.h"

class BFInstruction
{
public:
    BFInstruction(BFInstructionType type);
    
    BFInstructionType GetInstructionType();
private:
    BFInstructionType _instructionType;
};


#endif //BRAINFCKINTERPRETER_BFINSTRUCTION_H
