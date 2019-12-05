//
// Created by Stijn on 04/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINTERPRETER_H
#define BRAINFCKINTERPRETER_BFINTERPRETER_H


#include <vector>
#include <map>
#include "../BFInstructionType.h"
#include "../BFDefines.h"

#define CellSize 8
typedef std::pair<size_t, size_t> BFLoop; //BeginPtr, EndPtr

class BFInterpreter
{
public:
    BFInterpreter(std::vector<BFInstructionType>& instructions, size_t cellAmount = 30720);
    
    void Run();
    void Step();
private:
    void BuildLoopInfo();

    std::map<size_t, size_t> _loopInfo;
    std::map<size_t, size_t> _loopInfoReversed;
    
    size_t instruction_ptr = 0;
    bool _running;
    size_t _cellAmount;
    
    BFCell *_memory;
    size_t _dataPtr = 0;
};


#endif //BRAINFCKINTERPRETER_BFINTERPRETER_H
