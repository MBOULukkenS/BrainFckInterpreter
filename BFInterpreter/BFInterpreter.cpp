//
// Created by Stijn on 04/12/2019.
//

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <functional>
#include <cmath>

#include "Instructions/BFInstruction.h"
#include "Instructions/BFLoopInstruction.h"
#include "Instructions/BFMutatorInstruction.h"

#include "BFInterpreter.h"
#include "../Logging.h"
#include "Optimizer/BFOptimizer.h"

BFInterpreter::BFInterpreter(const std::vector<BFInstruction*>& instructions, bool flush, bool enableDump, size_t cellAmount)
: BFRunner(instructions, flush, enableDump)
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
    auto *currentMutatorInstruction = (BFMutatorInstruction*)currentInstruction;
    
    switch (currentInstruction->InstructionType)
    {        
        case dPtrMod:
            _bfEnvironment.CurrentCell += currentMutatorInstruction->Args[0];
            if (_bfEnvironment.CurrentCell < _bfEnvironment.Memory)
            {
                LogError("Interpreter failed; Outputting instruction context:\n")
                PrintInstructionTrace();
                DoCrashDump();
                
                LogFatal("Cell Pointer Underflow detected!! at: " + std::to_string(_bfEnvironment.InstructionPtr), -1);
            }
            if (_bfEnvironment.CurrentCell > _bfEnvironment.Memory + _bfEnvironment.PtrMaxOffset)
            {
                LogError("Interpreter failed; Outputting instruction context:\n")
                PrintInstructionTrace();
                DoCrashDump();
                
                LogFatal("Cell Pointer Overflow detected!! at: " + std::to_string(_bfEnvironment.InstructionPtr), -1);
            }
            break;
        case ModPtrVal:
            *_bfEnvironment.CurrentCell += currentMutatorInstruction->Args[0];
            break;
        case MultiplyPtrVal:
        {
            int64_t underflowCount = currentMutatorInstruction->Args[2];
            BFCell currentCellValue = *_bfEnvironment.CurrentCell;
            BFCell left =
                    underflowCount > 1 ? (BFCell)std::ceil((static_cast<double>(std::numeric_limits<BFCell>::max() + 1)
                        * (static_cast<double>(underflowCount) - currentCellValue)) / underflowCount) : currentCellValue;
            
            *(_bfEnvironment.CurrentCell + currentMutatorInstruction->Args[0]) +=
                    (left * currentMutatorInstruction->Args[1]);
            break;
        }
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
        case DumpMemory:
            DoMemoryDump();
            break;
        case None:
            break;
        default:
            LogFatal("Unhandled Instruction found!", -1);
    }

    _bfEnvironment.InstructionPtr++;
}

void BFInterpreter::DoCrashDump()
{
    DoMemoryDump(CrashDumpName);
}

void BFInterpreter::PrintInstructionTrace(size_t instructionAmount)
{
    auto begin = _instructions.begin() + (_bfEnvironment.InstructionPtr - instructionAmount);
    auto end = _instructions.begin() + (_bfEnvironment.InstructionPtr + instructionAmount);
    
    if (begin < _instructions.begin())
        begin = _instructions.begin();
    if (end > _instructions.end())
        end = _instructions.end();
    
    BFLoader::ExportInstructions(std::vector<BFInstruction*>(begin, end),std::cout);
}
