//
// Created by Stijn on 05/12/2019.
//

#include "BFInstruction.h"

BFInstruction::BFInstruction(BFInstructionType type)
{
    _instructionType = type;
}

BFInstructionType BFInstruction::GetInstructionType()
{
    return _instructionType;
}
