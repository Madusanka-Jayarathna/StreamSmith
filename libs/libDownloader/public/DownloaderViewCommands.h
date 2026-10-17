// DownloaderViewCommands.h
#pragma once
#include <QObject>
#include <QProcess>

class DownloaderViewCommands : public QObject {
    Q_OBJECT
public:
    explicit DownloaderViewCommands(QObject* parent = nullptr);
    void executeDownload(const QString& videoUrl);

signals:
    void progressUpdated(int percent);
    void statusUpdated(const QString& status);
    void processFinished(bool success);

private:
    QProcess* m_process{nullptr};
};
