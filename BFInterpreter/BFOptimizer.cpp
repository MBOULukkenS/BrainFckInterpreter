//
// Created by Stijn on 05/12/2019.
//

#include <stack>
#include <queue>
#include <algorithm>
#include <regex>
#include "BFInstruction.h"
#include "BFOptimizer.h"
#include "../Logging.h"

static const struct
{
    const std::vector<BFInstructionType> ClearLoopMinus = { DecrPtrVal };
    const std::vector<BFInstructionType> ClearLoopPlus = { IncrPtrVal };

    const std::vector<BFInstructionType> ClearLoop = { ModPtrVal };
    
    const std::vector<BFInstructionType> ScanLoopLeft = { dPtrDecr };
    const std::vector<BFInstructionType> ScanLoopRight = { dPtrIncr };
} BFSimpleLoopPatterns;

void ValidateInstructions(std::vector<BFInstruction*>& instructions)
{
    int i = 0;
    for (BFInstruction *instr : instructions)
    {
        if (instr == nullptr)
        LogFatal("Optimizer failed: Nullptr found in instruction list at position '" + std::to_string(i) + "'!!", -5);
        i++;
    }
}

BFInstructionType GetOppositeInstructionType(BFInstructionType instructionType)
{
    switch (instructionType)
    {
        case dPtrIncr:
            return dPtrDecr;
        case dPtrDecr:
            return dPtrIncr;
        case IncrPtrVal:
            return DecrPtrVal;
        case DecrPtrVal:
            return IncrPtrVal;
        case cWritePtrVal:
            return cReadPtrVal;
        case cReadPtrVal:
            return cWritePtrVal;
        case LoopBegin:
            return LoopEnd;
        case LoopEnd:
            return LoopBegin;
        default:
            return None;
    }
}

bool IsOppositeInstructionType(BFInstructionType left, BFInstructionType right)
{
    BFInstructionType leftOpposite = GetOppositeInstructionType(left);
    return leftOpposite != None && leftOpposite == right;
}

std::vector<BFInstruction*> BFOptimizer::OptimizeCode(const std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = instructions;
    
    Optimize_Contraction(result);
    Optimize_SimpleLoops(result);
    
    return result;
}

void BFOptimizer::Optimize_Contraction(std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = std::vector<BFInstruction*>();
    BFContractionOptimizationInfo optimizationInfo {};
    
    for (BFInstruction *instruction : instructions)
    {
        bool isOppositeInstruction = IsOppositeInstructionType(instruction->InstructionType, optimizationInfo.currentType);
        if (instruction->InstructionType != optimizationInfo.currentType)
        {
            if (optimizationInfo.currentType != None)
            {
                if (optimizationInfo.amount > 0)
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
        if (isOppositeInstruction)
            optimizationInfo.amount -= instruction->StepAmount;
        else
            optimizationInfo.amount += instruction->StepAmount;
        
        delete instruction;
    }
    
    ValidateInstructions(result);
    instructions = result;
}

void BFOptimizer::Optimize_SimpleLoops(std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = std::vector<BFInstruction*>();
    
    std::queue<BFInstruction*> allQueuedInstructions = std::queue<BFInstruction*>();
    std::vector<BFInstruction*> queuedBodyInstructions = std::vector<BFInstruction*>();

    BFSimpleLoopOptimizationInfo optimizationInfo {};
    
    for (BFInstruction *instruction : instructions)
    {
        if (instruction->InstructionType != LoopBegin
            && allQueuedInstructions.empty())
        {
            result.emplace_back(instruction);
            continue;
        }

        allQueuedInstructions.push(instruction);
        if (instruction->InstructionType != LoopBegin && instruction->InstructionType != LoopEnd)
        {
            queuedBodyInstructions.emplace_back(instruction);
            continue;
        }
        else if (queuedBodyInstructions.empty() && allQueuedInstructions.size() == 1)
            continue;
        
        if (!queuedBodyInstructions.empty() 
        && queuedBodyInstructions.size() == 1 
        && instruction->InstructionType == LoopEnd)
        {
            bool abort = false;
            switch (queuedBodyInstructions[0]->InstructionType)
            {
                //case dPtrMod:
                //    break;
                //case dPtrIncr:
                //    //scan loop right TODO: Implement this
                //    break;
                //case dPtrDecr:
                //    //scan loop left TODO: Implement this
                //    break;
                case ModPtrVal:
                case IncrPtrVal:
                case DecrPtrVal:
                    result.emplace_back(new BFInstruction(ClearPtrVal));
                    break;
                default:
                    abort = true;
                    break;
            }

            if (!abort)
            {
                queuedBodyInstructions.clear();
                do
                {
                    delete allQueuedInstructions.front();
                    allQueuedInstructions.pop();
                } while (!allQueuedInstructions.empty());
                continue;
            }
        }

        queuedBodyInstructions.clear();
        do
        {
            result.emplace_back(allQueuedInstructions.front());
            allQueuedInstructions.pop();
        } while (!allQueuedInstructions.empty());
    }
    
    ValidateInstructions(result);
    instructions = result;
}

void BFOptimizer::OptimizeCode(std::vector<BFInstruction *> &instructions)
{
    LogDebug("Size before optimization: '" + std::to_string(instructions.size()) + "'");
    instructions = OptimizeCode((const std::vector<BFInstruction *>&)instructions);
    LogDebug("Size after optimization: '" + std::to_string(instructions.size()) + "'");
}
