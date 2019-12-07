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
    explicit BFInstruction(BFInstructionType type, BFCell stepAmount = 1);
    explicit BFInstruction(const BFInstruction *other) : BFInstruction(other->InstructionType, other->StepAmount) {}
    BFInstruction(const BFInstruction &other) : BFInstruction(other.InstructionType, other.StepAmount) {}

    BFInstructionType InstructionType;
    BFCell StepAmount = 1;
};


#endif //BRAINFCKINTERPRETER_BFINSTRUCTION_H
