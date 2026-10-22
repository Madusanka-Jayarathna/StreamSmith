#include "libConverter/public/ConverterApp.h"
#include "libUtils/public/Utils.h"
#include <nlohmann/json.hpp>
#include <boost/filesystem.hpp>

using json = nlohmann::json;
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
}

ConverterApp::ConverterApp( void ) :
    _mediaFile("")
{
    //
}

void
ConverterApp::run() {
    showProductTitle();
    getFilePath();
    analyzeMediaFile();
    //startDownload();
}

void
ConverterApp::showProductTitle(){
    std::cout << "\n";
    std::cout << "--------------------------------------------------" << "\n";
    std::cout << "                  Converter                       " << "\n";    
    std::cout << "--------------------------------------------------" << "\n\n";
}

void
ConverterApp::getFilePath() {
    const std::string msg = "Enter Media Path: ";
    bool success = false;
    std::string inStr;
    while(!success) {
        inStr = Utils::getUserInput(msg);
        success = Utils::analyzeFile(inStr);
        if(!success) {
            std::cout << "ERROR     : Invalid Media File: " << inStr << std::endl << std::endl;
        } else {
            std::cout << "STATUS    : Media File Selected: " << inStr << std::endl << std::endl;
        } 
    }
    _mediaFile = inStr;
}

void
ConverterApp::analyzeMediaFile(){
    if (_mediaFile.empty()) {
        std::cout << "ERROR     : Invalid File: " << _mediaFile << "\n";
        return;
    }

    const char* fileName = _mediaFile.c_str();
    AVFormatContext* fmt = nullptr;
    if (avformat_open_input(&fmt, fileName, nullptr, nullptr) < 0) {
        std::cout << "ERROR     : Could not open media file " << fileName << "\n";
        return;
    }

    if (avformat_find_stream_info(fmt, nullptr) < 0) {
        std::cout << "ERROR     : Could not retrieve stream info\n";
        avformat_close_input(&fmt);
        return;
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

    std::cout << "================ MEDIA ANALYZER ================\n";
    std::cout << "File: " << fileName << "\n";
    std::cout << "Total File Size : " << (totalSize > 0 ? std::to_string(totalSize) + " bytes (" + std::to_string(totalSize / (1024.0 * 1024.0)) + " MB)" : "Unknown") << "\n\n";

    if (videoStream >= 0) {
        AVCodecParameters* vParams = fmt->streams[videoStream]->codecpar;
        const AVCodec* vCodec = avcodec_find_decoder(vParams->codec_id);

        int64_t vBitRate = vParams->bit_rate;
        if (vBitRate == 0 && fmt->duration > 0) {
            double durationSec = static_cast<double>(fmt->duration) / AV_TIME_BASE;
            vBitRate = static_cast<int64_t>((videoSize * 8) / durationSec);
        }

        std::cout << "--- Video Stream ---\n";
        std::cout << "Codec      : " << (vCodec ? vCodec->name : "Unknown") << "\n";
        std::cout << "Bit Rate   : " << vBitRate / 1000 << " kbps\n";
        std::cout << "Data Size  : " << videoSize << " bytes (" << std::fixed << std::setprecision(2) << (videoSize / (1024.0 * 1024.0)) << " MB)\n\n";
    } else {
        std::cout << "--- Video Stream ---\nNot Present\n\n";
    }

    if (audioStream >= 0) {
        AVCodecParameters* aParams = fmt->streams[audioStream]->codecpar;
        const AVCodec* aCodec = avcodec_find_decoder(aParams->codec_id);

        int64_t aBitRate = aParams->bit_rate;
        if (aBitRate == 0 && fmt->duration > 0) {
            double durationSec = static_cast<double>(fmt->duration) / AV_TIME_BASE;
            aBitRate = static_cast<int64_t>((audioSize * 8) / durationSec);
        }

        std::cout << "--- Audio Stream ---\n";
        std::cout << "Codec      : " << (aCodec ? aCodec->name : "Unknown") << "\n";
        std::cout << "Bit Rate   : " << aBitRate / 1000 << " kbps\n";
        std::cout << "Data Size  : " << audioSize << " bytes (" << std::fixed << std::setprecision(2) << (audioSize / (1024.0 * 1024.0)) << " MB)\n";
    } else {
        std::cout << "--- Audio Stream ---\nNot Present\n";
    }

    avformat_close_input(&fmt);
    return;
}