cmake_minimum_required(VERSION 3.14)
project(BrainFckInterpreter)

set(ASMJIT_STATIC ON)
#set(LINK_STATIC TRUE)

set(USE_JIT TRUE)
set(USE_STOPWATCH TRUE)

if(NOT DEFINED USE_JIT)
    set(USE_JIT FALSE)
endif()
if(NOT DEFINED USE_STOPWATCH)
    set(USE_STOPWATCH FALSE)
endif()
if (NOT DEFINED LINK_STATIC)
    set(LINK_STATIC FALSE)
endif ()

include("asmjit/CMakeLists.txt")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -s -march=native -m64")

add_executable(BrainFckInterpreter8 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/BFOptimizer.cpp BFInterpreter/BFOptimizer.h BFInterpreter/BFInstruction.cpp BFInterpreter/BFInstruction.h BFInterpreter/Environments/BFEnvironment.cpp BFInterpreter/Environments/BFEnvironment.h BFInterpreter/BFLoopInstruction.h BFInterpreter/JIT/BFJITRunner.cpp BFInterpreter/JIT/BFJITRunner.h BFInterpreter/Environments/BFInterpreterEnvironment.cpp BFInterpreter/Environments/BFInterpreterEnvironment.h BFInterpreter/BFLoader.cpp BFInterpreter/BFLoader.h Stopwatch.h)
add_executable(BrainFckInterpreter16 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/BFOptimizer.cpp BFInterpreter/BFOptimizer.h BFInterpreter/BFInstruction.cpp BFInterpreter/BFInstruction.h BFInterpreter/Environments/BFEnvironment.cpp BFInterpreter/Environments/BFEnvironment.h BFInterpreter/BFLoopInstruction.h BFInterpreter/JIT/BFJITRunner.cpp BFInterpreter/JIT/BFJITRunner.h BFInterpreter/Environments/BFInterpreterEnvironment.cpp BFInterpreter/Environments/BFInterpreterEnvironment.h BFInterpreter/BFLoader.cpp BFInterpreter/BFLoader.h Stopwatch.h)
add_executable(BrainFckInterpreter32 main.cpp BFInterpreter/BFInterpreter.cpp BFInterpreter/BFInterpreter.h Logging.h BFDefines.h BFInterpreter/BFOptimizer.cpp BFInterpreter/BFOptimizer.h BFInterpreter/BFInstruction.cpp BFInterpreter/BFInstruction.h BFInterpreter/Environments/BFEnvironment.cpp BFInterpreter/Environments/BFEnvironment.h BFInterpreter/BFLoopInstruction.h BFInterpreter/JIT/BFJITRunner.cpp BFInterpreter/JIT/BFJITRunner.h BFInterpreter/Environments/BFInterpreterEnvironment.cpp BFInterpreter/Environments/BFInterpreterEnvironment.h BFInterpreter/BFLoader.cpp BFInterpreter/BFLoader.h Stopwatch.h)

target_compile_definitions(BrainFckInterpreter16 PUBLIC LargeAddressAware)
target_compile_definitions(BrainFckInterpreter32 PUBLIC HugeAddressAware)

if (LINK_STATIC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -static")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static")
endif ()

if (USE_JIT)
    ADD_COMPILE_DEFINITIONS(USE_JIT)

    target_link_libraries(BrainFckInterpreter8 asmjit)
    target_link_libraries(BrainFckInterpreter16 asmjit)
    target_link_libraries(BrainFckInterpreter32 asmjit)
endif ()

if (USE_STOPWATCH)
    ADD_COMPILE_DEFINITIONS(USE_STOPWATCH)
endif ()