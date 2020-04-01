//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINSTRUCTION_H
#define BRAINFCKINTERPRETER_BFINSTRUCTION_H


#include "../../BFInstructionType.h"
#include "../../BFDefines.h"

class BFInstruction
{
public:
    explicit BFInstruction(BFInstructionType type);
    virtual ~BFInstruction() = default;

    BFInstructionType InstructionType;
};


#endif //BRAINFCKINTERPRETER_BFINSTRUCTION_H
