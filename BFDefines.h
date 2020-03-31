//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFDEFINES_H
#define BRAINFCKINTERPRETER_BFDEFINES_H

#include <stdint.h>

#ifdef LargeAddressAware
typedef unsigned short BFCell; //16-bit cell
typedef short SignedBFCell;
#elif HugeAddressAware
typedef unsigned int BFCell; //32-bit cell
typedef int SignedBFCell;
#else
typedef unsigned char BFCell; //8-bit cell
typedef char SignedBFCell;
#endif


#endif //BRAINFCKINTERPRETER_BFDEFINES_H
