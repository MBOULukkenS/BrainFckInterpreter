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
#include "../Logging.h"

std::vector<BFInstructionType> _instructions;


BFInterpreter::BFInterpreter(std::vector<BFInstructionType> &instructions, size_t cellAmount)
{
    _instructions = instructions;
    _cellAmount = cellAmount;
    _memory =  (BFCell *)calloc(cellAmount, sizeof(BFCell));
    _running = false;
    BuildLoopInfo();
}

void BFInterpreter::Run()
{
    _running = true;
    size_t instructionCount = _instructions.size();
    
    while (_running)
    {
        if (instruction_ptr >= instructionCount)
        {
            _running = false;
            break;
        }
        
        Step();
    }
}

void BFInterpreter::Step()
{
    BFInstructionType currentInstruction = _instructions[instruction_ptr];
    switch (currentInstruction)
    {
        case dPtrIncr:
            _dataPtr++;
            if (_dataPtr > _cellAmount)
                LogFatal("Cell Overflow detected!!", -1);
            break;
        case dPtrDecr:
            _dataPtr--;
            if (_dataPtr < 0)
                LogFatal("Cell Underflow detected!!", -1);
            break;
        case IncrPtrVal:
            _memory[_dataPtr]++;
            break;
        case DecrPtrVal:
            _memory[_dataPtr]--;
            break;
        case cWritePtrVal:
            std::cout << (char)_memory[_dataPtr];
            break;
        case cReadPtrVal:
            _memory[_dataPtr] = (char)getchar();
            break;
        case loopBegin:
            if (_memory[_dataPtr] != 0)
                break;

            instruction_ptr = _loopInfo[instruction_ptr];
            return;
        case loopEnd:
            if (_memory[_dataPtr] == 0)
                break;

            instruction_ptr = _loopInfoReversed[instruction_ptr];
            return;
    }

    instruction_ptr++;
}

void BFInterpreter::BuildLoopInfo()
{
    _loopInfo = std::map<size_t, size_t>();
    _loopInfoReversed = std::map<size_t, size_t>();
    std::stack<size_t> loopBegins = std::stack<size_t>();
    
    size_t instructionPtr = 0;
    for (BFInstructionType instruction : _instructions)
    {
        switch (instruction)
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
