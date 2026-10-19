// DownloaderViewCommands.cpp
#include "libDownloader/public/DownloaderViewCommands.h"

DownloaderViewCommands::DownloaderViewCommands(QObject* parent) : QObject(parent) {
    m_process = new QProcess(this);
    
    connect(m_process, &QProcess::readyReadStandardOutput, [this]() {
        QByteArray output = m_process->readAllStandardOutput();
        // Parsing output percentage from yt-dlp stdout
        emit progressUpdated(50); // Example parsed value
        emit statusUpdated("Downloading video segment...");
    });
}

void DownloaderViewCommands::executeDownload(const QString& videoUrl) {
    emit statusUpdated("Starting yt-dlp...");
    // Example: m_process->start("yt-dlp", QStringList() << videoUrl);
}
