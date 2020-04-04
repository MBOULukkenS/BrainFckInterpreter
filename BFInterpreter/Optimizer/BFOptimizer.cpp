//
// Created by Stijn on 05/12/2019.
//

#include <stack>
#include <string>

#include <map>
#include <queue>
#include <algorithm>
#include <functional>
#include "../Instructions/BFInstruction.h"
#include "BFOptimizer.h"
#include "../../Logging.h"
#include "../Instructions/BFMutatorInstruction.h"

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
    
    const LoopPattern FullMultiplyLoop = { dPtrIncr, IncrPtrVal, dPtrDecr, DecrPtrVal };
    const LoopPattern ReverseClearFullMultiplyLoop = { DecrPtrVal, dPtrIncr, IncrPtrVal, dPtrDecr };

    const LoopPattern NegativeFullMultiplyLoop = { dPtrDecr, IncrPtrVal, dPtrIncr, DecrPtrVal };

    const LoopPattern MultiplyLoopPart = { dPtrMod, IncrPtrVal };
    
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
        
        std::pair(BFLoopPatterns.FullMultiplyLoop, Handle_MultiplyLoop),
        std::pair(BFLoopPatterns.ReverseClearFullMultiplyLoop, Handle_MultiplyLoop),
        std::pair(BFLoopPatterns.NegativeFullMultiplyLoop, Handle_MultiplyLoop),
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

bool DetectPattern(const std::vector<BFInstruction*> &loopBody, 
        const std::vector<BFInstructionType> &pattern, 
        bool verifyBodyOnlyContainsPatternTypes = false)
{
    if (pattern.size() == 1)
        return std::equal(loopBody.begin(), loopBody.end(), pattern.begin(), pattern.end(), InstructionTypeComparator);
    
    bool allContainedInstructionsValid = true;
    if (verifyBodyOnlyContainsPatternTypes)
    {
        allContainedInstructionsValid = std::all_of(loopBody.begin(), loopBody.end(),
                [=](BFInstruction* const& instruction){
            return std::any_of(pattern.begin(), pattern.end(), 
                    [&](const BFInstructionType &patternInstruction){
                return InstructionTypeComparator(instruction, patternInstruction);
            });
        });
    }
    auto it = std::search(
            loopBody.begin(), loopBody.end(),
            pattern.begin(), pattern.end(),
            InstructionTypeComparator);
    
    return allContainedInstructionsValid 
    && it != loopBody.end()
    && (it + pattern.size() == loopBody.end() || it == loopBody.begin());
}

