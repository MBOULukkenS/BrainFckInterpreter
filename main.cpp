#include <iostream>
#include "CLI11.hpp"

#include "BFInterpreter/BFInstruction.h"
#include "BFInterpreter/BFInterpreter.h"
#include "Logging.h"
#include "BFInterpreter/BFOptimizer.h"

//Max 5mb file size
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"
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

int main(int argc, char **argv)
{
    std::string appDescription("BrainF*ck interpreter");
    
#ifdef LargeAddressAware
    appDescription.append(" (16-bit cells)");
#elif HugeAddressAware
    appDescription.append(" (32-bit cells)");
#else
    appDescription.append(" (8-bit cells)");
#endif

#ifdef _MSC_VER
    appDescription.append(" {Compiled using MSVC++}");
#elif __GNUC__
    appDescription.append(" {Compiled using G++}");
#endif    
    
    CLI::App app { appDescription };

    std::string filename;
    size_t cellCount = 30720;
    bool optimize = false;
    
    app.add_option("-f,--file", filename, "BrainF*ck file to run");
    app.add_option("-c,--cells", cellCount, "Amount of cells the BrainF*ck environment is allowed to use");
    app.add_flag("-o,--optimize", optimize, "Determines whether the interpreter should optimize the supplied BrainF*ck code");

    CLI11_PARSE(app, argc, argv);
    
    if (filename.empty())
        LogFatal("No file supplied, unable to continue.", 1);
    
    std::string instructionsStr = ReadFile(filename);        
    
    BFInterpreter interpreter(instructionsStr, cellCount);
    if (optimize)
    {
        LogMessage("Optimization: ON");
        interpreter.OptimizeInstructions();
    }
    
    interpreter.Run();
    
    return 0;
}
#pragma clang diagnostic pop