//
// Created by Stijn on 12/12/2019.
//

#ifndef BRAINFCKINTERPRETER_BFLOADER_H
#define BRAINFCKINTERPRETER_BFLOADER_H

#include <string>
#include <vector>

class BFInstruction;

class BFLoader
{
public:
    static std::vector<BFInstruction *> ParseInstructions(const std::string &instructionsStr);
    static void ExportInstructions(const std::vector<BFInstruction*> &instructions, std::ostream &output, bool lineNumbers = false);
    static void BuildLoopInfo(std::vector<BFInstruction*> &instructions);
};


#endif //BRAINFCKINTERPRETER_BFLOADER_H
