//
// Created by Stijn on 07/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFLOOPINSTRUCTION_H
#define BRAINFCKINTERPRETER_BFLOOPINSTRUCTION_H

#include "BFInstruction.h"

class BFLoopInstruction : public BFInstruction
{
public:
    explicit BFLoopInstruction(BFInstructionType type, size_t loopOther) 
    : BFInstruction(type), LoopOther(loopOther) {}
    
    explicit BFLoopInstruction(BFLoopInstruction *other) 
    : BFLoopInstruction(other->InstructionType, other->LoopOther) {}
    
    size_t LoopOther;
};

#endif //BRAINFCKINTERPRETER_BFLOOPINSTRUCTION_H
