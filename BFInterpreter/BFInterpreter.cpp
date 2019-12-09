//
// Created by Stijn on 04/12/2019.
//

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <stack>
#include <string>
#include <utility>

#include "BFInterpreter.h"
#include "BFInstruction.h"
#include "../Logging.h"
#include "BFLoopInstruction.h"
#include "BFOptimizer.h"

BFInterpreter::BFInterpreter(std::vector<BFInstruction*> instructions, size_t cellAmount)
{
    Construct(cellAmount);
    
    _instructions = instructions;
    BuildLoopInfo();
}

BFInterpreter::BFInterpreter(size_t cellAmount)
{
    Construct(cellAmount);
}

void BFInterpreter::Run()
{
    size_t instructionCount = _instructions.size();
    while (_bfEnvironment.InstructionPtr <= instructionCount)
    {        
        Step();
    }
}

void BFInterpreter::Step()
{
    BFInstruction *currentInstruction = _instructions[_bfEnvironment.InstructionPtr];
    switch (currentInstruction->InstructionType)
    {
        case dPtrIncr:
            _bfEnvironment.DataPtr += currentInstruction->StepAmount;
            if (_bfEnvironment.DataPtr > _bfEnvironment.CellAmount)
                LogFatal("Cell Pointer Overflow detected!!", -1);
            break;
        case dPtrDecr:
            _bfEnvironment.DataPtr -= currentInstruction->StepAmount;
            if (_bfEnvironment.DataPtr < 0)
                LogFatal("Cell Pointer Underflow detected!!", -1);
            break;
       /* case ClearPtrVal:
            _bfEnvironment.Memory[_bfEnvironment.DataPtr] = 0;
            break;*/ //currently unused
        case IncrPtrVal:
            _bfEnvironment.Memory[_bfEnvironment.DataPtr] += currentInstruction->StepAmount;
            break;
        case DecrPtrVal:
            _bfEnvironment.Memory[_bfEnvironment.DataPtr] -= currentInstruction->StepAmount;
            break;
        case cWritePtrVal:
            putchar((char)_bfEnvironment.Memory[_bfEnvironment.DataPtr]);
            break;
        case cReadPtrVal:
            _bfEnvironment.Memory[_bfEnvironment.DataPtr] = (char)getchar();
            break;
        case LoopBegin:
            if (_bfEnvironment.Memory[_bfEnvironment.DataPtr] != 0)
                break;
            
            _bfEnvironment.InstructionPtr = ((BFLoopInstruction*)currentInstruction)->LoopOther;
        case LoopEnd:
            if (_bfEnvironment.Memory[_bfEnvironment.DataPtr] == 0)
                break;
            
            _bfEnvironment.InstructionPtr = ((BFLoopInstruction*)currentInstruction)->LoopOther;
    }

    _bfEnvironment.InstructionPtr++;
}

void BFInterpreter::BuildLoopInfo()
{
    std::stack<size_t> loopBegins = std::stack<size_t>();
    
    size_t instructionPtr = 0;
    for (BFInstruction *instruction : _instructions)
    {
        switch (instruction->InstructionType)
        {
            case LoopBegin:
                loopBegins.push(instructionPtr);
                break;
            case LoopEnd:
                if (loopBegins.empty())
                    LogFatal("Loop end found at '" + std::to_string(instructionPtr) + "' but there was no loop to end!", 2);
                
                delete _instructions[loopBegins.top()];
                delete _instructions[instructionPtr];
                
                _instructions[loopBegins.top()] = new BFLoopInstruction(LoopBegin, instructionPtr);
                _instructions[instructionPtr] = new BFLoopInstruction(LoopEnd, loopBegins.top());
                
                loopBegins.pop();
                break;
        }
        instructionPtr++;
    }
    
    if (!loopBegins.empty())
        LogFatal("Not all loops are correctly closed!", 2);
}

void BFInterpreter::Construct(size_t cellAmount)
{
    _bfEnvironment = BFEnvironment(cellAmount);
}

std::vector<BFInstruction *> BFInterpreter::ParseInstructions(const std::string &instructionsStr)
{
    std::vector<BFInstruction*> instructions = std::vector<BFInstruction*>();
    for (char c : instructionsStr)
    {
        for(BFInstructionType instructionType : BFInstructionTypeList)
        {
            if (instructionType != c)
                continue;

            instructions.emplace_back(new BFInstruction((BFInstructionType)c));
            break;
        }
    }
    return instructions;
}

void BFInterpreter::OptimizeInstructions()
{
    LogDebug("Size before optimization: '" + std::to_string(_instructions.size()) + "'");
    BFOptimizer::OptimizeCode(_instructions);
    BuildLoopInfo();
    LogDebug("Size after optimization: '" + std::to_string(_instructions.size()) + "'")
}
