//
// Created by Stijn on 05/12/2019.
//

#include <cstdlib>
#include "BFEnvironment.h"

BFEnvironment::BFEnvironment(size_t cellAmount)
{
    CellAmount = cellAmount;
    Memory = (BFCell *)calloc(cellAmount, sizeof(BFCell));
}
