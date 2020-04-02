//
// Created by Stijn on 04/12/2019.
//

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <functional>

#include "Instructions/BFInstruction.h"
#include "Instructions/BFLoopInstruction.h"
#include "Instructions/BFMutatorInstruction.h"

#include "BFInterpreter.h"
#include "../Logging.h"
#include "BFOptimizer.h"

std::function<int(int)> InPutcharMethod;

int InFlushedPutchar(int ch)
{
    int result = putchar(ch);
    _flushall();
    return result;
}

BFInterpreter::BFInterpreter(const std::vector<BFInstruction*>& instructions, bool flush, size_t cellAmount)
{    
    _instructions = instructions;
    _bfEnvironment = BFInterpreterEnvironment(instructions, cellAmount);
    
    if (flush)
        InPutcharMethod = InFlushedPutchar;
    else
        InPutcharMethod = putchar;
    
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
            _bfEnvironment.CurrentCell += ((BFMutatorInstruction*)currentInstruction)->Args[0];
            if (_bfEnvironment.CurrentCell < _bfEnvironment.Memory)
                LogFatal("Cell Pointer Underflow detected!! at: " + std::to_string(_bfEnvironment.InstructionPtr), -1);
            if (_bfEnvironment.CurrentCell > _bfEnvironment.Memory + _bfEnvironment.PtrMaxOffset)
                LogFatal("Cell Pointer Overflow detected!! at: " + std::to_string(_bfEnvironment.InstructionPtr), -1);
            
            break;
        case ModPtrVal:
            *_bfEnvironment.CurrentCell += ((BFMutatorInstruction*)currentInstruction)->Args[0];
            break;
        case MultiplyPtrVal:
            *(_bfEnvironment.CurrentCell + ((BFMutatorInstruction*)currentInstruction)->Args[0]) += 
                    ((*_bfEnvironment.CurrentCell) * ((BFMutatorInstruction*)currentInstruction)->Args[1]);
            break;
        case ClearPtrVal:
            *_bfEnvironment.CurrentCell = 0;
            break;
        case cWritePtrVal:
            InPutcharMethod((char)*_bfEnvironment.CurrentCell);
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
        default:
            LogFatal("Unhandled Instruction found!", -1);
    }

    _bfEnvironment.InstructionPtr++;
}

void BFInterpreter::OptimizeInstructions()
{
    BFOptimizer::OptimizeCode(_instructions);
    BFLoader::BuildLoopInfo(_instructions);
}
