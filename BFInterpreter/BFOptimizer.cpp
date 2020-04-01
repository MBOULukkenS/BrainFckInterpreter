//
// Created by Stijn on 05/12/2019.
//

#include <stack>
#include <queue>
#include <algorithm>
#include <functional>
#include "Instructions/BFInstruction.h"
#include "BFOptimizer.h"
#include "../Logging.h"
#include "Instructions/BFMutatorInstruction.h"

static const struct
{
    const std::vector<BFInstructionType> ClearLoopMinus = { DecrPtrVal };
    const std::vector<BFInstructionType> ClearLoopPlus = { IncrPtrVal };

    const std::vector<BFInstructionType> ClearLoop = { ModPtrVal };
    
    const std::vector<BFInstructionType> ScanLoopLeft = { dPtrDecr };
    const std::vector<BFInstructionType> ScanLoopRight = { dPtrIncr };
} BFSimpleLoopPatterns;

bool Optimize_SimpleLoops(const std::vector<BFInstruction*> &loopBody, std::vector<BFInstruction*> &result);

static std::vector<std::function<bool(const std::vector<BFInstruction*> &loopBody, 
        std::vector<BFInstruction*> &result)>> LoopOptimizationCallbacks = { Optimize_SimpleLoops };

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
    
    std::queue<BFMutatorInstruction*> contractingInstructions = std::queue<BFMutatorInstruction*>();

    BFInstructionType currentType = None;

    for (BFInstruction *instruction : instructions)
    {
        BFMutatorInstruction *mutInstruction;
        bool handledInstruction = (mutInstruction = dynamic_cast<BFMutatorInstruction*>(instruction));
        
        if (instruction->InstructionType != currentType 
        && instruction->InstructionType != GetOppositeInstructionType(currentType)
        && currentType != None
        && !contractingInstructions.empty())
        {
            int64_t amount = 0;
            
            while (!contractingInstructions.empty())
            {
                BFMutatorInstruction *ins = contractingInstructions.front();
                contractingInstructions.pop();

                if (ins->InstructionType == GetOppositeInstructionType(currentType))
                    amount -= ins->StepAmount;
                else
                    amount += ins->StepAmount;
                
                delete ins;
            }

            if (amount != 0)
            {
                result.emplace_back(new BFMutatorInstruction(currentType, amount));
            }
            
            currentType = None;
        }

        if (!handledInstruction)
        {
            result.emplace_back(instruction);
        }
        else
        {
            if (currentType == None)
                currentType = mutInstruction->InstructionType;
            contractingInstructions.emplace(mutInstruction);
        }
    }
    
    ValidateInstructions(result);
    instructions = result;
}

void BFOptimizer::Optimize_SimpleLoops(std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = std::vector<BFInstruction*>();
    
    std::queue<BFInstruction*> allQueuedInstructions = std::queue<BFInstruction*>();
    std::vector<BFInstruction*> queuedBodyInstructions = std::vector<BFInstruction*>();
    
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
        && instruction->InstructionType == LoopEnd)
        {
            bool callbackHandled = false;
            for (const auto &callback : LoopOptimizationCallbacks)
            {
                if ((callbackHandled = callback(queuedBodyInstructions, result)))
                    break;
            }

            if (callbackHandled)
            {
                queuedBodyInstructions.clear();
                do
                {
                    BFInstruction *ins = allQueuedInstructions.front();
                    if (std::find(result.begin(), result.end(), ins) != result.end())
                        continue; //do not delete instructions that are still being used.
                    
                    delete ins;
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

bool Optimize_SimpleLoops(const std::vector<BFInstruction*> &loopBody, std::vector<BFInstruction*> &result)
{
    if (loopBody.size() > 1)
        return false;

    switch (loopBody[0]->InstructionType)
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
            return true;
        default:
            return false;
    }
}

void BFOptimizer::OptimizeCode(std::vector<BFInstruction *> &instructions)
{
    LogDebug("Size before optimization: '" + std::to_string(instructions.size()) + "'");
    instructions = OptimizeCode((const std::vector<BFInstruction *>&)instructions);
    LogDebug("Size after optimization: '" + std::to_string(instructions.size()) + "'");
}
