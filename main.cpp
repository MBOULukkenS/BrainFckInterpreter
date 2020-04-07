#include <iostream>
#include "CLI11.hpp"

#include "BFInterpreter/Instructions/BFInstruction.h"
#include "BFInterpreter/BFInterpreter.h"
#include "BFInterpreter/JIT/BFJITRunner.h"
#include "Logging.h"
#include "BFInterpreter/Optimizer/BFOptimizer.h"
#include "Stopwatch.h"

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
    
    infile.close();
    return result;
}

int main(int argc, char **argv)
{
    //region App Description
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
    //endregion

    //region App parameters

    CLI::App app{appDescription};
    Stopwatch::Stopwatch stopwatch;

    std::string filename;
    size_t cellCount = 30720;
    bool optimize = false;
    bool flush = false;
    bool useStopwatch = false;
    bool jit = false;

    //endregion

    //region App flags

    app.add_option("-f,--file", filename, "BrainF*ck file to run");
    app.add_option("-c,--cells", cellCount, "Amount of cells the BrainF*ck environment is allowed to use");
    app.add_flag("-o,--optimize", optimize,
                 "Determines whether the interpreter should optimize the supplied BrainF*ck code");
    app.add_flag("-F,--flush", flush,
                 "Determines whether a flush to stdout should occur after each BrainF*ck 'putchar' command.");
    app.add_flag("-j,--jit", jit, "Determines whether the supplied BrainF*ck code should be compiled or interpreted.");
    app.add_flag("-t,--time", useStopwatch,
                 "Whether the time the program takes to finish should be recorded and displayed");
#ifdef DEBUG
    bool outputInstructions = false;
    app.add_flag("-x,--output-instructions", outputInstructions,
                 "output the loaded code to stdout instead of executing it.");
#endif

    //endregion

    CLI11_PARSE(app, argc, argv);

    if (filename.empty())
        LogFatal("No file supplied, unable to continue.", 1);

    std::string instructionsStr = ReadFile(filename);
    std::vector<BFInstruction *> instructions = BFLoader::ParseInstructions(instructionsStr);

    //region Debug Output
#ifdef DEBUG
    if (outputInstructions)
    {
        if (optimize)
            BFOptimizer::OptimizeCode(instructions);

        BFLoader::BuildLoopInfo(instructions);
        BFLoader::ExportInstructions(instructions, std::cout, true);
        return 0;
    }
#endif
    //endregion

    BFRunner *runner = (jit
                        ? (BFRunner *) new BFJITRunner(instructions, flush, cellCount)
                        : (BFRunner *) new BFInterpreter(instructions, flush, cellCount));

    if (jit)
        LogMessage("JIT Enabled")
    
    if (optimize)
    {
        LogMessage("Optimization: ON");
        
        if (useStopwatch)
            stopwatch.Start();
        
        runner->OptimizeInstructions();
        
        if (useStopwatch)
            LogMessage("Optimization finished in <" + std::to_string(stopwatch.Elapsed<Stopwatch::milliseconds>()) + "ms>")
    }
    
    if (useStopwatch)
        stopwatch.Start();

    std::cout << std::string(27, '-') << "< Output >" << std::string(27, '-') << std::endl;
    runner->Run();
    std::cout << std::endl << std::string(64, '-') << std::endl;

    if (useStopwatch)
        LogMessage("Program finished in <" + std::to_string(stopwatch.Elapsed<Stopwatch::milliseconds>()) + "ms>")

    return 0;
}
#pragma clang diagnostic pop