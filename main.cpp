#include <iostream>
#include "CLI11.hpp"

#include "BFInterpreter/Instructions/BFInstruction.h"
#include "BFInterpreter/BFInterpreter.h"
#include "Logging.h"
#include "BFInterpreter/BFOptimizer.h"

#define OptimizeOnMessage "Optimization: ON"

#ifdef USE_STOPWATCH
#include "Stopwatch.h"
#endif

#ifdef USE_JIT
#include "BFInterpreter/JIT/BFJITRunner.h"
#endif

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
    bool flush = false;
    
    app.add_option("-f,--file", filename, "BrainF*ck file to run");
    app.add_option("-c,--cells", cellCount, "Amount of cells the BrainF*ck environment is allowed to use");
    app.add_flag("-o,--optimize", optimize, "Determines whether the interpreter should optimize the supplied BrainF*ck code");
    app.add_flag("-F,--flush", flush, "Determines whether a flush to stdout should occur after each BrainF*ck 'putchar' command.");
#ifdef USE_JIT
    bool jit = false;
    app.add_flag("-j,--jit", jit, "Determines whether the supplied BrainF*ck code should be compiled or interpreted.");
#endif
#ifdef USE_STOPWATCH
    bool useStopwatch = false;
    app.add_flag("-t,--time", useStopwatch, "Whether the time the program takes to finish should be recorded and displayed");
    Stopwatch::Stopwatch stopwatch;
#endif

    CLI11_PARSE(app, argc, argv);
    
    if (filename.empty())
        LogFatal("No file supplied, unable to continue.", 1);
    
    std::string instructionsStr = ReadFile(filename);

#ifdef USE_JIT
    if (jit)
    {
#if USE_STOPWATCH
        stopwatch.Start();
#endif
        
        BFJITRunner jitRunner(BFLoader::ParseInstructions(instructionsStr), cellCount);
        if (optimize)
        {
            LogMessage(OptimizeOnMessage);
            jitRunner.OptimizeInstructions();
        }
        
        if (flush)
            jitRunner.SetFlushType(DoFlush);
        
        asmjit::Error result = jitRunner.Compile();
        if (result != 0)
            LogFatal("Compilation failed!", -3);
        
#if USE_STOPWATCH
        if (useStopwatch)
            LogMessage("Optimization & Compilation finished in <" + std::to_string(stopwatch.Elapsed<Stopwatch::milliseconds>()) + "ms>")
#endif

#if USE_STOPWATCH
        stopwatch.Start();
#endif
        
        std::cout << std::string(25, '-') << "< JIT Output >" << std::string(25, '-') << std::endl;
        jitRunner.Run();
        std::cout << std::endl << std::string(64, '-') << std::endl;
        
#if USE_STOPWATCH
        if (useStopwatch)
            LogMessage("Program finished in <" + std::to_string(stopwatch.Elapsed<Stopwatch::milliseconds>()) + "ms>")
#endif
        return 0;
    }
#endif
    BFInterpreter interpreter(instructionsStr, flush, cellCount);
    if (optimize)
    {
        LogMessage(OptimizeOnMessage);
        interpreter.OptimizeInstructions();
    }
#if USE_STOPWATCH
    stopwatch.Start();
#endif

    std::cout << std::string(27, '-') << "< Output >" << std::string(27, '-') << std::endl;
    interpreter.Run();
    std::cout << std::endl << std::string(64, '-') << std::endl;
    
#if USE_STOPWATCH
    if (useStopwatch)
        LogMessage("Program finished in <" + std::to_string(stopwatch.Elapsed<Stopwatch::milliseconds>()) + "ms>")
#endif
    
    return 0;
}
#pragma clang diagnostic pop