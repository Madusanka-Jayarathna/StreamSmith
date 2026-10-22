#pragma once

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>



class ConverterApp{
public:
    ConverterApp();
    void run();

private:
    void showProductTitle();
    void getFilePath();
    void analyzeMediaFile();

    //bool showAvailableFormats(bool isVideo, bool isAudio);
    //void getURL();
    //void getDownloadPath();
    //bool isValidUrlSyntax(const std::string& url);
    //std::pair<int, std::string> executeCommand(const std::string& command);
    //bool analyzeURL(const std::string& url);
    //int getUserInput(const std::string& msg) ;
    //void startDownload();
    //bool DownloadMedia(DownloaderApp::DownloadMode mode, const int& videoId = -1, const int& audioId = -1);

private:
    //size_t                                          _uniqueID;
    //std::string                                     _url;
    std::string                                     _mediaFile; 
    //std::unordered_map<size_t, StreamInfo>          _videoFormats;
    //std::unordered_map<size_t, StreamInfo>          _audioFormats;
};