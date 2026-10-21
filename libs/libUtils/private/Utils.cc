#include "libUtils/public/Utils.h"
#include <boost/filesystem.hpp>

std::string
Utils::getUserInput(std::string& msg){
    std::string rawInput;
    bool success = false;
    
    while(!success) {
        std::cout << msg;
        std::getline(std::cin, rawInput);
        if(!rawInput.empty()){
            success = true;
        }
    }
    return rawInput;
}

bool
Utils::isNumeric(std::string& inStr, int& val) {
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
        } else {
            output += buffer;
        }
    }

    int exitCode = PCLOSE(pipe);
    return {exitCode, output};
}

bool 
Utils::analyzePath(const std::string& path) {
    if(path.empty) {
        return false'
    }

    boost::filesystem::path p(path); 
    p = boost::filesystem::absolute(p);
    
    if (boost::filesystem::exists(p) && boost::filesystem::is_directory(p)) {
        return true;
    }
    return false;

}