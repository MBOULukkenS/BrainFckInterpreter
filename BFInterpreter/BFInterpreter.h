//
// Created by Stijn on 04/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINTERPRETER_H
#define BRAINFCKINTERPRETER_BFINTERPRETER_H


#include <vector>
#include <array>
#include <map>

#include "../BFDefines.h"
#include "Environments/BFInterpreterEnvironment.h"
#include "BFLoader.h"
#include "BFRunner.h"

class BFInstruction;

class BFInterpreter : public BFRunner
{
public:
    explicit BFInterpreter(const std::vector<BFInstruction*>& instructions, bool flush = false, size_t cellAmount = 30720);
    explicit BFInterpreter(const std::string &instructionsStr, bool flush = false, size_t cellAmount = 30720) 
    : BFInterpreter(BFLoader::ParseInstructions(instructionsStr), flush, cellAmount) {}

    void Run() override;
    void Step();
protected:
    BFInterpreterEnvironment _bfEnvironment;
};


#endif //BRAINFCKINTERPRETER_BFINTERPRETER_H
