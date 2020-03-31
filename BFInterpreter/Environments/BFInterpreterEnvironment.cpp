//
// Created by Stijn on 12/12/2019.
//

#include <stack>
#include <string>

#include "../../Logging.h"

#include "../BFInstruction.h"
#include "../BFLoopInstruction.h"
#include "BFInterpreterEnvironment.h"

BFInterpreterEnvironment::BFInterpreterEnvironment(std::vector<BFInstruction *> instructions, size_t cellAmount)
        : BFEnvironment(cellAmount)
{
    Instructions = instructions;
}
