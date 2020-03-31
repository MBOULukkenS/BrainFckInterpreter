//
// Created by Stijn on 04/12/2019.
//

#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "BFInterpreter.h"
#include "BFInstruction.h"
#include "../Logging.h"
#include "BFLoopInstruction.h"
#include "BFOptimizer.h"

BFInterpreter::BFInterpreter(const std::vector<BFInstruction*>& instructions, size_t cellAmount)
{    
    _instructions = instructions;
    _bfEnvironment = BFInterpreterEnvironment(instructions, cellAmount);
    
    BFLoader::BuildLoopInfo(_instructions);
}

void BFInterpreter::Run()
{
    size_t instructionCount = _instructions.size();
    while (_bfEnvironment.InstructionPtr < instructionCount)
    {        
        Step();
    }
}

void BFInterpreter::Step()
{
    BFInstruction *currentInstruction = _instructions[_bfEnvironment.InstructionPtr];
    
    switch (currentInstruction->InstructionType)
    {        
        case dPtrMod:
            _bfEnvironment.CurrentCell += currentInstruction->StepAmount;
            if (_bfEnvironment.CurrentCell < _bfEnvironment.Memory)
                LogFatal("Cell Pointer Underflow detected!!", -1);
            if (_bfEnvironment.CurrentCell > _bfEnvironment.Memory + _bfEnvironment.PtrMaxOffset)
                LogFatal("Cell Pointer Overflow detected!!", -1);
            
            break;
        case ModPtrVal:
            *_bfEnvironment.CurrentCell += currentInstruction->StepAmount;
            break;
        case ClearPtrVal:
            *_bfEnvironment.CurrentCell = 0;
            break;
        case cWritePtrVal:
            putchar((char)*_bfEnvironment.CurrentCell);
            break;
        case cReadPtrVal:
            *_bfEnvironment.CurrentCell = (char)getchar();
            break;
        case LoopBegin:
            if (*_bfEnvironment.CurrentCell != 0)
                break;
            
            _bfEnvironment.InstructionPtr = ((BFLoopInstruction*)currentInstruction)->LoopOther;
            break;
        case LoopEnd:
            if (*_bfEnvironment.CurrentCell == 0)
                break;
            
            _bfEnvironment.InstructionPtr = ((BFLoopInstruction*)currentInstruction)->LoopOther;
            break;
    }

    _bfEnvironment.InstructionPtr++;
}



void BFInterpreter::OptimizeInstructions()
{
    BFOptimizer::OptimizeCode(_instructions);
    BFLoader::BuildLoopInfo(_instructions);
}
