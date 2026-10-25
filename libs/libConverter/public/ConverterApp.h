// Copyright (c) 2026 Madusanka Jayarathna. All rights reserved.
// Use of this source code is governed by the MIT license.

#pragma once

class ConverterApp{
public:
    struct MediaInfo {
        double      duration{0};
        int64_t     vBitRate{-1};
        int64_t     aBitRate{-1};
        int64_t     vSize{-1};
        int64_t     aSize{-1};
        int64_t     crf{0};
        int64_t     width{-1};
        int64_t     height{-1};
        int64_t     size{-1};
        bool        hasVStream{false};
        bool        hasAStream{false};
        std::string fileName;
        std::string vDecoder;
        std::string vCodec;
        std::string aDecoder;
        std::string aCodec;

    };

public:
    ConverterApp();
    void run();

private:
    void showProductTitle();
    void getFilePath();
    bool analyzeMediaFile();
    void showMediaInfo(const MediaInfo& info);
    bool getTargetMediaInfo();
    bool runConversion();

private:
    std::string                                     _mediaFile;
    std::string                                     _mediaDir;
    MediaInfo                                       _originalFile;
    MediaInfo                                       _targetFile;
};