// DownloaderViewUI.cpp
#include "libDownloader/public/DownloaderViewUI.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>

DownloaderViewUI::DownloaderViewUI(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void DownloaderViewUI::setupUI() {
    auto* layout = new QVBoxLayout(this);

    m_statusLabel = new QLabel("Ready to start process...", this);
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);

    m_startBtn = new QPushButton("Start Processing", this);
    m_backBtn = new QPushButton("Back to Main", this);

    layout->addWidget(m_statusLabel);
    layout->addWidget(m_progressBar);
    layout->addWidget(m_startBtn);
    layout->addWidget(m_backBtn);

    connect(m_startBtn, &QPushButton::clicked, this, &DownloaderViewUI::startProcessClicked);
    connect(m_backBtn, &QPushButton::clicked, this, &DownloaderViewUI::backButtonClicked);
}

void DownloaderViewUI::updateProgress(int percent) {
    m_progressBar->setValue(percent);
}

void DownloaderViewUI::updateStatus(const QString& statusText) {
    m_statusLabel->setText(statusText);
}
