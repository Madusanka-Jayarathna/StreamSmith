#pragma once

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#if defined(_WIN32)
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN popen
    #define PCLOSE pclose
#endif

class DownloaderApp{
public:
    enum class DownloadMode {
        HighestQuality,
        VideoOnly,
        AudioOnly,
        CustomCombine
    };

    struct StreamInfo {
        std::string     formatID;
        std::string     extension;
        std::string     resolutionOrBitRate;
        int             height{0};
        int             bitRate{0};
        int64_t         sizeBytes{0};

        std::string 
        getSize() const {
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
    }; 

public:
    DownloaderApp();
    void run();

private:
    void showProduct();
    bool showAvailableFormats(bool isVideo, bool isAudio);
    void getURL();
    void getDownloadPath();
    bool analyzePath(const std::string& downloadPath);
    bool isValidUrlSyntax(const std::string& url);
    std::pair<int, std::string> executeCommand(const std::string& command);
    bool analyzeURL(const std::string& url);
    int getUserInput(const std::string& msg) ;
    void startDownload();
    bool DownloadMedia(DownloaderApp::DownloadMode mode, const int& videoId = -1, const int& audioId = -1);

private:
    size_t                                          _uniqueID;
    std::string                                     _url;
    std::string                                     _downloadPath; 
    std::unordered_map<size_t, StreamInfo>          _videoFormats;
    std::unordered_map<size_t, StreamInfo>          _audioFormats;
};