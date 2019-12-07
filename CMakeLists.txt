cmake_minimum_required(VERSION 3.14)
project(BrainFckInterpreter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s")

add_executable(BrainFckInterpreter8 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/BFOptimizer.cpp BFInterpreter/BFOptimizer.h BFInterpreter/BFInstruction.cpp BFInterpreter/BFInstruction.h BFInterpreter/BFEnvironment.cpp BFInterpreter/BFEnvironment.h BFInterpreter/BFLoopInstruction.h)
add_executable(BrainFckInterpreter16 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/BFOptimizer.cpp BFInterpreter/BFOptimizer.h BFInterpreter/BFInstruction.cpp BFInterpreter/BFInstruction.h BFInterpreter/BFEnvironment.cpp BFInterpreter/BFEnvironment.h BFInterpreter/BFLoopInstruction.h)
add_executable(BrainFckInterpreter32 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/BFOptimizer.cpp BFInterpreter/BFOptimizer.h BFInterpreter/BFInstruction.cpp BFInterpreter/BFInstruction.h BFInterpreter/BFEnvironment.cpp BFInterpreter/BFEnvironment.h BFInterpreter/BFLoopInstruction.h)

target_compile_definitions(BrainFckInterpreter16 PUBLIC LargeAddressAware)
target_compile_definitions(BrainFckInterpreter32 PUBLIC HugeAddressAware)