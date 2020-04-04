//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFOPTIMIZER_H
#define BRAINFCKINTERPRETER_BFOPTIMIZER_H


#include <vector>
#include <queue>
#include "../../BFInstructionType.h"

class BFOptimizer
{
public:
    static std::vector<BFInstruction*> OptimizeCode(const std::vector<BFInstruction*>& instructions);
    static void OptimizeCode(std::vector<BFInstruction*>& instructions);

private:
    BFOptimizer();
    
    struct LoopOptimizeInfo {
        std::queue<BFInstruction*> AllQueuedInstructions = std::queue<BFInstruction*>();
        std::vector<BFInstruction*> LoopBody = std::vector<BFInstruction*>();
        std::vector<BFInstruction*> Output = std::vector<BFInstruction*>();
    };
    
    static void Optimize_Contraction(std::vector<BFInstruction*>& instructions);
    static void Optimize_Loops(std::vector<BFInstruction*>& instructions);
};




#endif //BRAINFCKINTERPRETER_BFOPTIMIZER_H
