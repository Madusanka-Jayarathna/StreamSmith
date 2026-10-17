#pragma once

#include <QObject>
#include <QWidget>

class DownloaderViewUI;
class DownloaderViewCommands;

class DownloaderView : public QWidget {
    Q_OBJECT

public:
    explicit DownloaderView(QWidget* parent = nullptr);

signals:
    void backRequested();

private:
    void setupView();

private:
    DownloaderViewUI* m_ui{nullptr};
    DownloaderViewCommands* m_commands{nullptr};
};