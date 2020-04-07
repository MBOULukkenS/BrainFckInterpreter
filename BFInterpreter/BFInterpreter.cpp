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
#include "Optimizer/BFOptimizer.h"

BFInterpreter::BFInterpreter(const std::vector<BFInstruction*>& instructions, bool flush, size_t cellAmount)
: BFRunner(instructions, flush)
{
    _environment = new BFInterpreterEnvironment(instructions, cellAmount);
    _bfEnvironment = *((BFInterpreterEnvironment*)_environment);

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
            {
                LogError("Interpreter failed; Outputting instruction context:\n")
                BFLoader::ExportInstructions(std::vector<BFInstruction*>(
                        _instructions.begin() + (_bfEnvironment.InstructionPtr - 5), 
                        _instructions.begin() + (_bfEnvironment.InstructionPtr + 5)), 
                                std::cout);
                LogFatal("Cell Pointer Underflow detected!! at: " + std::to_string(_bfEnvironment.InstructionPtr), -1);
            }
            if (_bfEnvironment.CurrentCell > _bfEnvironment.Memory + _bfEnvironment.PtrMaxOffset)
            {
                LogError("Interpreter failed; Outputting instruction context:\n")
                BFLoader::ExportInstructions(std::vector<BFInstruction*>(
                        _instructions.begin() + (_bfEnvironment.InstructionPtr - 5),
                        _instructions.begin() + (_bfEnvironment.InstructionPtr + 5)),
                                             std::cout);
                LogFatal("Cell Pointer Overflow detected!! at: " + std::to_string(_bfEnvironment.InstructionPtr), -1);
            }
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
            PutcharMethod(*_bfEnvironment.CurrentCell);
            break;
        case cReadPtrVal:
            *_bfEnvironment.CurrentCell = (BFCell)getchar();
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
        case None:
            break;
        default:
            LogFatal("Unhandled Instruction found!", -1);
    }

    _bfEnvironment.InstructionPtr++;
}
