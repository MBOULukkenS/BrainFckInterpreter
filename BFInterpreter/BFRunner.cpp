//
// Created by Stijn on 07/04/2020.
//

#include "../HexDump.h"
#include "BFRunner.h"
#include "BFLoader.h"
#include "Optimizer/BFOptimizer.h"
#include "../Util.h"
#include "../Logging.h"

int dumpIndex = 0;

void BFRunner::OptimizeInstructions()
{
    BFOptimizer::OptimizeCode(_instructions);
    BFLoader::BuildLoopInfo(_instructions);
}

void BFRunner::PutcharFlushed(int ch)
{
    putchar(ch);
    _flushall();
}

void BFRunner::Putchar(int ch)
{
    putchar(ch);
}

void BFRunner::DoMemoryDump()
{
    if (!DoMemoryDump(DumpName + std::to_string(dumpIndex)))
    {
        LogError("Unable to create Dump File!");
        return;
    }
    
    dumpIndex++;
}

bool BFRunner::DoMemoryDump(const std::string &name)
{
    if (!EnableDump)
        return false;

    std::ofstream outputStream;
    outputStream.open(name + DumpExtension);
    
    if (!outputStream.is_open())
        return false;

    HexDump::DumpCharArray(outputStream, (char*)_environment->Memory, _environment->CellAmount);

    outputStream.close();
    return true;
}
