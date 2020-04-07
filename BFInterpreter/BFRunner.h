//
// Created by Stijn on 07/04/2020.
//

#ifndef BRAINFCKINTERPRETER_BFRUNNER_H
#define BRAINFCKINTERPRETER_BFRUNNER_H


#include <utility>
#include <vector>
#include "Instructions/BFInstruction.h"
#include "Environments/BFEnvironment.h"

class BFRunner
{
public:
    BFRunner(std::vector<BFInstruction*> instructions, bool flush)
    {
        _instructions = std::move(instructions);
        if (flush)
            PutcharMethod = BFRunner::PutcharFlushed;
        else
            PutcharMethod = BFRunner::Putchar;
    }
    
    virtual void OptimizeInstructions();
    virtual void Run() = 0;

protected:
    void (*PutcharMethod)(int);
    
    std::vector<BFInstruction*> _instructions;
    BFEnvironment *_environment {};
    
    static void PutcharFlushed(int ch);
    static void Putchar(int ch);
};

#endif //BRAINFCKINTERPRETER_BFRUNNER_H
