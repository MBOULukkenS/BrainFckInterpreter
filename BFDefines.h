//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFDEFINES_H
#define BRAINFCKINTERPRETER_BFDEFINES_H


#ifdef LargeAddressAware
typedef int BFCell; //16-bit cell
#elif HugeAddressAware
typedef long BFCell; //32-bit cell
#else
typedef char BFCell; //8-bit cell
#endif


#endif //BRAINFCKINTERPRETER_BFDEFINES_H
