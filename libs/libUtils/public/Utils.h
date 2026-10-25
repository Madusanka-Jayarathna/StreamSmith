// Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
// Use of this source code is governed by the MIT license.

#pragma once

#include <string>
#include <utility>

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

    static void setupBundledPath(const char* argv0 = nullptr);
    static const std::string getUserInput(const std::string& msg, bool hasDefault=false);
    static bool isNumeric(const std::string& inStr, int& value);
    static std::pair<int, std::string> executeCommand(const std::string& command, bool verbose=false);
    static bool analyzePath(const std::string& path);
    static bool analyzeFile(const std::string& path);
};
