//
// Created by Stijn on 12/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFJITRUNNER_H
#define BRAINFCKINTERPRETER_BFJITRUNNER_H

#include <cstdlib>
#include <vector>
#include "../Instructions/BFInstruction.h"
#include "../Environments/BFEnvironment.h"

#include "../../asmjit/src/asmjit/asmjit.h"
#include "../BFRunner.h"

enum FlushType
{
    DontFlush = 0,
    DoFlush
};

class BFJITRunner : public BFRunner
{
public:
    BFJITRunner(const std::vector<BFInstruction*> &instructions, bool flush, size_t cellAmount);
    
    void CompileAndRun();
    
    asmjit::Error Compile();
    void Run() override;
    
    void OptimizeInstructions() override;
private:
    typedef void (*BFMain)(BFCell *memoryPtr);

    struct LoopLabelInfo {
        LoopLabelInfo(const asmjit::Label& openLabel, const asmjit::Label& closeLabel)
                : OpenLabel(openLabel), CloseLabel(closeLabel) {}

        asmjit::Label OpenLabel;
        asmjit::Label CloseLabel;
    };
    
    asmjit::JitRuntime _runtime;
    BFMain _bfMain = nullptr;
    
    void DoRun();
};

#endif //BRAINFCKINTERPRETER_BFJITRUNNER_H
