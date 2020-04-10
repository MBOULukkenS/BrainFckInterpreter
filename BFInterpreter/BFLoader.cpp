//
// Created by Stijn on 12/12/2019.
//

#include <stack>
#include <map>
#include "BFLoader.h"
#include "Instructions/BFInstruction.h"
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

void BFLoader::ExportInstructions(const std::vector<BFInstruction *> &instructions, std::ostream &output, bool lineNumbers)
{
    std::string outputString = std::string();
    
    size_t i = 0;
    for (auto instruction : instructions)
    {
        if (lineNumbers)
            output << i << ": ";
        
        output << (char)instruction->InstructionType;
        
        BFMutatorInstruction *mutatorInstruction;
        BFLoopInstruction *loopInstruction;
        if ((mutatorInstruction = dynamic_cast<BFMutatorInstruction*>(instruction)))
        {
            output << "(";
            for (auto it = mutatorInstruction->Args.begin(); it != mutatorInstruction->Args.end(); ++it)
            {
                output << *it;
                if (std::next(it) != mutatorInstruction->Args.end())
                    output << ", ";
            }
            output << ")";
        }
        else if ((loopInstruction = dynamic_cast<BFLoopInstruction*>(instruction)))
        {
            output << "-> ";
            output << loopInstruction->LoopOther;
        }
        
        output << std::endl;
        i++;
    }
}

std::string SanitizeInstructions(const std::string &instructionsStr, const BFTokenInfo &tokenInfo)
{
    std::vector<char> allowedChars = std::vector<char>();
    return std::string();
}

std::vector<BFInstruction *>
BFLoader::ParseInstructions(const std::string &instructionsStr, const BFTokenInfo &tokenInfo)
{
    std::vector<BFInstruction*> instructions = std::vector<BFInstruction*>();
    
    auto iterator = instructionsStr.begin(), end = instructionsStr.end();
    while (iterator != end)
    {
        bool success = false;
        for (const auto& item : tokenInfo.Tokens)
        {
            int tokenLength = item.first.size();
            
            auto result = std::equal(iterator, iterator + tokenLength, item.first.begin());
            if (!result)
                continue;

            success = true;
            iterator += tokenLength;

            BFInstruction *newInstruction = nullptr;
            switch (item.second)
            {
                case dPtrIncr:
                    newInstruction = new BFMutatorInstruction(dPtrMod, item.second, { 1 });
                    break;
                case dPtrDecr:
                    newInstruction = new BFMutatorInstruction(dPtrMod, item.second, { -1 });
                    break;
                case IncrPtrVal:
                    newInstruction = new BFMutatorInstruction(ModPtrVal, item.second, { 1 });
                    break;
                case DecrPtrVal:
                    newInstruction = new BFMutatorInstruction(ModPtrVal, item.second, { -1 });
                    break;
                default:
                    newInstruction = new BFInstruction(item.second);
                    break;
            }
            
            instructions.emplace_back(newInstruction);
            break;
        }
        if (!success && iterator != end)
            iterator++;
    }
    
    return instructions;
}

void BFLoader::ConvertToDialect(const std::vector<BFInstruction *> &instructions, const BFTokenInfo &outputFormat,
                                std::ostream &outputStream)
{
    for (auto instruction : instructions)
    {
        BFMutatorInstruction *mutatorInstruction;
        
        size_t emitCount = 1;
        BFInstructionType emitType = instruction->InstructionType;
        
        if ((mutatorInstruction = dynamic_cast<BFMutatorInstruction*>(instruction)))
        {
            emitType = mutatorInstruction->SimpleType;
            emitCount = std::abs(mutatorInstruction->Args[0]);
        }
        
        auto result = *std::find_if(outputFormat.Tokens.begin(), outputFormat.Tokens.end(), [&](const std::pair<std::string, BFInstructionType> &item){
            return item.second == emitType;
        });
        
        for (size_t i = 0; i < emitCount; i++)
        {
            outputStream << result.first;
            if (instruction != instructions.back())
                outputStream << outputFormat.Delimiter;
        }
    }
}
