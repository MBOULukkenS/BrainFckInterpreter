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

class BFInstruction;

typedef std::pair<size_t, size_t> BFLoop; //BeginPtr, EndPtr

class BFInterpreter
{
public:
    explicit BFInterpreter(const std::vector<BFInstruction*>& instructions, bool flush = false, size_t cellAmount = 30720);
    explicit BFInterpreter(const std::string &instructionsStr, bool flush = false, size_t cellAmount = 30720) 
    : BFInterpreter(BFLoader::ParseInstructions(instructionsStr), flush, cellAmount) {}
    
    void OptimizeInstructions();

    void Run();
    void Step();

    inline bool IsFinished()
    {
        return _bfEnvironment.InstructionPtr <= _instructions.size();
    }
protected:
    explicit BFInterpreter(size_t cellAmount);
    explicit BFInterpreter() : BFInterpreter(30720) 
    {}

    BFInterpreterEnvironment _bfEnvironment;
private:
    std::vector<BFInstruction*> _instructions;
};


#endif //BRAINFCKINTERPRETER_BFINTERPRETER_H
