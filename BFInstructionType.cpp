//
// Created by Stijn on 02/04/2020.
//

#include "BFInstructionType.h"

BFInstructionType GetOppositeInstructionType(BFInstructionType instructionType)
{
    switch (instructionType)
    {
        case dPtrIncr:
            return dPtrDecr;
        case dPtrDecr:
            return dPtrIncr;
        case IncrPtrVal:
            return DecrPtrVal;
        case DecrPtrVal:
            return IncrPtrVal;
        case cWritePtrVal:
            return cReadPtrVal;
        case cReadPtrVal:
            return cWritePtrVal;
        case LoopBegin:
            return LoopEnd;
        case LoopEnd:
            return LoopBegin;
        default:
            return None;
    }
}