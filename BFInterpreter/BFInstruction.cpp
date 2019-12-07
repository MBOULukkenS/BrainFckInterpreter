//
// Created by Stijn on 05/12/2019.
//


#include "BFInstruction.h"
#include "../Logging.h"

BFInstruction::BFInstruction(BFInstructionType type, BFCell stepAmount)
{
    InstructionType = type;
    StepAmount = stepAmount;
}
