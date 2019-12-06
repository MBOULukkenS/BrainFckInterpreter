//
// Created by Stijn on 04/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINTERPRETER_H
#define BRAINFCKINTERPRETER_BFINTERPRETER_H


#include <vector>
#include <map>

#include "../BFDefines.h"
#include "BFEnvironment.h"

class BFInstruction;

typedef std::pair<size_t, size_t> BFLoop; //BeginPtr, EndPtr

class BFInterpreter
{
public:
    explicit BFInterpreter(std::vector<BFInstruction>& instructions, size_t cellAmount = 30720);

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

    std::map<size_t, size_t> _loopInfo;
    std::map<size_t, size_t> _loopInfoReversed;

    BFEnvironment _bfEnvironment;
private:
    std::vector<BFInstruction> _instructions;
    
    void BuildLoopInfo();
    void Construct(size_t cellAmount = 30720);
};


#endif //BRAINFCKINTERPRETER_BFINTERPRETER_H
