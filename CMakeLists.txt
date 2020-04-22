cmake_minimum_required(VERSION 3.14)
project(BrainFckInterpreter)

set(ASMJIT_STATIC ON)
#set(LINK_STATIC TRUE)

if (NOT DEFINED LINK_STATIC)
    set(LINK_STATIC FALSE)
endif ()

include("asmjit/CMakeLists.txt")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -s -m64")

add_executable(BrainFckInterpreter8 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/Optimizer/BFOptimizer.cpp BFInterpreter/Optimizer/BFOptimizer.h BFInterpreter/Instructions/BFInstruction.cpp BFInterpreter/Instructions/BFInstruction.h BFInterpreter/Environments/BFEnvironment.cpp BFInterpreter/Environments/BFEnvironment.h BFInterpreter/Instructions/BFLoopInstruction.h BFInterpreter/JIT/BFJITRunner.cpp BFInterpreter/JIT/BFJITRunner.h BFInterpreter/Environments/BFInterpreterEnvironment.cpp BFInterpreter/Environments/BFInterpreterEnvironment.h BFInterpreter/BFLoader.cpp BFInterpreter/BFLoader.h Stopwatch.h BFInterpreter/Instructions/BFMutatorInstruction.h BFInterpreter/Optimizer/BFLoopPattern.h BFInterpreter/Optimizer/BFLoopPatternItem.h BFInstructionType.cpp BFInterpreter/BFRunner.h BFInterpreter/BFRunner.cpp BFInterpreter/BFTokenInfo.h)
add_executable(BrainFckInterpreter16 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/Optimizer/BFOptimizer.cpp BFInterpreter/Optimizer/BFOptimizer.h BFInterpreter/Instructions/BFInstruction.cpp BFInterpreter/Instructions/BFInstruction.h BFInterpreter/Environments/BFEnvironment.cpp BFInterpreter/Environments/BFEnvironment.h BFInterpreter/Instructions/BFLoopInstruction.h BFInterpreter/JIT/BFJITRunner.cpp BFInterpreter/JIT/BFJITRunner.h BFInterpreter/Environments/BFInterpreterEnvironment.cpp BFInterpreter/Environments/BFInterpreterEnvironment.h BFInterpreter/BFLoader.cpp BFInterpreter/BFLoader.h Stopwatch.h BFInterpreter/Instructions/BFMutatorInstruction.h BFInterpreter/Optimizer/BFLoopPattern.h BFInterpreter/Optimizer/BFLoopPatternItem.h BFInstructionType.cpp BFInterpreter/BFRunner.h BFInterpreter/BFRunner.cpp BFInterpreter/BFTokenInfo.h)
add_executable(BrainFckInterpreter32 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/Optimizer/BFOptimizer.cpp BFInterpreter/Optimizer/BFOptimizer.h BFInterpreter/Instructions/BFInstruction.cpp BFInterpreter/Instructions/BFInstruction.h BFInterpreter/Environments/BFEnvironment.cpp BFInterpreter/Environments/BFEnvironment.h BFInterpreter/Instructions/BFLoopInstruction.h BFInterpreter/JIT/BFJITRunner.cpp BFInterpreter/JIT/BFJITRunner.h BFInterpreter/Environments/BFInterpreterEnvironment.cpp BFInterpreter/Environments/BFInterpreterEnvironment.h BFInterpreter/BFLoader.cpp BFInterpreter/BFLoader.h Stopwatch.h BFInterpreter/Instructions/BFMutatorInstruction.h BFInterpreter/Optimizer/BFLoopPattern.h BFInterpreter/Optimizer/BFLoopPatternItem.h BFInstructionType.cpp BFInterpreter/BFRunner.h BFInterpreter/BFRunner.cpp BFInterpreter/BFTokenInfo.h)

target_link_libraries(BrainFckInterpreter8 asmjit)
target_link_libraries(BrainFckInterpreter16 asmjit)
target_link_libraries(BrainFckInterpreter32 asmjit)

target_compile_definitions(BrainFckInterpreter16 PUBLIC LargeAddressAware)
target_compile_definitions(BrainFckInterpreter32 PUBLIC HugeAddressAware)

if (LINK_STATIC)
    #set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -static")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static")
endif ()