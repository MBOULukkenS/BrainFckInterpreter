//
// Created by Stijn on 12/12/2019.
//

#include <stack>
#include <cmath>
#include "BFJITRunner.h"
#include "../Instructions/BFLoopInstruction.h"
#include "../../Logging.h"
#include "../Optimizer/BFOptimizer.h"
#include "../Instructions/BFMutatorInstruction.h"

int FindExponent(int value, int multiple, int &remainder)
{
    remainder = 0;
    
    if (value != multiple && (value % multiple != 0 || (value / multiple) % multiple != 0))
        return -1;

    int result;
    int valueTemp = value;

    for (result = 1; valueTemp != multiple; result++)
    {
        valueTemp /= multiple;

        if (valueTemp < multiple || valueTemp % multiple != 0)
        {
            remainder = valueTemp;
            break;
        }
    }

    return result;
}

BFCell MultiplyUnderflowLoop(BFCell currentCellValue, int64_t underflowCount)
{
    int max = std::numeric_limits<uint8_t>::max() + 1;

    return std::ceil(max * ((double)underflowCount - currentCellValue) / (double)underflowCount);
}

BFJITRunner::BFJITRunner(const std::vector<BFInstruction *>& instructions, bool flush, size_t cellAmount) 
: BFRunner(instructions, flush)
{
    _instructions = instructions;
    _environment = new BFEnvironment(cellAmount);
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
    DoRun();
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
    asmjit::x86::Gp tmpBig = compiler.newInt64("tmpBig");
    
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
            case MultiplyPtrVal:
            {
                int64_t multiplier = mutInstruction->Args[1];
                int64_t predictedUnderflows = mutInstruction->Args[2];
                
                compiler.movzx(tmp, asmjit::x86::ptr(dataPtr, 0, CellSize));
                if (multiplier > 1)
                {
                    if (predictedUnderflows > 1)
                    {
                        compiler.mov(tmpBig, mutInstruction->Args[2]);
                        asmjit::FuncCallNode *externalCall = compiler.call(asmjit::imm(&MultiplyUnderflowLoop),
                                                                           asmjit::FuncSignatureT<BFCell, BFCell, int64_t>(
                                                                                   asmjit::CallConv::kIdHost));
                        externalCall->setArg(0, tmp);
                        externalCall->setArg(1, tmpBig);

                        externalCall->setRet(0, tmp);
                    }
                    
                    int remainder;
                    int exponent = FindExponent(multiplier, 2, remainder);
                    
                    if (exponent <= 0)
                        compiler.imul(tmp, (remainder > 0 ? remainder : multiplier));
                    
                    if (exponent > 1) // if multiplier is a nth power of two...
                        compiler.sal(tmp, exponent); //... use a shift left instead of an imul
                    else if (exponent > 0)
                        compiler.add(tmp, tmp); //if exponent is 1, square tmp instead of multiplying
                }
                compiler.add(asmjit::x86::ptr(dataPtr, mutInstruction->Args[0] * CellSize, CellSize), tmp);
                break;
            }
            case ClearPtrVal:
                compiler.mov(asmjit::x86::ptr(dataPtr, 0, CellSize), 0);
                break;
            case cWritePtrVal:
            {
                compiler.movzx(tmp, asmjit::x86::ptr(dataPtr, 0, CellSize));
                asmjit::FuncCallNode *externalCall = compiler.call(asmjit::imm(PutcharMethod),
                                                                   asmjit::FuncSignatureT<void, int>(
                                                                           asmjit::CallConv::kIdHost));
                externalCall->setArg(0, tmp);
                break;
            }
            case cReadPtrVal:
            {
                asmjit::FuncCallNode *externalCall = compiler.call(asmjit::imm(&getchar),
                                                                   asmjit::FuncSignatureT<void>(
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
                    LogFatal("Unopened loop detected!", -2);

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
    
    if (!loopStack.empty())
        LogFatal("Unclosed loop detected!", -2);
    
    compiler.endFunc();
    compiler.finalize();
    
    return _runtime.add(&_bfMain, &codeHolder);
}
#pragma clang diagnostic pop

void BFJITRunner::Run()
{
    CompileAndRun();
}

void BFJITRunner::DoRun()
{
    if (_bfMain == nullptr)
        LogFatal("No Main function Found!", -3);

    if (_instructions.empty())
        LogFatal("No instructions to execute!", -1);

    _bfMain(_environment->Memory);
}
