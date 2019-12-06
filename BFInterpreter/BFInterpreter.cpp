//
// Created by Stijn on 04/12/2019.
//

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <queue>
#include <stack>
#include <string>

#include "BFInterpreter.h"
#include "BFInstruction.h"
#include "../Logging.h"

BFInterpreter::BFInterpreter(std::vector<BFInstruction> &instructions, size_t cellAmount)
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
    BFInstruction currentInstruction = _instructions[_bfEnvironment.InstructionPtr];
    switch (currentInstruction.InstructionType)
    {
        case dPtrIncr:
            _bfEnvironment.DataPtr += currentInstruction.StepAmount;
            if (_bfEnvironment.DataPtr > _bfEnvironment.CellAmount)
                LogFatal("Cell Pointer Overflow detected!!", -1);
            break;
        case dPtrDecr:
            _bfEnvironment.DataPtr -= currentInstruction.StepAmount;
            if (_bfEnvironment.DataPtr < 0)
                LogFatal("Cell Pointer Underflow detected!!", -1);
            break;
        case IncrPtrVal:
            _bfEnvironment.Memory[_bfEnvironment.DataPtr] += currentInstruction.StepAmount;
            break;
        case DecrPtrVal:
            _bfEnvironment.Memory[_bfEnvironment.DataPtr] -= currentInstruction.StepAmount;
            break;
        case cWritePtrVal:
            putchar((char)_bfEnvironment.Memory[_bfEnvironment.DataPtr]);
            break;
        case cReadPtrVal:
            _bfEnvironment.Memory[_bfEnvironment.DataPtr] = (char)getchar();
            break;
        case loopBegin:
            if (_bfEnvironment.Memory[_bfEnvironment.DataPtr] != 0)
                break;

            _bfEnvironment.InstructionPtr = _loopInfo[_bfEnvironment.InstructionPtr];
            return;
        case loopEnd:
            if (_bfEnvironment.Memory[_bfEnvironment.DataPtr] == 0)
                break;

            _bfEnvironment.InstructionPtr = _loopInfoReversed[_bfEnvironment.InstructionPtr];
            return;
    }

    _bfEnvironment.InstructionPtr++;
}

void BFInterpreter::BuildLoopInfo()
{
    _loopInfo = std::map<size_t, size_t>();
    _loopInfoReversed = std::map<size_t, size_t>();
    std::stack<size_t> loopBegins = std::stack<size_t>();
    
    size_t instructionPtr = 0;
    for (BFInstruction instruction : _instructions)
    {
        switch (instruction.InstructionType)
        {
            case loopBegin:
                loopBegins.push(instructionPtr);
                break;
            case loopEnd:
                if (loopBegins.empty())
                    LogFatal("Loop end found at '" + std::to_string(instructionPtr) + "' but there was no loop to end!", 2);
                
                _loopInfo.emplace(BFLoop(loopBegins.top(), instructionPtr));
                _loopInfoReversed.emplace(BFLoop(instructionPtr, loopBegins.top()));
                
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
