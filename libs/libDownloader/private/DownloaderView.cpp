#include "libDownloader/public/DownloaderView.h"
#include "libDownloader/public/DownloaderViewUI.h"
#include "libDownloader/public/DownloaderViewCommands.h"
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>

DownloaderView::DownloaderView(QWidget* parent): 
    QWidget(parent) 
    {
        //setWindowTitle("Advanced Media Downloader & Converter");
        //resize(740, 630);
        //setFixedSize(740, 630);
        setupView();
    }



void
DownloaderView::setupView() {
    auto* layout = new QVBoxLayout(this);

    layout->setContentsMargins(0, 0, 0, 0);

    // 1. Instantiate UI and Processing components
    m_ui = new DownloaderViewUI(this);
    m_commands = new DownloaderViewCommands(this);

    layout->addWidget(m_ui);

    // 2. Connect UI interactions to Logic Execution
    connect(m_ui, &DownloaderViewUI::startProcessClicked, [this]() {
        m_commands->executeDownload("https://example.com/video");
    });

    // 3. Connect Logic Output back to UI Display
    connect(m_commands, &DownloaderViewCommands::progressUpdated, m_ui, &DownloaderViewUI::updateProgress);
    connect(m_commands, &DownloaderViewCommands::statusUpdated, m_ui, &DownloaderViewUI::updateStatus);

    // 4. Forward Navigation Signal to MainApp
    connect(m_ui, &DownloaderViewUI::backButtonClicked, this, &DownloaderView::backRequested);


    //auto* titleLabel = new QLabel("media donwloader", this);
    //titleLabel->setFont(QFont("Arial", 16, QFont::Bold));
    //titleLabel->setAlignment(Qt::AlignCenter);
    //mainLayout->addWidget(titleLabel);

    //auto* progressBar = new QProgressBar(this);
    //progressBar->setRange(0, 100);
    //progressBar->setValue(45); // Example static value
    //mainLayout->addWidget(progressBar);

    //auto* backBtn = new QPushButton("Back to Main View", this);
    //backBtn->setFixedHeight(40);
    
    //// Emit signal to notify controller to return and destroy this view
    //connect(backBtn, &QPushButton::clicked, this, &DownloaderView::backRequested);
    
    //mainLayout->addWidget(backBtn);

}