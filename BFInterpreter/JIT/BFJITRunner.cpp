//
// Created by Stijn on 12/12/2019.
//

#ifdef USE_JIT

#include <stack>
#include "BFJITRunner.h"
#include "../Instructions/BFLoopInstruction.h"
#include "../../Logging.h"
#include "../Optimizer/BFOptimizer.h"
#include "../Instructions/BFMutatorInstruction.h"

int (*JITPutcharMethod)(int);

BFJITRunner::BFJITRunner(std::vector<BFInstruction *> instructions, size_t cellAmount)
{
    _instructions = instructions;
    _environment = BFEnvironment(cellAmount);
    JITPutcharMethod = putchar;
}

void BFJITRunner::OptimizeInstructions()
{
    BFOptimizer::OptimizeCode(_instructions);
}

void BFJITRunner::CompileAndRun()
{
    asmjit::Error result = Compile();
    if (result != 0)
        LogFatal("Compilation failed!", 1);
    Run();
}

int JITFlushedPutchar(int character)
{
    int result = putchar(character);
    _flushall();
    return result;
}

int DoGetchar()
{
    return getchar();
}

void BFJITRunner::SetFlushType(FlushType value)
{
    JITPutcharMethod = value == DontFlush ? putchar : JITFlushedPutchar;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
asmjit::Error BFJITRunner::Compile()
{
    asmjit::CodeHolder codeHolder;
    codeHolder.init(_runtime.codeInfo());
    
    std::stack<LoopLabelInfo> loopStack = std::stack<LoopLabelInfo>();
    
    asmjit::x86::Compiler compiler(&codeHolder);
    compiler.addFunc(asmjit::FuncSignatureT<void, BFCell*>());

    asmjit::x86::Gp dataPtr = compiler.newUIntPtr("dataPtr");
    
#ifdef LargeAddressAware
    asmjit::x86::Gp tmp = compiler.newUInt16("tmp");
#elif HugeAddressAware
    asmjit::x86::Gp tmp = compiler.newUInt32("tmp");
#else
    asmjit::x86::Gp tmp = compiler.newUInt8("tmp");
#endif
    
    
    compiler.setArg(0, dataPtr);
    
    for (BFInstruction *instruction : _instructions)
    {
        auto *mutInstruction = (BFMutatorInstruction*)instruction;
        
        switch (instruction->InstructionType)
        {
            case dPtrIncr:
                compiler.add(dataPtr, abs(mutInstruction->Args[0]) * CellSize);
                break;
            case dPtrDecr:
                compiler.sub(dataPtr, abs(mutInstruction->Args[0]) * CellSize);
                break;
            case dPtrMod:
                if (mutInstruction->Args[0] == 0)
                {
                    LogWarning("Zero pointer add instruction detected; Skipping");
                    break;
                }
                if (mutInstruction->Args[0] > 0)
                    compiler.add(dataPtr, abs(mutInstruction->Args[0]) * CellSize);
                else if (mutInstruction->Args[0] < 0)
                    compiler.sub(dataPtr, abs(mutInstruction->Args[0]) * CellSize);
                break;
            case ModPtrVal:
                if (mutInstruction->Args[0] == 0)
                {
                    LogWarning("Zero value add instruction detected; Skipping.");
                    break;
                }
                
                if (mutInstruction->Args[0] > 0)
                    compiler.add(asmjit::x86::ptr(dataPtr, 0, CellSize), abs(mutInstruction->Args[0]));
                else if (mutInstruction->Args[0] < 0)
                    compiler.sub(asmjit::x86::ptr(dataPtr, 0, CellSize), abs(mutInstruction->Args[0]));
                break;
            case IncrPtrVal:
                compiler.add(asmjit::x86::ptr(dataPtr, 0, CellSize), abs(mutInstruction->Args[0]));
                break;
            case DecrPtrVal:
                compiler.sub(asmjit::x86::ptr(dataPtr, 0, CellSize), abs(mutInstruction->Args[0]));
                break;
            case MultiplyPtrVal:
                compiler.movzx(tmp, asmjit::x86::ptr(dataPtr, 0, CellSize));
                if (mutInstruction->Args[1] > 1)
                {
                    compiler.imul(tmp, mutInstruction->Args[1]);
                }
                compiler.add(asmjit::x86::ptr(dataPtr, mutInstruction->Args[0] * CellSize, CellSize), tmp);
                break;
            case ClearPtrVal:
                compiler.mov(asmjit::x86::ptr(dataPtr, 0, CellSize), 0);
                break;
            case cWritePtrVal:
            {
                compiler.movzx(tmp, asmjit::x86::ptr(dataPtr, 0, CellSize));
                asmjit::FuncCallNode *externalCall = compiler.call(asmjit::imm(JITPutcharMethod),
                                                                   asmjit::FuncSignatureT<int, int>(
                                                                           asmjit::CallConv::kIdHost));
                externalCall->setArg(0, tmp);
                break;
            }
            case cReadPtrVal:
            {
                asmjit::FuncCallNode *externalCall = compiler.call(asmjit::imm(&getchar),
                                                                   asmjit::FuncSignatureT<int>(
                                                                           asmjit::CallConv::kIdHost));
                externalCall->setRet(0, tmp);
                compiler.mov(asmjit::x86::ptr(dataPtr, 0, CellSize), tmp);
                break;
            }
            case LoopBegin:
            {
                asmjit::Label beginLabel = compiler.newLabel();
                asmjit::Label endLabel = compiler.newLabel();

                compiler.bind(beginLabel);
                compiler.cmp(asmjit::x86::ptr(dataPtr, 0, CellSize), 0);
                compiler.jz(endLabel);
                
                loopStack.push(LoopLabelInfo(beginLabel, endLabel));
                break;
            }
            case LoopEnd:
            {
                if (loopStack.empty())
                    LogFatal("Unclosed loop detected!", -2);

                LoopLabelInfo info = loopStack.top();
                loopStack.pop();
                
                compiler.jmp(info.OpenLabel);
                compiler.bind(info.CloseLabel);
                break;
            }
            case None:
                compiler.nop();
                break;
            default:
                LogFatal("Invalid instruction found!", -2);
        }
    }
    
    compiler.endFunc();
    compiler.finalize();
    
    return _runtime.add(&_bfMain, &codeHolder);
}
#pragma clang diagnostic pop

void BFJITRunner::Run()
{
    if (_bfMain == nullptr)
        LogFatal("No Main function Found!", -3);
    
    _bfMain(_environment.Memory);
}

#endif