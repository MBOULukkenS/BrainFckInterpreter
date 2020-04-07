//
// Created by Stijn on 07/04/2020.
//

#include "BFRunner.h"
#include "BFLoader.h"
#include "Optimizer/BFOptimizer.h"

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
