//
// Created by Stijn on 23/04/2020.
//

#ifndef BRAINFCKINTERPRETER_UTIL_H
#define BRAINFCKINTERPRETER_UTIL_H

#include <string>
#include <ctime>

std::string CurrentDateTime() {
    time_t     now = time(0);
    struct tm  timeStruct{};
    char       buf[80];
    timeStruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%h-%m", &timeStruct);

    return std::string(buf);
}

#endif //BRAINFCKINTERPRETER_UTIL_H
