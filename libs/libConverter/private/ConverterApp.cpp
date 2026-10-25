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

#include "libConverter/public/ConverterApp.h"
#include "libUtils/public/Utils.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <filesystem>

using json = nlohmann::json;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

ConverterApp::ConverterApp( void ) :
    _mediaFile(""),
    _originalFile{},
    _targetFile{}
{
    //
}

void
ConverterApp::run() {
    showProductTitle();
    getFilePath();
    if(!analyzeMediaFile())
        return;

    showMediaInfo(_originalFile);
    if (!getTargetMediaInfo()) {
        return;
    }
    showMediaInfo(_targetFile);
    runConversion();
    return;

}

bool
ConverterApp::runConversion(){
    std::string command;
    const auto quoteShellArgument = [](const std::string& argument) {
#if defined(_WIN32)
        std::string quoted = "\"";
        for (char character : argument) {
            if (character == '"') {
                quoted += "\\\"";
            } else {
                quoted += character;
            }
        }
        quoted += "\"";
        return quoted;
#else
        std::string quoted = "'";
        for (char character : argument) {
            if (character == '\'') {
                quoted += "'\\''";
            } else {
                quoted += character;
            }
        }
        quoted += "'";
        return quoted;
#endif
    };


    const std::string inputPath = (std::filesystem::path(_mediaDir) / _originalFile.fileName).string();
    const std::string outputPath = (std::filesystem::path(_mediaDir) / _targetFile.fileName).string();
    const std::string inputArgument = quoteShellArgument(inputPath);
    const std::string outputArgument = quoteShellArgument(outputPath);

    const std::string scaleFilter = "scale=" +
        std::to_string(_targetFile.width) + ":" +
        std::to_string(_targetFile.height) + ":force_original_aspect_ratio=decrease,pad=" +
        std::to_string(_targetFile.width) + ":" +
        std::to_string(_targetFile.height) + ":(ow-iw)/2:(oh-ih)/2";

    const std::string videoOptions =
        "-vf " + quoteShellArgument(scaleFilter) +
        " -c:v " + quoteShellArgument(_targetFile.vDecoder) +
        (_targetFile.vDecoder != "mpeg2video"
            ? " -crf " + std::to_string(_targetFile.crf)
            : "");

    const std::string audioOptions =
        "-c:a " + quoteShellArgument(_targetFile.aDecoder) +
        " -b:a " + std::to_string(_targetFile.aBitRate) + "k";

    if (_targetFile.hasVStream && _targetFile.hasAStream) {
        command = "ffmpeg -i " + inputArgument + " " + videoOptions + " " + audioOptions + " " + outputArgument;
    } else if (_targetFile.hasVStream) {
        command = "ffmpeg -i " + inputArgument + " " + videoOptions + " -an " + outputArgument;
    } else if (_targetFile.hasAStream) {
        command = "ffmpeg -i " + inputArgument + " -vn " + audioOptions + " " + outputArgument;
    }

    
    auto [exitCode, output] = Utils::executeCommand(command, true);

    bool success = false;
    if (exitCode != 0) {
        std::cout << "unable to execute command\n";
    } else {
        success = true;
    }

    if(output.empty()) {
        //
    }

    if (success) {
        std::cout << "\nSTATUS  : Conversion Complete!\n";
    } else {
        std::cout << "\nERROR   : Conversion Failed!\n";
    }
    return success;
}