void BFOptimizer::Optimize_Loops(std::vector<BFInstruction*>& instructions)
{
    std::vector<BFInstruction*> result = std::vector<BFInstruction*>();
    
    std::stack<LoopOptimizeInfo> loopStack = std::stack<LoopOptimizeInfo>();
    LoopOptimizeInfo currentLoop = LoopOptimizeInfo();
    
    for (BFInstruction *instruction : instructions)
    {
        if (instruction->InstructionType != LoopBegin
            && currentLoop.AllQueuedInstructions.empty()) //Skip instruction when we are not in a loop.
        {
            result.emplace_back(instruction);
            continue;
        }
        
        if (!currentLoop.AllQueuedInstructions.empty() && instruction->InstructionType == LoopBegin)
        {
            loopStack.push(currentLoop);
            currentLoop = LoopOptimizeInfo();
        }
        
        currentLoop.AllQueuedInstructions.push(instruction);
        if (instruction->InstructionType != LoopBegin && instruction->InstructionType != LoopEnd)
        {
            currentLoop.LoopBody.emplace_back(instruction);
            continue;
        }
        else if (currentLoop.AllQueuedInstructions.size() == 1)
            continue;
        
        if (instruction->InstructionType == LoopEnd)
        {
            bool callbackHandled = currentLoop.LoopBody.empty();
            for (const auto &loopPattern : LoopPatterns)
            {
                if (callbackHandled 
                || std::find_if(currentLoop.LoopBody.begin(), currentLoop.LoopBody.end(),[](BFInstruction* const& item){
                    return item->InstructionType == LoopBegin || item->InstructionType == LoopEnd;
                }) != currentLoop.LoopBody.end())
                {
                    break;
                }
                
                if ((callbackHandled = DetectPattern(currentLoop.LoopBody, loopPattern.first, true)))
                {
                    auto newInstructions = loopPattern.second(currentLoop.LoopBody);
                    std::reverse(newInstructions.begin(), newInstructions.end());
                    
                    currentLoop.Output.insert(currentLoop.Output.end(), newInstructions.begin(), newInstructions.end());
                }
            }

            if (callbackHandled)
            {
                while (!currentLoop.AllQueuedInstructions.empty())
                {
                    BFInstruction *ins = currentLoop.AllQueuedInstructions.front();
                    if (std::find(result.begin(), result.end(), ins) != result.end())
                        continue; //do not delete instructions that are still being used.
                    
                    delete ins;
                    currentLoop.AllQueuedInstructions.pop();
                }
                while (!currentLoop.Output.empty())
                {
                    currentLoop.AllQueuedInstructions.push(currentLoop.Output.back());
                    currentLoop.Output.pop_back();
                }
            }
            
            if (!loopStack.empty())
            {
                LoopOptimizeInfo parent = loopStack.top();
                
                while (!currentLoop.AllQueuedInstructions.empty())
                {
                    parent.AllQueuedInstructions.emplace(currentLoop.AllQueuedInstructions.front());
                    parent.LoopBody.emplace_back(currentLoop.AllQueuedInstructions.front());

                    currentLoop.AllQueuedInstructions.pop();
                }
                
                currentLoop = parent;
                loopStack.pop();
                
                continue;
            }

            currentLoop.LoopBody.clear();
            while (!currentLoop.AllQueuedInstructions.empty())
            {
                result.emplace_back(currentLoop.AllQueuedInstructions.front());
                currentLoop.AllQueuedInstructions.pop();
            }
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
    //bool negative = ((BFMutatorInstruction*)loopBody.back())->SimpleType == dPtrIncr 
    //        || ((BFMutatorInstruction*)loopBody.front())->SimpleType == dPtrDecr;
    
    //int offsetMax = ((BFMutatorInstruction*)loopBody.back())->Args[0];
    std::vector<BFInstruction *> result = std::vector<BFInstruction *>();
    
    auto iterator = loopBody.begin(), end = loopBody.end();
    
    auto partBegin = BFLoopPatterns.MultiplyLoopPart.begin();
    auto partEnd = BFLoopPatterns.MultiplyLoopPart.end();
    
    for (int64_t off = 0;;)
    {
        iterator = std::search(iterator, loopBody.end(),
                               partBegin, partEnd,
                               InstructionTypeComparator);
        if (iterator == end)
            break;
        
        size_t pos = iterator - loopBody.begin();
        
        int64_t memOffset = ((BFMutatorInstruction*)loopBody[pos])->Args[0];
        int64_t amount = ((BFMutatorInstruction*)loopBody[pos + 1])->Args[0];

        off += memOffset;
        result.emplace_back(new BFMutatorInstruction(MultiplyPtrVal, None, 
                { off, amount }));
        
        iterator++;
    }
    if (result.empty())
        LogWarning("Multiply loop without any valid multiplication actions found!")
    result.emplace_back(new BFInstruction(ClearPtrVal));
    
    return result;
}

void BFOptimizer::OptimizeCode(std::vector<BFInstruction *> &instructions)
{
    LogDebug("Size before optimization: '" + std::to_string(instructions.size()) + "'");
    instructions = OptimizeCode((const std::vector<BFInstruction *>&)instructions);
    LogDebug("Size after optimization: '" + std::to_string(instructions.size()) + "'");
}
