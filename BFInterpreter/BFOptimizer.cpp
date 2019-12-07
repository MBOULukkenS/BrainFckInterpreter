//
// Created by Stijn on 05/12/2019.
//

#include <queue>
#include <stack>
#include "BFInstruction.h"
#include "BFOptimizer.h"

std::vector<BFInstruction*> BFOptimizer::OptimizeCode(const std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = instructions;
    
    result = Optimize_Contraction(result);
    //result = Optimize_SimpleLoops(result);
    
    return result;
}

std::vector<BFInstruction*>
BFOptimizer::Optimize_Contraction(const std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = std::vector<BFInstruction*>();
    BFOptimizationInfo optimizationInfo {};
    
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
                case loopBegin:
                case loopEnd:
                    result.emplace_back(new BFInstruction(instruction)); //ignore instructions not optimized by this optimizer.
                    continue;
                default:
                    optimizationInfo.currentType = instruction->InstructionType;
            }
        }

        optimizationInfo.amount++;
        delete instruction;
    }
    
    return result;
}

std::vector<BFInstruction*> BFOptimizer::Optimize_SimpleLoops(const std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = instructions;
    BFOptimizationInfo optimizationInfo {};
    
    for (BFInstruction *instruction : instructions)
    {
        switch (instruction->InstructionType)
        {
            case loopBegin:
                break;
            case loopEnd:
                break;

            case dPtrIncr:
            case dPtrDecr:
                break;
        }
        delete instruction;
    }
    
    return result;
}

void BFOptimizer::OptimizeCode(std::vector<BFInstruction *> &instructions)
{    
    instructions = OptimizeCode((const std::vector<BFInstruction *>&)instructions);
}
