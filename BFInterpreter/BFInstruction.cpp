//
// Created by Stijn on 05/12/2019.
//


#include "BFInstruction.h"

BFInstruction::BFInstruction(BFInstructionType type, int64_t stepAmount)
{
    InstructionType = type;
    StepAmount = stepAmount;
}