bool
ConverterApp::getTargetMediaInfo(){
    struct ContainerOptions {
        std::string name;
        std::string extension;
        std::vector<std::string> videoCodecs;
        std::vector<std::string> audioCodecs;
    };

    const std::vector<ContainerOptions> containers = {
        {"MKV", "mkv", {"H.264", "HEVC", "AV1", "VP9", "MPEG2"}, {"No audio", "AAC", "MP3", "Opus"}},
        {"MP4", "mp4", {"H.264", "HEVC", "AV1", "VP9"}, {"No audio", "AAC", "MP3", "Opus"}},
        {"MP3", "mp3", {}, {"MP3"}},
        {"AAC", "aac", {}, {"AAC"}},
        {"M4A", "m4a", {}, {"AAC", "MP3", "Opus"}}
    };

    const auto selectOption = [](const std::string& prompt, const std::vector<std::string>& options) {
        for (std::size_t index = 0; index < options.size(); ++index) {
            std::cout << index << " - " << options[index] << "\n";
        }

        int selection = -1;
        while (true) {
            if (Utils::isNumeric(Utils::getUserInput(prompt), selection) && selection >= 0 && static_cast<std::size_t>(selection) < options.size()) {
                return options[static_cast<std::size_t>(selection)];
            }
            std::cout << "ERROR     : Invalid selection. Please re-enter\n\n";
        }
    };

    const auto selectInteger = [](const std::string& prompt, int minimum, int maximum) {
        int selection = 0;
        while (true) {
            if (Utils::isNumeric(Utils::getUserInput(prompt), selection) && selection >= minimum && selection <= maximum) {
                return selection;
            }
            std::cout << "ERROR     : Invalid selection. Please re-enter\n\n";
        }
    };

    const auto selectDimension = [](const std::string& prompt, int64_t originalDimension) {
        while (true) {
            const std::string input = Utils::getUserInput(prompt, true);
            if (input.empty()) {
                return originalDimension;
            }

            int dimension = 0;
            if (Utils::isNumeric(input, dimension) && dimension >= 1 && dimension <= 5000) {
                return static_cast<int64_t>(dimension);
            }
            std::cout << "ERROR     : Invalid dimension. Please enter a value from 1 to 5000, or leave blank to keep the original\n\n";
        }
    };

    std::vector<std::string> containerNames;
    containerNames.reserve(containers.size());
    for (const auto& container : containers) {
        containerNames.push_back(container.name);
    }

    const std::string containerType = selectOption("Select Required Output Type: ", containerNames);
    const auto container = std::find_if(containers.begin(), containers.end(),
        [&containerType](const ContainerOptions& option) {
            return option.name == containerType;
        });

    if (container == containers.end()) {
        return false;
    }

    std::string vCodecType;
    if (_originalFile.hasVStream && !container->videoCodecs.empty()) {
        vCodecType = selectOption("Select Required Video Codec: ", container->videoCodecs);
    }

    std::string aCodecType = "No audio";
    if (_originalFile.hasAStream) {
        aCodecType = selectOption("Select Required Audio Codec: ", container->audioCodecs);
    }
    _targetFile.hasVStream = _originalFile.hasVStream && !vCodecType.empty();
    _targetFile.hasAStream = _originalFile.hasAStream && aCodecType != "No audio";

    if(!_targetFile.hasAStream && !_targetFile.hasVStream) {
        return false;
    }

    // vCompression Ratio
    if(_targetFile.hasVStream) {
        const int compression = selectInteger("Select Required Video Compression (0 - 100): ", 0, 100);
        _targetFile.crf = static_cast<int64_t>(compression * 0.51);
        _targetFile.width = selectDimension("Select Required Video Width (default " + std::to_string(_originalFile.width) + "): ", _originalFile.width);
        _targetFile.height = selectDimension("Select Required Video Height (default " + std::to_string(_originalFile.height) + "): ", _originalFile.height);
        _targetFile.vCodec = vCodecType;
        _targetFile.vBitRate = _originalFile.vBitRate * (100 - compression) / 100;
        _targetFile.vSize = _originalFile.vSize * (100 - compression) / 100;

        const std::unordered_map<std::string, std::string> videoEncoders = {
            {"H.264", "libx264"}, {"HEVC", "libx265"}, {"AV1", "libsvtav1"},
            {"VP9", "libvpx-vp9"}, {"MPEG2", "mpeg2video"}
        };
        _targetFile.vDecoder = videoEncoders.at(vCodecType);
    }

    // aCompression Ratio
    if(_targetFile.hasAStream) {
        const int compression = selectInteger("Select Required Audio Compression (0 - lossless && 100 - lossy): ", 0, 100);
        _targetFile.aBitRate = _originalFile.aBitRate * (100 - compression) / 100;
        _targetFile.aCodec = aCodecType;
        _targetFile.aSize = _originalFile.aSize * (100 - compression) / 100;

        const std::unordered_map<std::string, std::string> audioEncoders = {
            {"AAC", "aac"}, {"MP3", "libmp3lame"}, {"Opus", "libopus"}
        };
        _targetFile.aDecoder = audioEncoders.at(aCodecType);
    }


    _targetFile.fileName = "convertedFile." + container->extension;
    _targetFile.size = _targetFile.vSize > 0 ? _targetFile.vSize : 0 + _targetFile.aSize > 0 ? _targetFile.aSize : 0;
    _targetFile.duration = _originalFile.duration;
    return true;
}

void
ConverterApp::showProductTitle(){
    std::cout << "\n";
    std::cout << "--------------------------------------------------" << "\n";
    std::cout << "                  Converter                       " << "\n";    
    std::cout << "--------------------------------------------------" << "\n\n";
}

void
ConverterApp::showMediaInfo(const MediaInfo& info) {
    std::cout << "\n";
        std::cout << "Media Information     : " << info.fileName << "\tDuration: " << info.duration << " s\tFileSize: " << info.size << " MB\tResolution: " << info.width << "x" << info.height <<"\n";
    if(info.hasVStream) {
        std::cout << "Video Stream - vCodec : " << info.vCodec << "\tvDecoder: " << info.vDecoder << "\tvBitRate: " << info.vBitRate << " kbps\tvSize: " << info.vSize << " MB\tResolution: " << info.width << "x" << info.height << "\n";
    }
    if(info.hasAStream) {
        std::cout << "Audio Stream - aCodec : " << info.aCodec << "\taDecoder: " << info.aDecoder << "\taBitRate: " << info.aBitRate << " kbps\taSize: " << info.aSize << " MB\n\n";
    }
}

