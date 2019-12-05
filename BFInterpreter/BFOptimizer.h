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
    std::vector<BFInstructionType> OptimizeCode(std::vector<BFInstructionType> instructions);
};


#endif //BRAINFCKINTERPRETER_BFOPTIMIZER_H
