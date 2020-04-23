#include <iostream>
#include <fstream>
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

struct {
    std::string Filename;
    std::string SyntaxSwapFilename;
    size_t CellCount = 30720;
    bool Optimize = false;
    bool Flush = false;
    bool UseStopwatch = false;
    bool UseJIT = false;
    bool EnableDump = false;
#ifdef DEBUG
    bool OutputInstructions = false;
#endif
} RunnerOptions;

struct {
    std::string InputConfigPath;
    std::string OutputConfigPath;
    std::string InputPath;
    std::string OutputPath;
} ConverterOptions;

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

void ConverterCallback()
{    
    bool outputToFile = !ConverterOptions.OutputPath.empty();
    
    std::ofstream outputFile;
    if (outputToFile)
        outputFile.open(ConverterOptions.OutputPath);

    std::string input = ReadFile(ConverterOptions.InputPath);
    
    BFTokenInfo inputTokenInfo = ConverterOptions.InputConfigPath.empty() 
            ? BFTokenInfo::Default() 
            : BFTokenInfo(ConverterOptions.InputConfigPath);
    
    BFTokenInfo outputTokenInfo = BFTokenInfo(ConverterOptions.OutputConfigPath);
    
    std::vector<BFInstruction*> instructions = BFLoader::ParseInstructions(input, inputTokenInfo);
    BFLoader::ConvertToDialect(instructions, outputTokenInfo, outputToFile ? outputFile : std::cout);
}

void RunnerCallback()
{
    Stopwatch::Stopwatch stopwatch;
    BFTokenInfo tokenInfo = BFTokenInfo::Default();

    if (!RunnerOptions.SyntaxSwapFilename.empty())
        tokenInfo = BFTokenInfo(RunnerOptions.SyntaxSwapFilename);

    std::string instructionsStr = ReadFile(RunnerOptions.Filename);
    std::vector<BFInstruction *> instructions = BFLoader::ParseInstructions(instructionsStr, tokenInfo);

    //region Debug Output
#ifdef DEBUG
    if (RunnerOptions.OutputInstructions)
    {
        if (RunnerOptions.Optimize)
            BFOptimizer::OptimizeCode(instructions);

        BFLoader::BuildLoopInfo(instructions);
        BFLoader::ExportInstructions(instructions, std::cout, true);
        return;
    }
#endif
    //endregion

    BFRunner *bfRunner = (RunnerOptions.UseJIT
                          ? (BFRunner *) new BFJITRunner(instructions, RunnerOptions.Flush, RunnerOptions.EnableDump, RunnerOptions.CellCount)
                          : (BFRunner *) new BFInterpreter(instructions, RunnerOptions.Flush, RunnerOptions.EnableDump, RunnerOptions.CellCount));

    if (RunnerOptions.UseJIT)
        LogMessage("JIT Enabled")

    if (RunnerOptions.Optimize)
    {
        LogMessage("Optimization: ON");

        if (RunnerOptions.UseStopwatch)
            stopwatch.Start();

        bfRunner->OptimizeInstructions();

        if (RunnerOptions.UseStopwatch)
            LogMessage("Optimization finished in <" + std::to_string(stopwatch.Elapsed<Stopwatch::milliseconds>()) + "ms>");
    }

    if (RunnerOptions.UseStopwatch)
        stopwatch.Start();

    std::cout << std::string(27, '-') << "< Output >" << std::string(27, '-') << std::endl;
    bfRunner->Run();
    std::cout << std::endl << std::string(64, '-') << std::endl;

    if (RunnerOptions.UseStopwatch)
        LogMessage("Program finished in <" + std::to_string(stopwatch.Elapsed<Stopwatch::milliseconds>()) + "ms>");

    if (RunnerOptions.EnableDump)
        bfRunner->DoMemoryDump();
}

int main(int argc, char **argv)
{
    //region App Description
    std::string appDescription("BrainF*ck interpreter");
    appDescription.append("\n");
    
    appDescription.append("\nCell size: ");
#ifdef LargeAddressAware
    appDescription.append("[16-bit]");
#elif HugeAddressAware
    appDescription.append("[32-bit]");
#else
    appDescription.append("[8-bit]");
#endif

    appDescription.append("\nCompiled using: ");
    
#ifdef _MSC_VER
    appDescription.append("[MSVC]");
#elif __GNUC__
    appDescription.append("[GCC]");
#endif
    
    appDescription.append("\n");
    //endregion

    //region App parameters

    CLI::App app{appDescription};
    app.require_subcommand();
    app.ignore_case();
    
    CLI::App *converter = app.add_subcommand("Convert", "brainf*ck dialect/syntax swap converter");
    CLI::App *runner = app.add_subcommand("Run", "brainf*ck runner");

    //endregion

    //region Runner options
    
    runner->callback(RunnerCallback);

    runner->add_option("file", RunnerOptions.Filename, "BrainF*ck file to run")->required(true);
    runner->add_option("-i,--instructions-config", RunnerOptions.SyntaxSwapFilename, "BrainF*ck syntax swap configuration file");
    runner->add_option("-c,--cells", RunnerOptions.CellCount, "Amount of cells the BrainF*ck environment is allowed to use");
    runner->add_flag("-o,--optimize", RunnerOptions.Optimize,
                 "Determines whether the interpreter should optimize the supplied BrainF*ck code");
    runner->add_flag("-F,--flush", RunnerOptions.Flush,
                 "Determines whether a flush to stdout should occur after each BrainF*ck 'putchar' command.");
    runner->add_flag("-j,--jit", RunnerOptions.UseJIT, "Determines whether the supplied BrainF*ck code should be compiled or interpreted.");
    runner->add_flag("-t,--time", RunnerOptions.UseStopwatch,
                 "Whether the time the program takes to finish should be recorded and displayed");
    runner->add_flag("-d,--dump", RunnerOptions.EnableDump,
                     "Determines whether the the memory should be dumped when the Brainf*ck program exits, and whether the MemoryDump instruction should be executed or not");
#ifdef DEBUG
    runner->add_flag("-x,--output-instructions", RunnerOptions.OutputInstructions,
                 "output the loaded code to stdout instead of executing it.");
#endif

    //endregion
    
    //region Converter options
    
    converter->callback(ConverterCallback);

    converter->add_option("OutputConfigPath", ConverterOptions.OutputConfigPath, "Output dialect configuration file path")->required();
    converter->add_option("InputProgramPath", ConverterOptions.InputPath, "Input program file path")->required();

    converter->add_option("OutputProgramPath", ConverterOptions.OutputPath, "Converted program output file path");
    converter->add_option("InputConfigPath", ConverterOptions.InputConfigPath, "Input dialect configuration file path");
    
    //endregion

    CLI11_PARSE(app, argc, argv);
    
    return 0;
}
#pragma clang diagnostic pop