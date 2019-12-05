//
// Created by Stijn on 05/12/2019.
//

#ifndef BRAINFCKINTERPRETER_LOGGING_H
#define BRAINFCKINTERPRETER_LOGGING_H

#define LogMessage(message) \
{                                            \
std::cout << "[+] " << message << std::endl;           \
}

#define LogWarning(message) \
{                                            \
std::cout << "[*] " << message << std::endl;           \
}

#define LogError(message) \
{                                            \
std::cout << "[!] " << message << std::endl;           \
}

#ifdef DEBUG
#define LogDebug(message) \
{                                            \
std::cout << "[?] " << message << std::endl;           \
}
#else
#define LogDebug(message)
#endif

#define LogFatal(message, exitCode) \
{                                            \
std::cout << "[!!!] " << message << std::endl;           \
exit(exitCode);                              \
}

#endif //BRAINFCKINTERPRETER_LOGGING_H