void
ConverterApp::getFilePath() {
    const std::string msg = "Enter Media Dir (default: pwd): ";
    bool success = false;
    std::string inStr;
    while(!success) {
        inStr = Utils::getUserInput(msg, true);
        if(inStr.empty()) {
            inStr = std::filesystem::current_path().string();
        }

        success = Utils::analyzePath(inStr);
        if(!success) {
            std::cout << "ERROR     : Invalid Dir: " << inStr << std::endl << std::endl;
        } else {
            std::cout << "STATUS    : Media Directory Selected: " << inStr << std::endl << std::endl;
        }
    }
    _mediaDir = inStr;

    success = false;
    const std::string fileNamePrompt = "Enter File Name: ";
    while(!success) {
        inStr = Utils::getUserInput(fileNamePrompt);
        success = Utils::analyzeFile((std::filesystem::path(_mediaDir) / inStr).string());
        if(!success) {
            std::cout << "ERROR     : Invalid Media File: " << inStr << std::endl << std::endl;
        } else {
            std::cout << "STATUS    : Media File Selected: " << inStr << std::endl << std::endl;
            _mediaFile = inStr;
        }
    }
}

bool
ConverterApp::analyzeMediaFile(){
    if (_mediaFile.empty()) {
        std::cout << "ERROR     : Invalid File: " << _mediaFile << "\n";
        return false;
    }

    _originalFile.fileName = _mediaFile;
    const std::string mediaPath = (std::filesystem::path(_mediaDir) / _mediaFile).string();

    AVFormatContext* fmt = nullptr;
    if (avformat_open_input(&fmt, mediaPath.c_str(), nullptr, nullptr) < 0) {
        std::cout << "ERROR     : Could not open media file: " << mediaPath << "\n";
        return false;
    }

    if (avformat_find_stream_info(fmt, nullptr) < 0) {
        std::cout << "ERROR     : Could not retrieve stream info: " << mediaPath << "\n";
        avformat_close_input(&fmt);
        return false;
    }

    int videoStream = -1;
    int audioStream = -1;

    for (unsigned int i = 0; i < fmt->nb_streams; ++i) {
        AVMediaType type = fmt->streams[i]->codecpar->codec_type;
        if (type == AVMEDIA_TYPE_VIDEO && videoStream < 0) {
            videoStream = static_cast<int>(i);
        } else if (type == AVMEDIA_TYPE_AUDIO && audioStream < 0) {
            audioStream = static_cast<int>(i);
        }
    }

    if(videoStream < 0 && audioStream < 0) {
        std::cout << "ERROR     : Could not retrieve stream info: " << mediaPath << "\n";
        avformat_close_input(&fmt);
        return false;
    }

    int64_t videoSize = 0;
    int64_t audioSize = 0;

    AVPacket* packet = av_packet_alloc();
    while (av_read_frame(fmt, packet) >= 0) {
        if (packet->stream_index == videoStream) {
            videoSize += packet->size;
        } else if (packet->stream_index == audioStream) {
            audioSize += packet->size;
        }
        av_packet_unref(packet);
    }
    av_packet_free(&packet);
    int64_t totalSize = avio_size(fmt->pb);
    const double durationSec = fmt->duration > 0
        ? static_cast<double>(fmt->duration) / AV_TIME_BASE
        : 0.0;

    _originalFile.aSize = (audioStream >= 0 && audioSize)
        ? static_cast<int64_t>(audioSize / (1024.0 * 1024.0))
        : 0;
    _originalFile.size = totalSize > 0
        ? static_cast<int64_t>(totalSize / (1024.0 * 1024.0))
        : 0;

    if (videoStream >= 0) {
        AVCodecParameters* vParams = fmt->streams[videoStream]->codecpar;
        const AVCodec* vCodec = avcodec_find_decoder(vParams->codec_id);

        int64_t vBitRate = vParams->bit_rate;
        if (vBitRate == 0 && durationSec > 0) {
            vBitRate = static_cast<int64_t>((videoSize * 8) / durationSec);
        }

        _originalFile.hasVStream = true;
        _originalFile.width = vParams->width;
        _originalFile.height = vParams->height;
        _originalFile.duration = durationSec;
        _originalFile.vSize = (videoSize)
            ? static_cast<int64_t>(videoSize / (1024.0 * 1024.0))
            : 0;
        _originalFile.vBitRate = vBitRate/1000;
        _originalFile.vDecoder = (vCodec) ? vCodec->name : "NA";
        _originalFile.vCodec = avcodec_get_name(vParams->codec_id);
    }

    if (audioStream >= 0) {
        AVCodecParameters* aParams = fmt->streams[audioStream]->codecpar;
        const AVCodec* aCodec = avcodec_find_decoder(aParams->codec_id);

        int64_t aBitRate = aParams->bit_rate;
        if (aBitRate == 0 && durationSec > 0) {
            aBitRate = static_cast<int64_t>((audioSize * 8) / durationSec);
        }

        _originalFile.hasAStream = true;
        _originalFile.duration = durationSec;
        _originalFile.aSize = (audioSize)
            ? static_cast<int64_t>(audioSize / (1024.0 * 1024.0))
            : 0;
        _originalFile.aBitRate = aBitRate/1000;
        _originalFile.aDecoder = (aCodec) ? aCodec->name : "NA";
        _originalFile.aCodec = avcodec_get_name(aParams->codec_id);
    }

    avformat_close_input(&fmt);
    return true;
}