//
// Created by Stijn on 05/12/2019.
//

#include <cstdlib>
#include <stack>
#include <string>

#include "BFEnvironment.h"
#include "../Instructions/BFLoopInstruction.h"
#include "../../Logging.h"

BFEnvironment::BFEnvironment(size_t cellAmount)
{
    CellAmount = cellAmount;
    PtrMaxOffset = cellAmount * CellSize;
    
    Memory = (BFCell *)calloc(cellAmount, CellSize);
    CurrentCell = Memory;
}
