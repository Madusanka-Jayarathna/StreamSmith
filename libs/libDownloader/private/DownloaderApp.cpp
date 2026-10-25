// Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
// Use of this source code is governed by the MIT license.

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "libDownloader/public/DownloaderApp.h"
#include "libUtils/public/Utils.h"
#include <nlohmann/json.hpp>
#include <filesystem>

using json = nlohmann::json;

std::string
DownloaderApp::StreamInfo::getSize() const {
    if(sizeBytes <= 0) {
        return "unknown size";
    }
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double size = static_cast<double>(sizeBytes);
    int i = 0;
    while (size >= 1024.0 && i < 4) {
        size /= 1024.0;
        i++;
    }
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[i]);
    return std::string(buffer);
}

DownloaderApp::DownloaderApp( void ) :
    _uniqueID(0),
    _url(""),
    _downloadPath("")
{
    _videoFormats.clear();
    _audioFormats.clear(); 
}

void
DownloaderApp::run() {
    showProductTitle();
    getDownloadPath();
    getURL();
    startDownload();
}

void
DownloaderApp::getDownloadPath() {
    const std::string msg = "Enter Download Path (default: pwd): ";
    bool success = false;
    std::string inStr;
    while(!success) {
        inStr = Utils::getUserInput(msg, true);
        if(inStr.empty()) {
            inStr = std::filesystem::current_path().string();
        }

        success = Utils::analyzePath(inStr);
        if(!success) {
            std::cout << "ERROR     : Invalid Download Path: " << inStr << std::endl << std::endl;
        } else {
            std::cout << "STATUS    : Donwload Director Selected: " << inStr << std::endl << std::endl;
        } 
    }
    _downloadPath = inStr;
}

void
DownloaderApp::showProductTitle(){
    std::cout << "\n";
    std::cout << "--------------------------------------------------" << "\n";
    std::cout << "                  Downloader                      " << "\n";    
    std::cout << "--------------------------------------------------" << "\n\n";
}

// handle empty map; go back analyze step
bool
DownloaderApp::showAvailableFormats(bool isVideo, bool isAudio){
    bool success = false;
    if (isVideo && !_videoFormats.empty()) {
        std::cout << "\n=== Video Formats Found (" << _videoFormats.size() << ") ===\n";
        for(const auto& v: _videoFormats) {
            std::cout   << "ID: " << v.first
                        << "\t| Res: " << v.second.resolutionOrBitRate
                        << "\t| Ext: " << v.second.extension
                        << "\t| BitRate: " << v.second.bitRate << "kbps"
                        << "\t| Size: " << v.second.getSize() << "\n";
        }
        success = true;
    }

    if (isAudio && !_audioFormats.empty()) {
        std::cout << "\n=== Audio Formats Found (" << _audioFormats.size() << ") ===\n";
        for (const auto& a : _audioFormats) {
            std::cout << "ID: " << a.first 
                      << "\t| Ext: " << a.second.extension 
                      << "\t| Bitrate: " << a.second.resolutionOrBitRate 
                      << "\t| Size: " << a.second.getSize() << "\n";
        }
        success = true;
    }

    return success;
}

void
DownloaderApp::getURL() {
    const std::string msg = "Enter URL: ";
    bool success = false;
    std::string inStr;
    while(!success) {
        inStr = Utils::getUserInput(msg);
        success = analyzeURL(inStr);
    }
    _url = inStr;
}

bool 
DownloaderApp::isValidUrlSyntax(const std::string& url) {
    if (url.empty()) return false;
    return (url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0);
}

