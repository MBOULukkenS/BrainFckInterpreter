//
// Created by Stijn on 12/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFLOADER_H
#define BRAINFCKINTERPRETER_BFLOADER_H

#include <string>
#include <vector>
#include "BFTokenInfo.h"

class BFInstruction;

class BFLoader
{
public:
    static std::vector<BFInstruction *> ParseInstructions(const std::string &instructionsStr);
    static std::vector<BFInstruction *> ParseInstructions(const std::string &instructionsStr, const BFTokenInfo &tokenInfo);
    
    static void ConvertToDialect(const std::vector<BFInstruction*> &instructions, const BFTokenInfo &outputFormat, std::ostream &outputStream);
    
    static void ExportInstructions(const std::vector<BFInstruction*> &instructions, std::ostream &output, bool lineNumbers = false);
    static void BuildLoopInfo(std::vector<BFInstruction*> &instructions);
};


#endif //BRAINFCKINTERPRETER_BFLOADER_H
