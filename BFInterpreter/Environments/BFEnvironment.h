//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFENVIRONMENT_H
#define BRAINFCKINTERPRETER_BFENVIRONMENT_H


#include "../../BFDefines.h"
#include <cstdlib>
#include <vector>

class BFInstruction;

class BFEnvironment
{

public:
    explicit BFEnvironment(size_t cellAmount = 30270);
    
    BFCell *Memory;
    BFCell *CurrentCell; //size_t DataPtr = 0;
   
    size_t PtrMaxOffset;
    size_t CellAmount;
    
    size_t MemoryDumpIndex = 0;
private:
    
};


#endif //BRAINFCKINTERPRETER_BFENVIRONMENT_H
