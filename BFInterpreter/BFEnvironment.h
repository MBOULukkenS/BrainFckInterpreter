//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFENVIRONMENT_H
#define BRAINFCKINTERPRETER_BFENVIRONMENT_H


#include "../BFDefines.h"
#include <cstdlib>

class BFEnvironment
{

public:
    explicit BFEnvironment(size_t cellAmount = 30270);
    
    BFCell *Memory;
    size_t DataPtr = 0;
    size_t InstructionPtr = 0;
    
    size_t CellAmount;
private:
    
};


#endif //BRAINFCKINTERPRETER_BFENVIRONMENT_H
