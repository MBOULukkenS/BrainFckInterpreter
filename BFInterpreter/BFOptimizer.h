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
    struct BFContractionOptimizationInfo
    {
        BFInstructionType currentType = None;
        int64_t amount = 0;
    };
    struct BFSimpleLoopOptimizationInfo
    {
        enum LoopType
        {
            ScanLeftLoop,
            ScanRightLoop,

            ClearLoop
        };
        
        LoopType loopType;
        size_t position = 0;
    };
    
    BFOptimizer();
    
    static void Optimize_Contraction(std::vector<BFInstruction*>& instructions);
    static void Optimize_SimpleLoops(std::vector<BFInstruction*>& instructions);


};




#endif //BRAINFCKINTERPRETER_BFOPTIMIZER_H
