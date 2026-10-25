// Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
// Use of this source code is governed by the MIT license.

#pragma once

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

        std::string getSize() const;
    }; 

public:
    DownloaderApp();
    void run();

private:
    void showProductTitle();
    bool showAvailableFormats(bool isVideo, bool isAudio);
    void getURL();
    void getDownloadPath();
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