bool
DownloaderApp::analyzeURL(const std::string& url) {
    _videoFormats.clear();
    _audioFormats.clear();
    _uniqueID = 1;

    if (!isValidUrlSyntax(url)) {
        std::cout << "ERROR     : Invalid URL: " << url << std::endl << std::endl;
        return false;
    }

    std::string command =
        "yt-dlp"
        " --dump-json"
        " --quiet"
        " --no-warnings"
        " --socket-timeout 15"
        " --no-playlist"
        " --impersonate firefox"
        " \"" + url + "\"";
    //std::string command = "yt-dlp --dump-json --quiet --no-warnings --socket-timeout 15 --no-playlist --ies \"all,-generic\" \"" + url + "\"";
    auto [exitCode, jsonOutput] = Utils::executeCommand(command);

    if (exitCode != 0 || jsonOutput.empty()) {
        std::cout << "ERROR     : Unable Extract Media Information\n\n";
        return false;
    }

    try {
        json parsedData = json::parse(jsonOutput);

        if (!parsedData.contains("formats") || !parsedData["formats"].is_array()) {
            std::cout << "ERROR     : Unable Extract Media Information\n\n";
            return false;
        }

        for (const auto& fmt : parsedData["formats"]) {
            std::string formatId = fmt.value("format_id", "");
            if(formatId.empty()) {
                continue;
            }
            std::string ext = fmt.value("ext", "N/A");
            
            std::string vcodec = fmt.value("vcodec", "none");
            std::string acodec = fmt.value("acodec", "none");

            int64_t sizeBytes = 0;
            if (fmt.contains("filesize") && !fmt["filesize"].is_null()) {
                sizeBytes = fmt["filesize"].get<int64_t>();
            } else if (fmt.contains("filesize_approx") && !fmt["filesize_approx"].is_null()) {
                sizeBytes = fmt["filesize_approx"].get<int64_t>();
            }

            if (vcodec != "none") {
                StreamInfo vInfo;
                vInfo.formatID = formatId;
                vInfo.extension = ext;
                vInfo.sizeBytes = sizeBytes;
                vInfo.height = fmt.value("height", 0);
                vInfo.bitRate = static_cast<int>(fmt.value("tbr", 0.0));
                
                vInfo.resolutionOrBitRate = (vInfo.height > 0) 
                    ? std::to_string(vInfo.height) + "p" 
                    : fmt.value("format_note", "N/A");

                _videoFormats.emplace(_uniqueID, vInfo);
            }

            if (acodec != "none" && vcodec == "none") {
                StreamInfo aInfo;
                aInfo.formatID = formatId;
                aInfo.extension = ext;
                aInfo.sizeBytes = sizeBytes;
                
                int abr = static_cast<int>(fmt.value("abr", 0.0));
                if (abr == 0) {
                    abr = static_cast<int>(fmt.value("tbr", 0.0));
                }
                
                aInfo.bitRate = abr;
                aInfo.resolutionOrBitRate = std::to_string(abr) + " kbps";

                _audioFormats.emplace(_uniqueID, aInfo);
            }
            _uniqueID++;
        }

        if (_videoFormats.empty() && _audioFormats.empty()) {
            std::cout << "ERROR     : Unable Extract Media Information\n\n";
            return false;
        }
        std::string title = parsedData["title"];
        if(title.empty()) {
            title = url;
        }
        std::cout << "STATUS    : Successfully Extracted Media Information: " << title << std::endl << std::endl;
        return true;

    } catch (const json::parse_error&) {
        std::cout << "ERROR     : Unable Extract Media Information\n\n";
    }
    return false;
}



