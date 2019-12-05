//
// Created by Stijn on 04/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINTERPRETER_H
#define BRAINFCKINTERPRETER_BFINTERPRETER_H


#include <vector>
#include <map>
#include "../BFInstructionType.h"
#include "../BFDefines.h"
#include "BFEnvironment.h"

#define CellSize 8
typedef std::pair<size_t, size_t> BFLoop; //BeginPtr, EndPtr

class BFInterpreter
{
public:
    explicit BFInterpreter(std::vector<BFInstructionType>& instructions, size_t cellAmount = 30720);
    
    void Run();
    void Step();
private:
    void BuildLoopInfo();

    std::map<size_t, size_t> _loopInfo;
    std::map<size_t, size_t> _loopInfoReversed;
    
    BFEnvironment _bfEnvironment;
    
    bool _running;
};


#endif //BRAINFCKINTERPRETER_BFINTERPRETER_H
