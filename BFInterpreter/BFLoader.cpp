//
// Created by Stijn on 12/12/2019.
//

#include <stack>
#include "BFLoader.h"
#include "Instructions/BFInstruction.h"
#include "../BFInstructionType.h"
#include "../Logging.h"
#include "Instructions/BFLoopInstruction.h"
#include "Instructions/BFMutatorInstruction.h"

std::vector<BFInstruction *> BFLoader::ParseInstructions(const std::string &instructionsStr)
{
    std::vector<BFInstruction*> instructions = std::vector<BFInstruction*>();
    for (char c : instructionsStr)
    {
        for(BFInstructionType instructionType : BFParsableInstructions)
        {
            if (instructionType != c)
                continue;

            BFInstruction *newInstruction = nullptr;
            switch (instructionType)
            {
                case dPtrIncr:
                    newInstruction = new BFMutatorInstruction(dPtrMod, instructionType, { 1 });
                    break;
                case dPtrDecr:
                    newInstruction = new BFMutatorInstruction(dPtrMod, instructionType, { -1 });
                    break;
                case IncrPtrVal:
                    newInstruction = new BFMutatorInstruction(ModPtrVal, instructionType, { 1 });
                    break;
                case DecrPtrVal:
                    newInstruction = new BFMutatorInstruction(ModPtrVal, instructionType, { -1 });
                    break;
                default:
                    newInstruction = new BFInstruction(instructionType);
                    break;
            }

            instructions.emplace_back(newInstruction);
            break;
        }
    }
    return instructions;
}

void BFLoader::BuildLoopInfo(std::vector<BFInstruction*> &instructions)
{
    std::stack<size_t> loopBegins = std::stack<size_t>();

    size_t instructionPtr = 0;
    for (BFInstruction *instruction : instructions)
    {
        switch (instruction->InstructionType)
        {
            case LoopBegin:
                loopBegins.push(instructionPtr);
                break;
            case LoopEnd:
                if (loopBegins.empty())
                    LogFatal("Loop end found at '" + std::to_string(instructionPtr) + "' but there was no loop to end!", 2);

                delete instructions[loopBegins.top()];
                delete instructions[instructionPtr];

                instructions[loopBegins.top()] = new BFLoopInstruction(LoopBegin, instructionPtr);
                instructions[instructionPtr] = new BFLoopInstruction(LoopEnd, loopBegins.top());

                loopBegins.pop();
                break;
            default:
                break; //Only loop instructions are needed to build loop info.
        }
        instructionPtr++;
    }

    if (!loopBegins.empty())
        LogFatal("Not all loops are correctly closed!", 2);
}
