//
// Created by Stijn on 12/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFJITRUNNER_H
#define BRAINFCKINTERPRETER_BFJITRUNNER_H

#ifdef USE_JIT

#include <cstdlib>
#include <vector>
#include "../BFInstruction.h"
#include "../Environments/BFEnvironment.h"

#include "../../asmjit/src/asmjit/asmjit.h"

enum FlushType
{
    DontFlush = 0,
    DoFlush
};

class BFJITRunner
{
public:
    BFJITRunner(std::vector<BFInstruction*> instructions, size_t cellAmount);
    
    void CompileAndRun();

    void OptimizeInstructions();
    void SetFlushType(FlushType value);
    
    asmjit::Error Compile(std::vector<BFInstruction*> instructions);
    asmjit::Error Compile();
    
    void Run();
private:
    typedef void (*BFMain)(BFCell *memoryPtr);

    struct LoopLabelInfo {
        LoopLabelInfo(const asmjit::Label& openLabel, const asmjit::Label& closeLabel)
                : OpenLabel(openLabel), CloseLabel(closeLabel) {}

        asmjit::Label OpenLabel;
        asmjit::Label CloseLabel;
    };
    
    FlushType _flushType;
    
    std::vector<BFInstruction*> _instructions;
    BFEnvironment _environment;
    
    asmjit::JitRuntime _runtime;
    BFMain _bfMain = nullptr;
};

#endif

#endif //BRAINFCKINTERPRETER_BFJITRUNNER_H
