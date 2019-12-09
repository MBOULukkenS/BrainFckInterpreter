//
// Created by Stijn on 05/12/2019.
//

#include <stack>
#include <algorithm>
#include "BFInstruction.h"
#include "BFOptimizer.h"
#include "../Logging.h"

static const struct
{
    const std::vector<BFInstructionType>  ClearLoopMinus = { LoopBegin, DecrPtrVal, LoopEnd };
    const std::vector<BFInstructionType> ClearLoopPlus = { LoopBegin, IncrPtrVal, LoopEnd };
} BFSimpleLoopPatterns;

std::vector<BFInstruction*> BFOptimizer::OptimizeCode(const std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = instructions;
    
    Optimize_Contraction(result);
    //Optimize_SimpleLoops(result);
    
    return result;
}

void BFOptimizer::Optimize_Contraction(std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = std::vector<BFInstruction*>();
    BFContractOptimizationInfo optimizationInfo {};
    
    for (BFInstruction *instruction : instructions)
    {        
        if (instruction->InstructionType != optimizationInfo.currentType)
        {
            if (optimizationInfo.currentType != None)
            {
                result.emplace_back(new BFInstruction(optimizationInfo.currentType, optimizationInfo.amount));

                optimizationInfo.amount = 0;
                optimizationInfo.currentType = None;
            }

            switch (instruction->InstructionType)
            {
                case cWritePtrVal:
                case cReadPtrVal:
                case LoopBegin:
                case LoopEnd:
                    result.emplace_back(instruction); //ignore instructions not optimized by this optimizer.
                    continue;
                default:
                    optimizationInfo.currentType = instruction->InstructionType;
            }
        }

        optimizationInfo.amount++;
        delete instruction;
    }

    int i = 0;
    for (BFInstruction *instr : result)
    {
        if (instr == nullptr)
        LogFatal("Optimizer failed: Nullptr found in instruction list at position '" + std::to_string(i) + "'!!", -5);
        i++;
    }
    
    instructions = result;
}

void BFOptimizer::Optimize_SimpleLoops(std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = std::vector<BFInstruction*>();
    std::vector<BFInstructionType> currentPattern;
    
    size_t currentIndex;
    size_t previousIndex = 0;
   /* do
    {
        auto it = std::search(instructions.begin(), instructions.end(),
                              BFSimpleLoopPatterns.ClearLoopMinus.begin(),
                              BFSimpleLoopPatterns.ClearLoopMinus.end(),
                              [](BFInstruction value1, BFInstructionType value2) -> bool
                              { return (value1.InstructionType == value2); });
        
        currentIndex = it - instructions.begin();
        if (it == instructions.end())
            break;
        
        for (size_t i = previousIndex; i < currentIndex - 3; i++)
        {
            result.emplace_back(instructions[i]);
        }
        
        result.emplace_back(it);
        previousIndex = currentIndex;
        
    } while(currentIndex < instructions.size()); */
    
    
    
    int i = 0;
    for (BFInstruction *instr : result)
    {
        if (instr == nullptr)
            LogFatal("Optimizer failed: Nullptr found in instruction list at position '" + std::to_string(i) + "'!!", -5);
        i++;
    }
    
    instructions = result;
}

void BFOptimizer::OptimizeCode(std::vector<BFInstruction *> &instructions)
{    
    instructions = OptimizeCode((const std::vector<BFInstruction *>&)instructions);
}
