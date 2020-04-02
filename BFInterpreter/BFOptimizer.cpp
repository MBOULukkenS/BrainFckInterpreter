//
// Created by Stijn on 05/12/2019.
//

#include <stack>
#include <string>

#include <map>
#include <queue>
#include <algorithm>
#include <functional>
#include "Instructions/BFInstruction.h"
#include "BFOptimizer.h"
#include "../Logging.h"
#include "Instructions/BFMutatorInstruction.h"

typedef std::vector<BFInstructionType> LoopPattern;
typedef std::function<std::vector<BFInstruction*>(const std::vector<BFInstruction*>&)> LoopPatternHandler;

std::vector<BFInstruction*> Handle_ClearLoop(const std::vector<BFInstruction*>&);
std::vector<BFInstruction*> Handle_MultiplyLoop(const std::vector<BFInstruction*>&);

static const struct
{
    const LoopPattern ClearLoopMinus = { DecrPtrVal };
    const LoopPattern ClearLoopPlus = { IncrPtrVal };

    const LoopPattern ClearLoop = { ModPtrVal };
    
    const LoopPattern ScanLoopLeft = { dPtrDecr };
    const LoopPattern ScanLoopRight = { dPtrIncr };

    const LoopPattern ScanLoop = { dPtrMod };
    
    const LoopPattern FullMultiplyLoop = { dPtrIncr, ModPtrVal, dPtrDecr };
    const LoopPattern MultiplyLoopPart = { dPtrIncr, ModPtrVal };
    
    const std::vector<LoopPattern> SimpleLoops = { 
            ClearLoopMinus, 
            ClearLoopPlus, 
            ClearLoop, 
            
            ScanLoopLeft, 
            ScanLoopRight,
            ScanLoop 
    };
} BFLoopPatterns;


static std::map<LoopPattern, LoopPatternHandler> LoopPatterns = {
        std::pair(BFLoopPatterns.ClearLoop, Handle_ClearLoop),
        //std::pair(BFLoopPatterns.FullMultiplyLoop, Handle_MultiplyLoop)
};

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
    Optimize_Loops(result);
    
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
            BFInstructionType simpleType = None;
            
            while (!contractingInstructions.empty())
            {
                BFMutatorInstruction *ins = contractingInstructions.front();
                contractingInstructions.pop();

                if (ins->InstructionType == GetOppositeInstructionType(currentType))
                    amount -= ins->Args[0];
                else
                {
                    amount += ins->Args[0];
                    if (simpleType == None)
                        simpleType = ins->SimpleType;
                }
                
                delete ins;
            }

            if (amount != 0)
            {
                result.emplace_back(new BFMutatorInstruction(currentType, simpleType, { amount }));
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

bool InstructionTypeComparator(BFInstruction* const& left, BFInstructionType const& right)
{
    if (right == SearchWildcard)
        return true;

    BFMutatorInstruction *mLeft = nullptr;
    if ((mLeft = dynamic_cast<BFMutatorInstruction *>(left)))
        return left->InstructionType == right
               || mLeft->SimpleType == right;

    return left->InstructionType == right;
}

bool DetectPattern(const std::vector<BFInstruction*> &loopBody, const std::vector<BFInstructionType> &pattern)
{
    if (pattern.size() == 1)
        return std::equal(loopBody.begin(), loopBody.end(), pattern.begin(), pattern.end(), InstructionTypeComparator);
    
    return std::search(loopBody.begin(), loopBody.end(), pattern.begin(), pattern.end(), InstructionTypeComparator) != loopBody.end();
}

void BFOptimizer::Optimize_Loops(std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = std::vector<BFInstruction*>();
    
    std::queue<BFInstruction*> allQueuedInstructions = std::queue<BFInstruction*>();
    std::vector<BFInstruction*> loopBody = std::vector<BFInstruction*>();
    
    for (BFInstruction *instruction : instructions)
    {
        if (instruction->InstructionType != LoopBegin
            && allQueuedInstructions.empty()) //Skip instruction when we are not in a loop.
        {
            result.emplace_back(instruction);
            continue;
        }

        allQueuedInstructions.push(instruction);
        if (instruction->InstructionType != LoopBegin && instruction->InstructionType != LoopEnd)
        {
            loopBody.emplace_back(instruction);
            continue;
        }
        else if (allQueuedInstructions.size() == 1)
            continue;
        
        if (instruction->InstructionType == LoopEnd)
        {
            bool callbackHandled = loopBody.empty();
            for (const auto &loopPattern : LoopPatterns)
            {
                if (callbackHandled)
                    break;
                
                if ((callbackHandled = DetectPattern(loopBody, loopPattern.first)))
                {
                    auto newInstructions = loopPattern.second(loopBody);
                    result.insert(result.end(), newInstructions.begin(), newInstructions.end());
                }
            }

            if (callbackHandled)
            {
                while (!allQueuedInstructions.empty())
                {
                    BFInstruction *ins = allQueuedInstructions.front();
                    if (std::find(result.begin(), result.end(), ins) != result.end())
                        continue; //do not delete instructions that are still being used.
                    
                    delete ins;
                    allQueuedInstructions.pop();
                }
            }
        }

        loopBody.clear();
        while (!allQueuedInstructions.empty())
        {
            result.emplace_back(allQueuedInstructions.front());
            allQueuedInstructions.pop();
        } 
    }
    
    ValidateInstructions(result);
    instructions = result;
}

std::vector<BFInstruction*> Handle_ClearLoop(const std::vector<BFInstruction*>& loopBody)
{
    return std::vector<BFInstruction *> { new BFInstruction(ClearPtrVal) };
}

std::vector<BFInstruction*> Handle_MultiplyLoop(const std::vector<BFInstruction*>& loopBody)
{
    int multiplyCount = ((BFMutatorInstruction*)loopBody.back())->Args[0];
    std::vector<BFInstruction *> result = std::vector<BFInstruction *>();
    
    auto iterator = loopBody.begin(), end = loopBody.end();
    for (int64_t i = 0;;i++)
    {
        iterator = std::search(iterator, loopBody.end(),
                               BFLoopPatterns.MultiplyLoopPart.begin(), BFLoopPatterns.MultiplyLoopPart.end(),
                               InstructionTypeComparator);
        if (iterator == end)
            break;
        
        size_t pos = iterator - loopBody.begin();
        result.emplace_back(new BFMutatorInstruction(MultiplyPtrVal, None, 
                { ((BFMutatorInstruction*)loopBody[pos])->Args[0] + i, ((BFMutatorInstruction*)loopBody[pos+1])->Args[0] }));
        
        iterator++;
    }
    
    return result;
}

void BFOptimizer::OptimizeCode(std::vector<BFInstruction *> &instructions)
{
    LogDebug("Size before optimization: '" + std::to_string(instructions.size()) + "'");
    instructions = OptimizeCode((const std::vector<BFInstruction *>&)instructions);
    LogDebug("Size after optimization: '" + std::to_string(instructions.size()) + "'");
}
