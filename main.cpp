#include <iostream>
//#include <CLI11.hpp>
#include "CLI11.hpp"

#include "BFInstructionType.h"
#include "BFInterpreter/BFInterpreter.h"
#include "Logging.h"

//Max 5mb file size
#define MaxFileSize 5242880

std::string ReadFile(std::string &path)
{
    std::ifstream infile(path);
    std::string result;
    
    if (!infile.good())
        LogFatal("File '" + path + "' cannot be found!", 1);

    infile.seekg(0, std::ios::end);
    size_t fileSize = infile.tellg();
    infile.seekg(0, std::ios::beg);
    
    LogDebug("File size: '" + std::to_string(fileSize) + "'")
    
    fileSize = (fileSize > MaxFileSize ? MaxFileSize : fileSize);
    
    if (fileSize == 0)
        LogFatal("File '" + path + "' is empty!", 1);
    if (fileSize > MaxFileSize)
        LogFatal("File '" + path + "' is too large!", 1);

    result.assign((std::istreambuf_iterator<char>(infile)),
                  std::istreambuf_iterator<char>());
    
    return result;
}

std::vector<BFInstructionType> ParseInstructions(std::string str)
{
    std::vector<BFInstructionType> instructions = std::vector<BFInstructionType>();
    for (char c : str)
    {
        for(int instructionType : BFInstructionTypeList)
        {
            if (instructionType != c)
                continue;
            
            instructions.push_back((BFInstructionType) c);
            break;
        }
    }
    return instructions;
}

int main(int argc, char **argv)
{
    CLI::App app{"BrainFck interpreter"};
    std::vector<BFInstructionType> instructions;

    std::string filename;
    size_t cellCount = 30720;
    bool optimize = false;
    app.add_option("-f,--file", filename, "BrainF*ck file to run");
    app.add_option("-c,--cells", cellCount, "Amount of cells the BrainF*ck environment is allowed to use");
    app.add_flag("-o,--optimize", optimize, "Determines whether the interpreter should optimize the supplied BrainF*ck code");

    CLI11_PARSE(app, argc, argv);
    
    if (filename.empty())
        LogFatal("No file supplied, unable to continue.", 1);
    
    std::string result = ReadFile(filename);
    instructions = ParseInstructions(result);
    
    

    BFInterpreter interpreter(instructions, cellCount);
    interpreter.Run();
    
    return 0;
}