// Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
// Use of this source code is governed by the MIT license.

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <filesystem>

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <unistd.h>
    #include <climits>
#elif defined(__linux__)
    #include <unistd.h>
    #include <climits>
#else
    #include <unistd.h>
#endif

#include "libUtils/public/Utils.h"

void
Utils::setupBundledPath(const char* argv0) {
    namespace fs = std::filesystem;
    fs::path exeDir;

#if defined(_WIN32)
    char buffer[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (length > 0) {
        exeDir = fs::path(std::string(buffer, length)).parent_path();
    }
#elif defined(__APPLE__)
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) == 0) {
        std::error_code ec;
        exeDir = fs::canonical(fs::path(buffer), ec).parent_path();
    }
#elif defined(__linux__)
    char buffer[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (count > 0) {
        buffer[count] = '\0';
        std::error_code ec;
        exeDir = fs::canonical(fs::path(buffer), ec).parent_path();
    }
#endif

    // Fallback using argv0 if platform detection did not resolve
    if (exeDir.empty() && argv0 && argv0[0] != '\0') {
        std::error_code ec;
        exeDir = fs::absolute(fs::path(argv0), ec).parent_path();
    }

    if (exeDir.empty()) {
        return;
    }

#if defined(_WIN32)
    std::string currentPath = std::getenv("PATH") ? std::getenv("PATH") : "";
    std::string newPath = exeDir.string() + ";" + currentPath;
    _putenv_s("PATH", newPath.c_str());
#else
    const char* currentPath = std::getenv("PATH");
    std::string newPath = exeDir.string() + ":" + (currentPath ? currentPath : "");
    setenv("PATH", newPath.c_str(), 1);
#endif
}

const std::string
Utils::getUserInput(const std::string& msg, bool hasDefault){
    std::string rawInput;
    bool success = false;
    
    while(!success) {
        std::cout << msg;
        std::getline(std::cin, rawInput);
        if(!rawInput.empty()){
            success = true;
        } else if (hasDefault) {
            success = true;
        }
    }
    return rawInput;
}

bool
Utils::isNumeric(const std::string& inStr, int& val) {
    if (inStr.empty()) {
        return false;
    }

    try {
        size_t charsProcessed = 0;
        int parsedValue = std::stoi(inStr, &charsProcessed);

        if (charsProcessed < inStr.size()) {
            return false;
        }

        val = parsedValue;
        return true;
    }
    catch (const std::invalid_argument&) {
        return false;
    }
    catch (const std::out_of_range&) {
        return false;
    }
}

std::pair<int, std::string> 
Utils::executeCommand(const std::string& command, bool verbose) {
    std::string output;
    char buffer[256];

    FILE* pipe = POPEN(command.c_str(), "r");
    if (!pipe) {
        return {-1, ""};
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        if(verbose) {
            std::cout << buffer;
        }
        output += buffer;
    }

    int exitCode = PCLOSE(pipe);
    return {exitCode, output};
}

bool 
Utils::analyzePath(const std::string& path) {
    if(path.empty()) {
        return false;
    }

    std::filesystem::path p(path); 
    std::error_code ec;
    p = std::filesystem::absolute(p, ec);
    if (ec) {
        return false;
    }
    
    return std::filesystem::exists(p, ec) && std::filesystem::is_directory(p, ec);
}

bool 
Utils::analyzeFile(const std::string& path) {
    if(path.empty()) {
        return false;
    }

    std::filesystem::path p(path); 
    std::error_code ec;
    p = std::filesystem::absolute(p, ec);
    if (ec) {
        return false;
    }
    
    return std::filesystem::exists(p, ec) && std::filesystem::is_regular_file(p, ec);
}