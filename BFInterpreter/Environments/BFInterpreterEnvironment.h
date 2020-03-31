//
// Created by Stijn on 12/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINTERPRETERENVIRONMENT_H
#define BRAINFCKINTERPRETER_BFINTERPRETERENVIRONMENT_H


#include "BFEnvironment.h"

class BFInterpreterEnvironment : public BFEnvironment
{
public:
    explicit BFInterpreterEnvironment(std::vector<BFInstruction*> instructions, size_t cellAmount = 30720);
    BFInterpreterEnvironment() : BFEnvironment() {}

    size_t InstructionPtr = 0;
    std::vector<BFInstruction*> Instructions;
};


#endif //BRAINFCKINTERPRETER_BFINTERPRETERENVIRONMENT_H
