#pragma once 
#include <iostream>

#if defined(_WIN32)
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN popen
    #define PCLOSE pclose
#endif

class Utils{
public:
    Utils() = delete;
    ~Utils() = delete;
    Utils(const Utils&) = delete;
    Utils& operator=(const Utils&) = delete;

    static std::string getUserInput(std::string& msg);
    static bool isNumeric(std::string& inStr, int& value);
    static std::pair<int, std::string> executeCommand(const std::string& command, bool verbose=false);
    bool analyzePath(const std::string& path);
};