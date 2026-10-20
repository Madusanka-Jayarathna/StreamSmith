#include "libDownloader/public/DownloaderApp.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

DownloaderApp::DownloaderApp( void )
{
    _videoFormats.clear();
    _audioFormats.clear(); 
    _url("");
    _downloadFolder("~/Downloads"); 
}

void
DownloaderApp::run() {
    showProduct();
    getURL();
    startDonwload();
    //showAvailableFormats();
}



void
DownloaderApp::showProduct(){
    std::cout << "--------------------------------------------------" << "\n";
    std::cout << "                  Downloader                      " << "\n";    
    std::cout << "--------------------------------------------------" << "\n\n";
}

bool
DownloaderApp::showAvailableFormats(){
    if (!_videoFormats.empty() || !_audioFormats.empty()) {
        std::cout << "\n=== Video Formats Found (" << _videoFormats.size() << ") ===\n";
        for (const auto& v : _videoFormats) {
            std::cout << "ID: " << v.formatID 
                      << " | Res: " << v.resolutionOrBitRate 
                      << " | Ext: " << v.extension 
                      << " | Bitrate: " << v.bitRate << "kbps"
                      << " | Size: " << v.getSize() << "\n";
        }

        std::cout << "\n=== Audio Formats Found (" << _audioFormats.size() << ") ===\n";
        for (const auto& a : _audioFormats) {
            std::cout << "ID: " << a.formatID 
                      << " | Ext: " << a.extension 
                      << " | Bitrate: " << a.resolutionOrBitRate 
                      << " | Size: " << a.getSize() << "\n";
        }
    }
    return true;
}

void
DownloaderApp::getURL() {
    std::string rawInput;
    bool success = false;
    while(!success) {
        std::cout << "Enter URL: ";
        std::getline(std::cin, rawInput);
        success = analyzeURL(rawInput);
    }
    _url = rawInput;
}

bool 
DownloaderApp::isValidUrlSyntax(const std::string& url) {
    if (url.empty()) return false;
    return (url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0);
}

std::pair<int, std::string> 
DownloaderApp::executeCommand(const std::string& command) {
    std::string output;
    char buffer[256];

    FILE* pipe = POPEN(command.c_str(), "r");
    if (!pipe) {
        return {-1, ""};
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output += buffer;
    }

    int exitCode = PCLOSE(pipe);
    return {exitCode, output};
}



bool
DownloaderApp::analyzeURL(const std::string& url) {
    _videoFormats.clear();
    _audioFormats.clear();

    if (!isValidUrlSyntax(url)) {
        std::cout << "invalid URL\n";
        return false;
    }

    std::string command = "yt-dlp --dump-json --quiet --no-warnings --socket-timeout 15 --no-playlist --ies \"all,-generic\" \"" + url + "\"";

    auto [exitCode, jsonOutput] = executeCommand(command);

    if (exitCode != 0 || jsonOutput.empty()) {
        std::cout << "unable extract media information\n";
        return false;
    }

    try {
        json parsedData = json::parse(jsonOutput);

        if (!parsedData.contains("formats") || !parsedData["formats"].is_array()) {
            std::cout << "unable extract media information\n";
            return false;
        }

        for (const auto& fmt : parsedData["formats"]) {
            std::string formatId = fmt.value("format_id", "N/A");
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

                _videoFormats.push_back(vInfo);
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

                _audioFormats.push_back(aInfo);
            }
        }

        if (_videoFormats.empty() && _audioFormats.empty()) {
            std::cout << "unable extract media information\n";
            return false;
        }
        return true;

    } catch (const json::parse_error&) {
        std::cout << "unable extract media information\n";
    }
    return false;
}

bool 
DownloaderApp::executeDownload(const std::string& command) {
    char buffer[256];
    FILE* pipe = POPEN(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Unable to spawn download process.\n";
        return false;
    }

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        std::cout << buffer; 
    }

    int exitCode = PCLOSE(pipe);
    return (exitCode == 0);
}

bool 
DownloaderApp::DownloadMedia(DownloadMode mode, const std::string& videoId = "", const std::string& audioId = "") {
    if (_url.empty() || _downloadFolder.empty()) {
        std::cout << "Status: Invalid URL or output folder.\n";
        return;
    }

    std::string outTemplate = _downloadFolder + "/%(title)s.%(ext)s";
    std::string command = "yt-dlp --newline --no-warnings --socket-timeout 15 -o \"" + outTemplate + "\" ";

    switch (mode) {
        case DownloaderApp::DownloadMode::HighestQuality:
            command += "-f \"bestvideo+bestaudio/best\" --merge-output-format mp4 ";
            break;

        case DownloaderApp::DownloadMode::VideoOnly:
            if (videoId.empty()) {
                std::cout << "Error: Video ID is required for Video Only download.\n";
                return false;
            }
            command += "-f \"" + videoId + "\" ";
            break;

        case DownloaderApp::DownloadMode::AudioOnly:
            if (audioId.empty()) {
                std::cout << "Error: Audio ID is required for Audio Only download.\n";
                return false;
            }
            command += "-f \"" + audioId + "\" -x --audio-format mp3 --audio-quality 192K ";
            break;

        case DownloaderApp::DownloadMode::CustomCombine:
            if (videoId.empty() || audioId.empty()) {
                std::cout << "Error: Both Video ID and Audio ID are required for Custom download.\n";
                return false;
            }
            command += "-f \"" + videoId + "+" + audioId + "\" --merge-output-format mkv ";
            break;
    }

    command += "\"" + _url + "\"";

    std::cout << "\nExecuting Command:\n" << command << "\n\n";

    bool success = executeDownload(command);

    if (success) {
        std::cout << "\nStatus: Download Complete!\n";
    } else {
        std::cout << "\nStatus: Download Failed!\n";
    }

    return success;
}

void
DownloaderApp::startDownload() {
    std::string rawInput;
    int validMode = 0;
    bool isValid = false;

    while(!isValid) {
        std::cout << "\tDownload Modes\n";
        std::cout << "\t 1 - Best Quality \n";
        std::cout << "\t 2 - Video Only \n";
        std::cout << "\t 3 - Audio Only \n";
        std::cout << "\t 4 - Custom \n\n";

        std::cout << "select Download Mode: ";
        std::getline(std::cin, rawInput);

        try{
            size_t chars;
            validMode = std::stoi(rawInput, &chars);

            if(chars < rawInput.size() || validMode > 4 || validMode < 1){
                throw std::invalid_argument("ERROR      : Invalid Mode selected\n");
            } else {
                isValid = true;
            }
        }
        catch (const std::invalid_argument& e) {
            std::cout <<  e.what() << "\n";
        }
        catch (const std::out_of_range& e) {
            std::cout << "ERROR      : Invalid Mode selected\n\n";
        }
    }

    if (validMode == 1) {
        std::cout 
    }


}