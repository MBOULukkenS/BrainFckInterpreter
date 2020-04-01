//
// Created by Stijn on 01/04/2020.
//

#ifndef BRAINFCKINTERPRETER_BFMUTATORINSTRUCTION_H
#define BRAINFCKINTERPRETER_BFMUTATORINSTRUCTION_H

#include "BFInstruction.h"

class BFMutatorInstruction : public BFInstruction
{
public:
    explicit BFMutatorInstruction(BFInstructionType type, int64_t stepAmount)
    : BFInstruction(type), StepAmount(stepAmount) {}

    explicit BFMutatorInstruction(const BFMutatorInstruction *other) 
    : BFMutatorInstruction(other->InstructionType, other->StepAmount) {}

    BFMutatorInstruction(const BFMutatorInstruction &other) 
    : BFMutatorInstruction(other.InstructionType, other.StepAmount) {}

    int64_t StepAmount = 1;
};

#endif //BRAINFCKINTERPRETER_BFMUTATORINSTRUCTION_H
