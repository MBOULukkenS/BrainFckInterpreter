//
// Created by Stijn on 05/12/2019.
//


#include "BFInstruction.h"
#include "../Logging.h"

BFInstruction::BFInstruction(BFInstructionType type)
{
    _instructionType = type;
}

BFInstructionType BFInstruction::GetInstructionType()
{
    return _instructionType;
}

void BFInstruction::Run(BFEnvironment environment)
{
    switch (_instructionType)
    {
        case dPtrIncr:
            environment.DataPtr++;
            if (environment.DataPtr > environment.CellAmount)
                LogFatal("Cell Overflow detected!!", -1);
            break;
        case dPtrDecr:
            environment.DataPtr--;
            if (environment.DataPtr < 0)
                LogFatal("Cell Underflow detected!!", -1);
            break;
        case IncrPtrVal:
            environment.Memory[environment.DataPtr]++;
            break;
        case DecrPtrVal:
            environment.Memory[environment.DataPtr]--;
            break;
        case cWritePtrVal:
            std::cout << (char)environment.Memory[environment.DataPtr];
            break;
        case cReadPtrVal:
            environment.Memory[environment.DataPtr] = (char)getchar();
            break;
        case loopBegin:
            if (environment.Memory[environment.DataPtr] != 0)
                break;

            environment.Instruction_ptr = _loopInfo[environment.Instruction_ptr];
            return;
        case loopEnd:
            if (environment.Memory[environment.DataPtr] == 0)
                break;

            environment.Instruction_ptr = _loopInfoReversed[environment.Instruction_ptr];
            return;
    }
}
