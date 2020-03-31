//
// Created by Stijn on 04/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFINSTRUCTIONTYPE_H
#define BRAINFCKINTERPRETER_BFINSTRUCTIONTYPE_H

enum BFInstructionType
{
    None = -1,
    dPtrIncr = '>', //increment pointer
    dPtrDecr = '<', //decrement pointer
    IncrPtrVal = '+', //increment value behind pointer
    DecrPtrVal = '-', //decrement value behind pointer
    cWritePtrVal = '.', //write value behind pointer to console
    cReadPtrVal = ',', //read value from console and write to value behind pointer
    LoopBegin = '[', //begin a loop
    LoopEnd = ']', //end the loop
    
    //Optimizer instructions
    ClearPtrVal,
    ModPtrVal,
    dPtrMod,
};

const constexpr BFInstructionType BFInstructionTypeList[] = {dPtrIncr, dPtrDecr, IncrPtrVal, DecrPtrVal, cWritePtrVal, cReadPtrVal, LoopBegin, LoopEnd };

#endif //BRAINFCKINTERPRETER_BFINSTRUCTIONTYPE_H
