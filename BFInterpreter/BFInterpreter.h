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
    explicit BFInterpreter(const std::vector<BFInstruction*>& instructions, bool flush = false, bool enableDump = false, size_t cellAmount = 30720);
    explicit BFInterpreter(const std::string &instructionsStr, bool flush = false, bool enableDump = false, size_t cellAmount = 30720) 
    : BFInterpreter(BFLoader::ParseInstructions(instructionsStr), flush, enableDump, cellAmount) {}

    void Run() override;

    __forceinline void Step();
protected:
    BFInterpreterEnvironment _bfEnvironment;
    
    void DoCrashDump();
    void PrintInstructionTrace(size_t instructionAmount = 5);
};


#endif //BRAINFCKINTERPRETER_BFINTERPRETER_H
