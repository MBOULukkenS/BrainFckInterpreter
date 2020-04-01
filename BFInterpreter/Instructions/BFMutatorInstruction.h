//
// Created by Stijn on 01/04/2020.
//

#ifndef BRAINFCKINTERPRETER_BFMUTATORINSTRUCTION_H
#define BRAINFCKINTERPRETER_BFMUTATORINSTRUCTION_H

#include <utility>
#include <vector>
#include "BFInstruction.h"

class BFMutatorInstruction : public BFInstruction
{
public:
    explicit BFMutatorInstruction(BFInstructionType type, BFInstructionType simpleType, std::vector<int64_t> args)
            : BFInstruction(type), SimpleType(simpleType), Args(std::move(args)) {}

    explicit BFMutatorInstruction(const BFMutatorInstruction *other) 
            : BFMutatorInstruction(other->InstructionType, other->SimpleType, other->Args) {}

    BFMutatorInstruction(const BFMutatorInstruction &other) 
            : BFMutatorInstruction(other.InstructionType, other.SimpleType, other.Args) {}

    BFInstructionType SimpleType = None;
    std::vector<int64_t> Args;
};

#endif //BRAINFCKINTERPRETER_BFMUTATORINSTRUCTION_H
