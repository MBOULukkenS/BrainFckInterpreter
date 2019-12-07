//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFOPTIMIZER_H
#define BRAINFCKINTERPRETER_BFOPTIMIZER_H


#include <vector>
#include "../BFInstructionType.h"

class BFOptimizer
{
public:
    static std::vector<BFInstruction*> OptimizeCode(const std::vector<BFInstruction*>& instructions);
    static void OptimizeCode(std::vector<BFInstruction*>& instructions);

private:
    struct BFOptimizationInfo
    {
        BFInstructionType currentType = None;
        size_t amount = 0;
    };
    
    BFOptimizer();
    
    static std::vector<BFInstruction*> Optimize_Contraction(const std::vector<BFInstruction*>& instructions);
    static std::vector<BFInstruction*> Optimize_SimpleLoops(const std::vector<BFInstruction*>& instructions);


};




#endif //BRAINFCKINTERPRETER_BFOPTIMIZER_H
