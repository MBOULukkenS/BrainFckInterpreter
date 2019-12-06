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
    explicit BFInstruction(BFInstructionType type);

    BFInstructionType InstructionType;
    size_t StepAmount = 1;
};


#endif //BRAINFCKINTERPRETER_BFINSTRUCTION_H