bool 
DownloaderApp::DownloadMedia(DownloaderApp::DownloadMode mode, const int& videoId, const int& audioId) {
    if (_url.empty()) {
        std::cout << "ERROR     : Invalid URL: " << _url << "\n";
        return false;
    }


    if (_downloadPath.empty()) {
        std::cout << "ERROR     : Invalid Donwload Path: " << _downloadPath << "\n";
        return false;
    }

    std::string outTemplate = _downloadPath + "/%(title).30s.%(ext)s";
    std::string command = "yt-dlp --newline --no-warnings --socket-timeout 15 --restrict-filenames --trim-filenames 30 -o \"" + outTemplate + "\" ";
    std::string vID;
    std::string aID;

    switch (mode) {
        case DownloaderApp::DownloadMode::HighestQuality:
        {
            command += "-f \"bestvideo+bestaudio/best\" --merge-output-format mp4 ";
            break;
        }
        case DownloaderApp::DownloadMode::VideoOnly:
        { 
            if (videoId < 0) {
                std::cout << "Error     : Video ID is required for Video Only download.\n";
                return false;
            }

            auto it = _videoFormats.find(videoId);
            if(it == _videoFormats.end()) {
                std::cout << "Error     : Valid Video ID is required for Video Only download.\n";
                return false; 
            }
            
            vID = it->second.formatID;
            command += "-f \"" + vID + "\" ";
            break;
        }
        case DownloaderApp::DownloadMode::AudioOnly:
        {
            if (audioId < 0) {
                std::cout << "Error: Audio ID is required for Audio Only download.\n";
                return false;
            }

            auto it = _audioFormats.find(audioId);
            if(it == _audioFormats.end()) {
                std::cout << "Error     : Valid Audio ID is required for Audio Only download.\n";
                return false; 
            }
            
            aID = it->second.formatID;
            command += "-f \"" + aID + "\" -x --audio-format mp3 --audio-quality 192K ";
            break;
        }
        case DownloaderApp::DownloadMode::CustomCombine:
        {
            if (videoId < 0 || audioId < 0) {
                std::cout << "Error     : Both Video ID and Audio ID are required for Custom download.\n";
                return false;
            }

            auto it = _videoFormats.find(videoId);
            if(it == _videoFormats.end()) {
                std::cout << "Error     : Valid Video ID is required for custom download.\n";
                return false; 
            }
            vID = it->second.formatID;

            it = _audioFormats.find(audioId);
            if(it == _audioFormats.end()) {
                std::cout << "Error     : Valid Audio ID is required for custom download.\n";
                return false; 
            }
            aID = it->second.formatID;

            command += "-f \"" + vID + "+" + aID + "\" --merge-output-format mkv ";
            break;
        }
        default:
            break;
    }

    command += "\"" + _url + "\"";

    auto [exitCode, output] = Utils::executeCommand(command, true);
    
    bool success = false;
    if (exitCode != 0 || output.empty()) {
        std::cout << "unable to execute download command\n";
    } else {
        success = true;
    }

    if (success) {
        std::cout << "\nSTATUS  : Download Complete!\n";
    } else {
        std::cout << "\nERROR   : Download Failed!\n";
    }
    return success;
}

void
DownloaderApp::startDownload() {
    std::cout << "\n";
    std::cout << "1 - Highest Quality Available\n";
    std::cout << "2 - Video Only\n";
    std::cout << "3 - Audio Only\n";
    std::cout << "4 - Custom\n";
    
    const std::string msg = "Select Download Mode: ";
    int mode = 0;
    bool success = Utils::isNumeric(Utils::getUserInput(msg), mode);
    bool valid = false;
    while(!valid) {
        if(!success || (success && (mode < 1 || mode > 4))) {
            std::cout << "ERROR     : Invalid Donwload Mode Selected. Please re-enter\n";
            success = Utils::isNumeric(Utils::getUserInput(msg), mode);
            continue;
        }
        valid = true;
    }

    const std::string vMsg = "Select Required Video (ID): ";
    const std::string aMsg = "Select Required Audio (ID): ";

    int vID = -1;
    int aID = -1;

    if (mode == 1) {
        std::cout << "Downloading Highest Quality Media\n";
        DownloadMedia(DownloaderApp::DownloadMode::HighestQuality); 
    } else if (mode == 2) {
        showAvailableFormats(true, false);
        Utils::isNumeric(Utils::getUserInput(vMsg), vID);
        DownloadMedia(DownloaderApp::DownloadMode::VideoOnly, vID); 
    } else if (mode == 3) {
        showAvailableFormats(false, true);
        Utils::isNumeric(Utils::getUserInput(aMsg), aID);
        DownloadMedia(DownloaderApp::DownloadMode::AudioOnly, aID);  
    } else {
        showAvailableFormats(true, true);
        Utils::isNumeric(Utils::getUserInput(vMsg), vID);
        Utils::isNumeric(Utils::getUserInput(aMsg), aID);
        DownloadMedia(DownloaderApp::DownloadMode::CustomCombine, vID, aID);  
    }
    return;
}