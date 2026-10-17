// DownloaderViewUI.h
#pragma once
#include <QWidget>

class QProgressBar;
class QLabel;
class QPushButton;

class DownloaderViewUI : public QWidget {
    Q_OBJECT
public:
    explicit DownloaderViewUI(QWidget* parent = nullptr);
    
    void updateProgress(int percent);
    void updateStatus(const QString& statusText);

signals:
    void backButtonClicked();
    void startProcessClicked();

private:
    void setupUI();
    
    QLabel* m_statusLabel{nullptr};
    QProgressBar* m_progressBar{nullptr};
    QPushButton* m_backBtn{nullptr};
    QPushButton* m_startBtn{nullptr};
};
