//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFDEFINES_H
#define BRAINFCKINTERPRETER_BFDEFINES_H

#include <stdint.h>

#define DumpPath "MemoryDumps/"
#define CrashDumpName "CrashDump"

#ifdef LargeAddressAware
typedef uint16_t BFCell; //16-bit cell
typedef int16_t SignedBFCell;
#elif HugeAddressAware
typedef uint32_t BFCell; //32-bit cell
typedef int32_t SignedBFCell;
#else
typedef uint8_t BFCell; //8-bit cell
typedef char SignedBFCell;
#endif

#define CellSize sizeof(BFCell)


#endif //BRAINFCKINTERPRETER_BFDEFINES_H
